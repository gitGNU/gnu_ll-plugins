/****************************************************************************
    
    lv2host.cpp - Simple LV2 plugin loader for Elven
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA

****************************************************************************/

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <turtleparser.hpp>
#include <query.hpp>
#include <namespaces.hpp>

#include "lv2host.hpp"
#include "lv2-midiport.h"
#include "lv2-midifunctions.h"
#include "debug.hpp"
#include "midiutils.hpp"


using namespace std;
using namespace PAQ;
using namespace sigc;


LV2Host::LV2Host(const string& uri, unsigned long frame_rate) 
  : m_uri(uri),
    m_rate(frame_rate),
    m_handle(0),
    m_desc(0),
    m_comm_desc(0),
    m_midimap(128, -1),
    m_ports_updated(false) {
  
  DBG2("Creating plugin loader...");
  
  pthread_mutex_init(&m_mutex, 0);
  
  sem_init(&m_notification_sem, 0, 0);
  
  m_comm_host_desc.feedback = &LV2Host::feedback_wrapper;
  m_comm_host_desc.host_data = this;
  
  // get the directories to look in
  vector<string> search_dirs = get_search_dirs();
  
  // iterate over all directories
  scan_manifests(search_dirs, mem_fun(*this, &LV2Host::match_uri));
  
  // if we didn't find it, assume that we were given a partial URI
  // XXX lots of scanning here, we shouldn't need to do 3 passes
  if (m_rdffiles.size() == 0) {
    DBG1("Did not find a complete match, looking for partial match");
    if (scan_manifests(search_dirs, 
		       mem_fun(*this, &LV2Host::match_partial_uri))) {
      DBG2("Found matching URI, re-scanning manifests");
      scan_manifests(search_dirs, mem_fun(*this, &LV2Host::match_uri));
    }
  }
  
  load_plugin();
}
  
  
LV2Host::~LV2Host() {
  if (m_handle) {
    DBG2("Destroying plugin instance");
    if (m_desc->cleanup)
      m_desc->cleanup(m_handle);
    dlclose(m_libhandle);
    sem_destroy(&m_notification_sem);
  }
}


const std::string& LV2Host::get_plugin_uri() const {
  return m_uri;
}


void LV2Host::list_plugins() {
  vector<string> search_dirs = get_search_dirs();
  scan_manifests(search_dirs, &LV2Host::print_uri);
}


void LV2Host::run_main() {
  if (!sem_trywait(&m_notification_sem)) {
    DBG2("Semaphore posted");
    while (!sem_trywait(&m_notification_sem));
    for (unsigned i = 0; i < m_ports.size(); ++i) {
      if (m_ports[i].notify && m_ports[i].direction == OutputPort) {
	signal_port_event(i, sizeof(float), &m_ports[i].old_value);
	DBG2("Sending port event for output port "<<i);
      }
    }
  }
}


bool LV2Host::is_valid() const {
  return (m_handle != 0);
}


const vector<LV2Port>& LV2Host::get_ports() const {
  assert(m_handle != 0);
  return m_ports;
}


vector<LV2Port>& LV2Host::get_ports() {
  assert(m_handle);
  return m_ports;
}


long LV2Host::get_default_midi_port() const {
  return m_default_midi_port;
}


void LV2Host::activate() {
  assert(m_handle);
  DBG2("Activating plugin instance");
  if (m_desc->activate)
    m_desc->activate(m_handle);
}


void LV2Host::run(unsigned long nframes) {
  
  assert(m_handle);
  
  for (size_t i = 0; i < m_ports.size(); ++i)
    m_desc->connect_port(m_handle, i, m_ports[i].buffer);
  
  if (!pthread_mutex_trylock(&m_mutex)) {
    
    // copy any updated control port values into the port buffers
    if (m_ports_updated) {
      m_ports_updated = false;
      for (unsigned i = 0; i < m_ports.size(); ++i) {
	if (m_ports[i].type == ControlType && m_ports[i].direction == InputPort)
	  memcpy(m_ports[i].buffer, &m_ports[i].value, sizeof(float));
      }
    }
    
    // read MIDI events from the vector
    for (unsigned i = 0; i < m_midi_events.size(); ++i) {
      if (m_midi_events[i]->written)
	continue;
      MidiEvent* e = m_midi_events[i];
      DBG3("Received MIDI event from the main thread for port "
	   <<m_ports[e->port].symbol<<": "
	   <<midi2str(e->event_size, e->data));
      LV2_MIDI* midi = static_cast<LV2_MIDI*>(m_ports[e->port].buffer);
      LV2_MIDIState state = { midi, nframes, 0 };
      // XXX DANGER! Can't set the timestamp to 0 since other events may
      // have been written to the buffer already!
      lv2midi_put_event(&state, 0, e->event_size, e->data);
      e->written = true;
    }
    
    pthread_mutex_unlock(&m_mutex);
  }
  
  m_desc->run(m_handle, nframes);
  
  // send port notifications
  for (unsigned i = 0; i < m_ports.size(); ++i) {
    if (m_ports[i].notify && 
	m_ports[i].old_value != *static_cast<float*>(m_ports[i].buffer))
      sem_post(&m_notification_sem);
    m_ports[i].old_value = *static_cast<float*>(m_ports[i].buffer);
  }
}


void LV2Host::deactivate() {
  assert(m_handle);
  DBG2("Deactivating the plugin instance");
  if (m_desc->deactivate)
    m_desc->deactivate(m_handle);
}


char* LV2Host::command(uint32_t argc, const char* const* argv) {

  cerr<<__PRETTY_FUNCTION__<<endl;
  cerr<<"POINTERS:"<<endl;
  for (unsigned i = 0; i < argc; ++i)
    cerr<<"  "<<(const void*)(argv[i])<<endl;

  DBG2("Calling command() with "<<argc<<" parameters:");
  for (unsigned i = 0; i < argc; ++i)
    DBG2("  '"<<argv[i]<<"'");
  if (m_comm_desc && m_comm_desc->command) {
    char* result = m_comm_desc->command(m_handle, argc, argv);
    if (result) {
      DBG0("ERROR REPORTED BY PLUGIN: "<<result);
      DBG0("  for command:");
      for (unsigned i = 0; i < argc; ++i)
	DBG0("'"<<argv[i]<<"'");
    }
    return result;
  }
  else
    DBG0("This plugin has no command() callback");

  return 0;
  
}


void LV2Host::write_port(uint32_t index, uint32_t buffer_size, 
			 const void* buffer) {
  if (index < m_ports.size() && m_ports[index].direction == InputPort) {
    if (m_ports[index].type == ControlType)
      set_control(index, *static_cast<const float*>(buffer));
    else if (m_ports[index].type == MidiType)
      queue_midi(index, buffer_size, static_cast<const unsigned char*>(buffer));
    else
      DBG0("Tried to write to port that is not a control or MIDI port");
  }
  else
    DBG0("Tried to write to invalid input port");
}


void LV2Host::set_control(uint32_t index, float value) {
  if (index < m_ports.size() && m_ports[index].type == ControlType &&
      m_ports[index].direction == InputPort) {
    pthread_mutex_lock(&m_mutex);
    m_ports[index].value = value;
    m_ports_updated = true;
    pthread_mutex_unlock(&m_mutex);
    signal_port_event(index, sizeof(float), &value);
  }
}


void LV2Host::set_program(unsigned char program) {
  std::map<unsigned char, LV2Preset>::const_iterator iter = m_presets.find(program);
  if (iter != m_presets.end()) {
    signal_program_changed(program);
    const std::map<uint32_t, float>& preset = iter->second.values;
    std::map<uint32_t, float>::const_iterator piter;
    for (piter = preset.begin(); piter != preset.end(); ++piter) {
      if (piter->first < m_ports.size() && 
	  m_ports[piter->first].type == ControlType &&
	  m_ports[piter->first].direction == InputPort)
	set_control(piter->first, piter->second);
    }
  }
}


void LV2Host::save_program(unsigned char program, const char* name) {
  if (program > 127) {
    DBG0("Can not save program with number "<<int(program));
    return;
  }
  LV2Preset preset;
  preset.name = name;
  for (unsigned i = 0; i < m_ports.size(); ++i) {
    if (m_ports[i].type == ControlType && m_ports[i].direction == InputPort)
      preset.values[i] = m_ports[i].value;
  }
  DBG2("Program \""<<name<<"\" added with number "<<int(program));
  DBG0("WARNING: Saved programs are not persistent!");
  m_presets[program] = preset;
  signal_program_added(program, name);
  signal_program_changed(program);
}


const std::vector<int>& LV2Host::get_midi_map() const {
  return m_midimap;
}


const std::string& LV2Host::get_icon_path() const {
  return m_iconpath;
}


const std::string& LV2Host::get_gui_path() const {
  return m_plugingui;
}


const std::string& LV2Host::get_gui_uri() const {
  return m_guiuri;
}


const std::string& LV2Host::get_bundle_dir() const {
  return m_bundledir;
}


const std::string& LV2Host::get_name() const {
  return m_name;
}


void LV2Host::queue_midi(uint32_t port, uint32_t size, 
                         const unsigned char* midi) {
  if (port < m_ports.size() && m_ports[port].type == MidiType && 
      m_ports[port].direction == InputPort) {
    if (size <= 4) {
      DBG2("Queueing MIDI event for port "<<port);
      pthread_mutex_lock(&m_mutex);
      unsigned i;
      for (i = 0; i < m_midi_events.size(); ++i) {
	if (!m_midi_events[i]->written)
	  break;
	delete m_midi_events[i]->data;
	delete m_midi_events[i];
      }
      m_midi_events.erase(m_midi_events.begin(), m_midi_events.begin() + i);
      m_midi_events.push_back(new MidiEvent(port, size, midi));
      pthread_mutex_unlock(&m_mutex);
    }
    else
      DBG0("Can only write MIDI events smaller than 5 bytes");
  }
  else
    DBG0("Trying to write MIDI to invalid port "<<port);
}


const std::map<unsigned char, LV2Preset>& LV2Host::get_presets() const {
  return m_presets;
}


std::vector<std::string> LV2Host::get_search_dirs() {
  vector<string> search_dirs;
  const char* lv2p_c = getenv("LV2_PATH");
  if (!lv2p_c) {
    DBG1("LV2_PATH is not defined, will search default directories");
    search_dirs.push_back(string(getenv("HOME")) + "/.lv2");
    search_dirs.push_back("/usr/lib/lv2");
    search_dirs.push_back("/usr/local/lib/lv2");
  }
  else {
    string lv2_path = lv2p_c;
    int split;
    while ((split = lv2_path.find(':')) != (int)string::npos) {
      search_dirs.push_back(lv2_path.substr(0, split));
      lv2_path = lv2_path.substr(split + 1);
    }
    search_dirs.push_back(lv2_path);
  }
  
  return search_dirs;
}


bool LV2Host::scan_manifests(const std::vector<std::string>& search_dirs, 
                             scan_callback_t callback) {
  
  bool done = false;
  
  for (unsigned i = 0; i < search_dirs.size(); ++i) {
    
    DBG2("Searching "<<search_dirs[i]);
    
    // open the directory
    DIR* d = opendir(search_dirs[i].c_str());
    if (d == 0) {
      DBG1("Could not open "<<search_dirs[i]<<": "<<strerror(errno));
      continue;
    }
    
    // iterate over all directory entries
    dirent* e;
    while ((e = readdir(d))) {

      std::string plugindir;
      std::string ttlfile;
      std::string library;
      
      vector<QueryResult> qr;

      if (strlen(e->d_name) >= 4) {
        
        // is it named like an LV2 bundle?
        if (strcmp(e->d_name + (strlen(e->d_name) - 4), ".lv2"))
          continue;
        
        // is it actually a directory?
        plugindir = search_dirs[i] + "/" + e->d_name + "/";
        struct stat stat_info;
        if (stat(plugindir.c_str(), &stat_info)) {
          DBG1("Could not get file information about "<<plugindir);
          continue;
        }
        if (!S_ISDIR(stat_info.st_mode)) {
          DBG1(plugindir<<" is not a directory");
          continue;
        }
        
        // execute the callback
        ttlfile = plugindir + "manifest.ttl";
	struct stat buf;
	if (stat(ttlfile.c_str(), &buf) != ENOENT) {
	  if (callback(plugindir)) {
	    done = true;
	    break;
	  }
        }
	
      }
    }
    
    closedir(d);
    
    if (done)
      break;
  }
  
  return done;
}

 
bool LV2Host::match_uri(const string& bundle) {
  
  // parse
  TurtleParser tp;
  RDFData data;
  if (!tp.parse_ttl_file(bundle + "manifest.ttl", data)) {
    DBG1("Could not parse "<<bundle<<"manifest.ttl");
    return false;
  }
  
  // query
  Namespace lv2("<http://lv2plug.in/ns/lv2core#>");
  Variable datafile;
  string uriref = string("<") + m_uri + ">";
  vector<QueryResult> qr = select(datafile)
    .where(uriref, rdfs("seeAlso"), datafile)
    .run(data);
  for (unsigned i = 0; i < qr.size(); ++i) {
    const string& str = qr[i][datafile]->name;
    if (str.size() >= 10 && str.substr(0, 9) == "<file:///") {
      m_rdffiles.push_back(str.substr(8, str.size() - 9));
      DBG2("Found datafile "<<m_rdffiles[m_rdffiles.size() - 1]);
    }
    else
      DBG1("Unknown URI type: "<<qr[i][datafile]->name);
  }
  qr = select(datafile)
    .where(uriref, rdf("type"), lv2("Plugin"))
    .run(data);
  if (qr.size() > 0) {
    DBG2("Found datafile "<<bundle<<"manifest.ttl");
    m_rdffiles.push_back(bundle + "manifest.ttl");
    m_bundle = bundle;
  }

  return false;
}


bool LV2Host::match_partial_uri(const string& bundle) {
  
  // parse
  TurtleParser tp;
  RDFData data;
  if (!tp.parse_ttl_file(bundle + "manifest.ttl", data)) {
    DBG1("Could not parse "<<bundle<<"manifest.ttl");
    return false;
  }
  
  // query
  Namespace lv2("<http://lv2plug.in/ns/lv2core#>");
  Variable uriref;
  vector<QueryResult> qr = select(uriref)
    .where(uriref, rdf("type"), lv2("Plugin"))
    .run(data);
  for (unsigned i = 0; i < qr.size(); ++i) {
    if (qr[i][uriref]->name.find(m_uri) != string::npos) {
      m_uri = qr[i][uriref]->name;
      m_uri = m_uri.substr(1, m_uri.size() - 2);
      DBG2("Found matching URI: "<<m_uri);
      return true;
    }
  }

  return false;
}


bool LV2Host::load_plugin() {
  
  bool usesCommands = false;
  
  DBG2(__PRETTY_FUNCTION__);
  
  // parse the datafile to get port info
  {
    
    vector<QueryResult> qr;
    string uriref = string("<") + m_uri + ">";
    
    // parse all data files
    RDFData data;
    TurtleParser tp;
    for (unsigned i = 0; i < m_rdffiles.size(); ++i) {
      DBG2("Parsing "<<m_rdffiles[i]);
      if (!tp.parse_ttl_file(m_rdffiles[i], data)) {
	DBG0("Failed to parse "<<m_rdffiles[i]);
	return false;
      }
    }
    
    Namespace lv2("<http://lv2plug.in/ns/lv2core#>");
    Namespace ll("<http://ll-plugins.nongnu.org/lv2/namespace#>");
    Namespace llext("<http://ll-plugins.nongnu.org/lv2/ext/>");
    Namespace mm("<http://ll-plugins.nongnu.org/lv2/ext/midimap#>");
    Variable name, license, binary;
    qr = select(name, binary)
      .where(uriref, rdf("type"), lv2("Plugin"))
      .where(uriref, lv2("binary"), binary)
      .where(uriref, doap("name"), name)
      .where(uriref, doap("license"), license)
      .run(data);
    if (qr.size() > 0) {
      m_name = qr[0][name]->name;
      m_binary = qr[0][binary]->name;
    }
    else {
      DBG0("A required property (binary, name or license) is missing");
      return false;
    }
    
    Variable symbol, index, portclass, porttype, port;
    qr = select(index, symbol, portclass, porttype)
      .where(uriref, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("symbol"), symbol)
      .run(data);

    if (qr.size() == 0) {
      DBG1("Can not find any ports for <"<<m_uri<<">");
      return false;
    }
    
    m_ports.resize(qr.size());
    
    for (unsigned j = 0; j < qr.size(); ++j) {
      
      // index
      size_t p = atoi(qr[j][index]->name.c_str());
      if (p >= m_ports.size()) {
        DBG0("Found port with index "<<p<<", but there are only "
             <<m_ports.size()<<" ports in total");
        return false;
      }
      
      // symbol
      m_ports[p].symbol = qr[j][symbol]->name;
      DBG2("Found port "<<m_ports[p].symbol);
      
      // direction and type
      m_ports[p].direction = NoDirection;
      m_ports[p].type = NoType;
      vector<QueryResult> qr2 = select(portclass)
	.where(uriref, lv2("port"), port)
	.where(port, lv2("index"), qr[j][index]->name)
	.where(port, rdf("type"), portclass)
	.run(data);
      
      for (unsigned k = 0; k < qr2.size(); ++k) {
	string pclass = qr2[k][portclass]->name;
	DBG2(m_ports[p].symbol<<" is an "<<pclass);
	if (pclass == lv2("InputPort"))
	  m_ports[p].direction = InputPort;
	else if (pclass == lv2("OutputPort"))
	  m_ports[p].direction = OutputPort;
	else if (pclass == lv2("AudioPort"))
	  m_ports[p].type = AudioType;
	else if (pclass == llext("MidiPort"))
	  m_ports[p].type = MidiType;
	else if (pclass == lv2("ControlPort"))
	  m_ports[p].type = ControlType;
	else
	  DBG1("Unknown port class: "<<pclass);
      }
      
      if (m_ports[p].direction == NoDirection) {
	DBG0("No direction given for port "<<m_ports[p].symbol);
	return false;
      }
	
      if (m_ports[p].type == NoType) {
	DBG0("No known data type given for port "<<m_ports[p].symbol);
	return false;
      }
	
      m_ports[p].default_value = 0;
      m_ports[p].min_value = 0;
      m_ports[p].max_value = 1;
      //m_ports[p].notify = (m_ports[p].direction == InputPort &&
      //			   m_ports[p].type == ControlType);
      m_ports[p].notify = (m_ports[p].type == ControlType);

    }
    
    // default values
    Variable default_value;
    qr = select(index, default_value)
      .where(uriref, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("default"), default_value)
      .run(data);
    for (unsigned j = 0; j < qr.size(); ++j) {
      unsigned p = atoi(qr[j][index]->name.c_str());
      if (p >= m_ports.size())
        return false;
      m_ports[p].default_value = 
        atof(qr[j][default_value]->name.c_str());
    }
    
    // minimum values
    Variable min_value;
    qr = select(index, min_value)
      .where(uriref, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("minimum"), min_value)
      .run(data);
    for (unsigned j = 0; j < qr.size(); ++j) {
      unsigned p = atoi(qr[j][index]->name.c_str());
      if (p >= m_ports.size())
        return false;
      m_ports[p].min_value = 
        atof(qr[j][min_value]->name.c_str());
    }
    
    // maximum values
    Variable max_value;
    qr = select(index, max_value)
      .where(uriref, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("maximum"), max_value)
      .run(data);
    for (unsigned j = 0; j < qr.size(); ++j) {
      unsigned p = atoi(qr[j][index]->name.c_str());
      if (p >= m_ports.size())
        return false;
      m_ports[p].max_value = 
        atof(qr[j][max_value]->name.c_str());
      DBG2(m_ports[p].symbol<<" has the range ["<<m_ports[p].min_value
           <<", "<<m_ports[p].max_value<<"]");
    }
    
    // check range sanity
    for (unsigned long i = 0; i < m_ports.size(); ++i) {
      if (m_ports[i].direction == OutputPort)
        continue;
      if (m_ports[i].min_value > m_ports[i].default_value)
        m_ports[i].min_value = m_ports[i].default_value;
      if (m_ports[i].max_value < m_ports[i].default_value)
        m_ports[i].max_value = m_ports[i].default_value;
      if (m_ports[i].max_value - m_ports[i].min_value <= 0)
        m_ports[i].max_value = m_ports[i].min_value + 10;
    }
    
    // MIDI controller bindings
    Variable controller, cc_number;
    qr = select(index, cc_number)
      .where(uriref, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, mm("defaultMidiController"), controller)
      .where(controller, mm("controllerType"), mm("CC"))
      .where(controller, mm("controllerNumber"), cc_number)
      .run(data);
    for (unsigned j = 0; j < qr.size(); ++j) {
      unsigned p = atoi(qr[j][index]->name.c_str());
      unsigned cc = atoi(qr[j][cc_number]->name.c_str());
      if (p < m_ports.size() && cc < 128)
        m_midimap[cc] = p;
    }
    
    // default MIDI port
    qr = select(index)
      .where(uriref, mm("defaultMidiPort"), index)
      .run(data);
    if (qr.size() > 0)
      m_default_midi_port = atoi(qr[0][index]->name.c_str());
    else {
      m_default_midi_port = -1;
      for (unsigned long i = 0; i < m_ports.size(); ++i) {
        if (m_ports[i].type == MidiType && m_ports[i].direction == InputPort) {
          m_default_midi_port = i;
          break;
        }
      }
    }
    
    // is the command extension used?
    Variable extension_predicate;
    qr = select(extension_predicate)
      .where(uriref, extension_predicate, ll("dont-use-this-extension"))
      .run(data);
    if (qr.size() > 0 && 
	(qr[0][extension_predicate]->name == lv2("requiredFeature") ||
	 qr[0][extension_predicate]->name == lv2("optionalFeature"))) {
      usesCommands = true;
      DBG2("This plugin uses the command extension");
    }
    
    // GUI plugin path
    Variable gui_uri, gui_path;
    Namespace gg("<http://ll-plugins.nongnu.org/lv2/ext/ui#>");
    qr = select(gui_uri, gui_path)
      .where(uriref, gg("ui"), gui_uri)
      .where(gui_uri, gg("binary"), gui_path)
      .where(gui_uri, rdf("type"), gg("GtkUI"))
      .run(data);
    if (qr.size() > 0) {
      m_guiuri = qr[0][gui_uri]->name.
	substr(1, qr[0][gui_uri]->name.length() - 2);
      m_plugingui = qr[0][gui_path]->name;
      m_plugingui = m_plugingui.substr(8, m_plugingui.size() - 9);
      DBG2("Found GUI plugin file "<<m_plugingui);
    }
    
    // icon path
    Variable icon_path;
    qr = select(icon_path)
      .where(uriref, ll("svgIcon"), icon_path)
      .run(data);
    if (qr.size() > 0) {
      m_iconpath = qr[0][icon_path]->name;
      m_iconpath = m_iconpath.substr(8, m_iconpath.size() - 9);
      DBG2("Found SVG icon file "<<m_iconpath);
    }
    
    m_bundledir = m_bundle;
    
    // default preset path
    Variable preset_path;
    qr = select(preset_path)
      .where(uriref, ll("defaultPresets"), preset_path)
      .run(data);
    if (qr.size() > 0) {
      string presetfile = qr[0][preset_path]->name;
      DBG2("Found default preset file "<<presetfile);
      TurtleParser tp;
      RDFData data;
      if (!tp.parse_ttl_url(presetfile, data)) {
        DBG0("Could not parse presets from "<<presetfile);
      }
      else {
        Variable bank;
        qr = select(bank).where(uriref, ll("hasBank"), bank).run(data);
        if (qr.size() > 0) {
          string bankuri = qr[0][bank]->name;
          DBG2("Found preset bank "<<bankuri);
          Variable preset, name, program;
          vector<QueryResult> qr2 = select(preset, name, program)
            .where(uriref, ll("hasSetting"), preset)
            .where(preset, doap("name"), name)
            .where(preset, ll("midiProgram"), program)
            .where(preset, ll("inBank"), bankuri)
            .run(data);
          for (unsigned j = 0; j < qr2.size(); ++j) {
            string preseturi = qr2[j][preset]->name;
            DBG2("Found the preset \""<<qr2[j][name]->name<<"\" "
                 <<" with MIDI program number "<<qr2[j][program]->name);
            int pnum = atoi(qr2[j][program]->name.c_str());
            m_presets[pnum].name = qr2[j][name]->name;
            m_presets[pnum].values.clear();
            Variable pv, port, value;
            vector<QueryResult> qr3 = select(port, value)
              .where(preseturi, ll("hasPortValue"), pv)
              .where(pv, ll("forPort"), port)
              .where(pv, rdf("value"), value)
              .run(data);
            for (unsigned k = 0; k < qr3.size(); ++k) {
              int p = atoi(qr3[k][port]->name.c_str());
              float v = atof(qr3[k][value]->name.c_str());
              //cerr<<p<<": "<<v<<endl;
              m_presets[pnum].values[p] = v;
            }
          }
        }
      }
    }

  }
  
  // if we got this far the data is OK. time to load the library
  m_libhandle = dlopen(m_binary.substr(8, m_binary.size() - 9).c_str(), 
		       RTLD_NOW);
  if (!m_libhandle) {
    DBG0("Could not dlopen "<<m_binary<<": "<<dlerror());
    return false;
  }
  
  // get the descriptor
  LV2_Descriptor_Function dfunc = get_symbol<LV2_Descriptor_Function>("lv2_descriptor");
  if (!dfunc) {
    DBG0(m_binary<<" has no LV2 descriptor function");
    dlclose(m_libhandle);
    return false;
  }
  for (unsigned long j = 0; (m_desc = dfunc(j)); ++j) {
    if (m_uri == m_desc->URI)
      break;
  }
  if (!m_desc) {
    DBG0(m_binary<<" does not contain the plugin "<<m_uri);
    dlclose(m_libhandle);
    return false;
  }
  
  // get the command descriptor (if it has one)
  if (usesCommands) {
    if (m_desc->extension_data)
      m_comm_desc = (LV2_CommandDescriptor*)(m_desc->extension_data("http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension"));
    if (!m_comm_desc) {
      DBG0(m_uri<<" does not use the command extension like the RDF said it should");
      return false;
    }
  }
  
  // instantiate the plugin
  LV2_Feature command_feature = {
    "http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension",
    &m_comm_host_desc
  };
  const LV2_Feature* features[] = { &command_feature, 0 };
  m_handle = m_desc->instantiate(m_desc, m_rate, m_bundle.c_str(), features);
  
  if (!m_handle) {
    DBG0("Could not instantiate the plugin");
    dlclose(m_libhandle);
    m_libhandle = 0;
    m_desc = 0;
    return false;
  }
  
  return true;
}


bool LV2Host::print_uri(const string& bundle) {
  
  // parse
  TurtleParser tp;
  RDFData data;
  if (!tp.parse_ttl_file(bundle + "manifest.ttl", data)) {
    DBG1("Could not parse "<<bundle<<"manifest.ttl");
    return false;
  }
  
  // query
  Namespace lv2("<http://lv2plug.in/ns/lv2core#>");
  Variable uriref;
  vector<QueryResult> qr = select(uriref)
    .where(uriref, rdf("type"), lv2("Plugin"))
    .run(data);
  for (unsigned i = 0; i < qr.size(); ++i)
    cout<<qr[i][uriref]->name<<endl;

  return false;
}


void LV2Host::feedback(uint32_t argc, const char* const* argv) {
  cout<<"Plugin said:";
  for (unsigned i = 0; i < argc; ++i)
    cout<<" "<<"'"<<argv[i]<<"'";
  cout<<endl;
  signal_feedback(argc, argv);
}


void LV2Host::feedback_wrapper(void* me, uint32_t argc, 
			       const char* const* argv) {
  std::cerr<<__PRETTY_FUNCTION__<<endl;
  static_cast<LV2Host*>(me)->feedback(argc, argv);
}
