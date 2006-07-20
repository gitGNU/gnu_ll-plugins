#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dlfcn.h>

#include <turtleparser.hpp>
#include <query.hpp>
#include <namespaces.hpp>

#include "lv2host.hpp"


using namespace std;
using namespace PAQ;


namespace {
  
  string absolutise(const string& reluri, const string& baseuri) {
    string result = baseuri + "/" + reluri.substr(1, reluri.length() - 2);
    return result;
  }
  
}



LV2Host::LV2Host(const string& uri, unsigned long frame_rate) 
  : m_handle(0),
    m_inst_desc(0),
    m_midimap(128, -1),
    m_program_is_valid(false),
    m_new_program(false) {
  
  pthread_mutex_init(&m_mutex, 0);
  
  vector<string> search_dirs;
  search_dirs.push_back(string(getenv("HOME")) + "/.lv2");
  search_dirs.push_back("/usr/lib/lv2");
  search_dirs.push_back("/usr/local/lib/lv2");
  
  string uriref = string("<") + uri + ">";
  string library;
  string ttlfile;
  string plugindir;
  
  for (unsigned i = 0; i < search_dirs.size(); ++i) {
    DIR* d = opendir(search_dirs[i].c_str());
    if (d == 0) {
      cerr<<"Could not open "<<search_dirs[i]<<": "<<strerror(errno)<<endl;
      continue;
    }
    dirent* e;
    while (e = readdir(d)) {
      if (strlen(e->d_name) >= 4) {
        
        // is it an LV2 bundle?
        if (strcmp(e->d_name + (strlen(e->d_name) - 4), ".lv2"))
          continue;
        
        // is it actually a directory?
        plugindir = search_dirs[i] + "/" + e->d_name;
        struct stat stat_info;
        if (stat(plugindir.c_str(), &stat_info))
          continue;
        if (!S_ISDIR(stat_info.st_mode))
          continue;
        
        // parse the manifest to get the library filename and the data filename
        ttlfile = plugindir + "/manifest.ttl";
        {
          
          ifstream ifs((plugindir + "/manifest.ttl").c_str());
          string text, line;
          while (getline(ifs, line))
            text += line + "\n";
          TurtleParser tp;
          RDFData data;
          if (!tp.parse_ttl(text, data)) {
            cerr<<"Could not parse "<<plugindir<<"/manifest.ttl"<<endl;
            continue;
          }
          Variable binary, datafile;
          Namespace lv2("<http://lv2plug.in/ontology#>");
          Query q1 = select(binary)
            .where(uriref, lv2("binary"), binary);
          vector<QueryResult> qr = q1.run(data);
          if (qr.size() == 0)
            continue;
          else
            library = absolutise(qr[0][binary]->name, plugindir);
          Query q2 = select(datafile)
            .where(uriref, rdfs("seeAlso"), datafile);
          qr = q2.run(data);
          if (qr.size() > 0)
            ttlfile = absolutise(qr[0][datafile]->name, plugindir);
          break;
        }
      }
    }
    closedir(d);
    if (library != "")
      break;
  }
  
  if (library == "") {
    cerr<<"Could not find plugin "<<uri<<endl;
    return;
  }
  
  // parse the datafile to get port info
  {
    
    ifstream ifs(ttlfile.c_str());
    string text, line;
    while (getline(ifs, line))
      text += line + "\n";
    TurtleParser tp;
    RDFData data;
    if (!tp.parse_ttl(text, data)) {
      cerr<<"Could not parse "<<ttlfile<<endl;
      return;
    }
    Variable symbol, index, portclass, porttype, port;
    Namespace lv2("<http://lv2plug.in/ontology#>");
    Namespace ll("<http://ll-plugins.nongnu.org/lv2/namespace#>");
    
    Query q1 = select(index, symbol, portclass, porttype)
      .where(uriref, lv2("port"), port)
      .where(port, rdf("type"), portclass)
      .where(port, lv2("index"), index)
      .where(port, lv2("symbol"), symbol)
      .where(port, lv2("datatype"), porttype);
    vector<QueryResult> qr = q1.run(data);
    m_ports.resize(qr.size());
    for (unsigned j = 0; j < qr.size(); ++j) {
      
      // index
      size_t p = atoi(qr[j][index]->name.c_str());
      if (p >= m_ports.size())
        return;
      
      // symbol
      m_ports[p].symbol = qr[j][symbol]->name;
      
      // direction and rate
      string pclass = qr[j][portclass]->name;
      if (pclass == lv2("ControlRateInputPort")) {
        m_ports[p].direction = InputPort;
        m_ports[p].rate = ControlRate;
      }
      else if (pclass == lv2("ControlRateOutputPort")) {
        m_ports[p].direction = OutputPort;
        m_ports[p].rate = ControlRate;
      }
      else if (pclass == lv2("AudioRateInputPort")) {
        m_ports[p].direction = InputPort;
        m_ports[p].rate = AudioRate;
      }
      else if (pclass == lv2("AudioRateOutputPort")) {
        m_ports[p].direction = OutputPort;
        m_ports[p].rate = AudioRate;
      }
      
      // type
      string type = qr[j][porttype]->name;
      if (type == lv2("float"))
        m_ports[p].midi = false;
      else if (type == ll("miditype"))
        m_ports[p].midi = true;
      else
        return;
      
      m_ports[p].default_value = 0;
      m_ports[p].min_value = 0;
      m_ports[p].max_value = 1;
    }
    
    // default values
    Variable default_value;
    Query q3 = select(index, default_value)
      .where(uriref, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("default"), default_value);
    qr = q3.run(data);
    for (unsigned j = 0; j < qr.size(); ++j) {
      unsigned p = atoi(qr[j][index]->name.c_str());
      if (p >= m_ports.size())
        return;
      m_ports[p].default_value = 
        atof(qr[j][default_value]->name.c_str());
      m_ports[p].locked_value = m_ports[p].default_value;
    }
    
    // minimum values
    Variable min_value;
    q3 = select(index, min_value)
      .where(uriref, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("minimum"), min_value);
    qr = q3.run(data);
    for (unsigned j = 0; j < qr.size(); ++j) {
      unsigned p = atoi(qr[j][index]->name.c_str());
      if (p >= m_ports.size())
        return;
      m_ports[p].min_value = 
        atof(qr[j][min_value]->name.c_str());
    }
    
    // maximum values
    Variable max_value;
    q3 = select(index, max_value)
      .where(uriref, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("maximum"), max_value);
    qr = q3.run(data);
    for (unsigned j = 0; j < qr.size(); ++j) {
      unsigned p = atoi(qr[j][index]->name.c_str());
      if (p >= m_ports.size())
        return;
      m_ports[p].max_value = 
        atof(qr[j][max_value]->name.c_str());
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
      cerr<<"range for port "<<i<<": "
          <<m_ports[i].min_value<<" - "<<m_ports[i].max_value<<endl;
    }
    
    // MIDI controller bindings
    Variable controller, cc_number;
    Query q4 = select(index, cc_number)
      .where(uriref, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, ll("defaultMidiController"), controller)
      .where(controller, ll("controllerType"), ll("CC"))
      .where(controller, ll("controllerNumber"), cc_number);
    qr = q4.run(data);
    for (unsigned j = 0; j < qr.size(); ++j) {
      unsigned p = atoi(qr[j][index]->name.c_str());
      unsigned cc = atoi(qr[j][cc_number]->name.c_str());
      if (p < m_ports.size() && cc < 128)
        m_midimap[cc] = p;
    }
    
    // default MIDI port
    Query q5 = select(index)
      .where(uriref, ll("defaultMidiPort"), index);
    qr = q5.run(data);
    if (qr.size() > 0)
      m_default_midi_port = atoi(qr[0][index]->name.c_str());
    else {
      m_default_midi_port = -1;
      for (unsigned long i = 0; i < m_ports.size(); ++i) {
        if (m_ports[i].midi && m_ports[i].direction == InputPort && 
            m_ports[i].rate == ControlRate) {
          m_default_midi_port = i;
          break;
        }
      }
    }
    
    // standalone GUI path
    Variable gui_path;
    Query q0 = select(gui_path)
      .where(uriref, ll("standaloneGui"), gui_path);
    qr = q0.run(data);
    if (qr.size() > 0)
      m_standalonegui = absolutise(qr[0][gui_path]->name, plugindir);
    
    m_bundledir = plugindir;
  }
  
  // if we got this far the data is OK. time to get the descriptor
  m_libhandle = dlopen(library.c_str(), RTLD_NOW);
  if (!m_libhandle) {
    cerr<<"Could not dlopen "<<library<<": "<<dlerror()<<endl;
    return;
  }
  union {
    LV2_Descriptor_Function f;
    void* v;
  } lv2_desc_func;
  lv2_desc_func.v = dlsym(m_libhandle, "lv2_descriptor");
  if (!lv2_desc_func.v) {
    cerr<<library<<" has no LV2 descriptor function"<<endl;
    dlclose(m_libhandle);
    return;
  }
  for (unsigned long j = 0; m_desc = lv2_desc_func.f(j); ++j) {
    if (uri == m_desc->URI)
      break;
  }
  if (!m_desc) {
    cerr<<library<<" does not contain the plugin "<<uri<<endl;
    dlclose(m_libhandle);
    return;
  }
  
  // instantiate the plugin
  LV2_Host_Feature instrument_feature;
  instrument_feature.URI = "<http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext>";
  union {
    LV2_InstrumentFunctionCallback lifc;
    void* v;
  } u;
  u.lifc = &LV2Host::instrument_descriptor_callback;
  instrument_feature.data = u.v;
  const LV2_Host_Feature* features[2];
  features[0] = &instrument_feature;
  features[1] = 0;
  
  // nasty workaround for C callback
  m_current_object = this;
  m_handle = m_desc->instantiate(m_desc, frame_rate, 
                                 plugindir.c_str(), features);
  m_current_object = 0;
  
  // list available programs
  bool default_program_set = false;
  unsigned long default_program;
  const LV2_ProgramDescriptor* pdesc;
  for (unsigned long index = 0; (pdesc = get_program(index)); ++index) {
    cerr<<"Program "<<pdesc->number<<": "<<pdesc->name<<endl;
    if (!default_program_set) {
      queue_program(pdesc->number);
      default_program_set = true;
    }
  }
  
  
  if (!m_handle) {
    cerr<<"Could not instantiate plugin "<<uri<<endl;
    dlclose(m_libhandle);
    return;
  }
}
  
  
LV2Host::~LV2Host() {
  if (m_handle) {
    m_desc->cleanup(m_handle);
    dlclose(m_libhandle);
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
  m_desc->activate(m_handle);
}


void LV2Host::run(unsigned long nframes) {
  assert(m_handle);
  for (size_t i = 0; i < m_ports.size(); ++i)
    m_desc->connect_port(m_handle, i, m_ports[i].buffer);
  
  // try to execute queued commands
  if (!pthread_mutex_trylock(&m_mutex)) {
    
    // copy new control values to the real port buffers
    for (size_t i = 0; i < m_ports.size(); ++i) {
      if (m_ports[i].rate == ControlRate && !m_ports[i].midi && 
          m_ports[i].direction == InputPort)
        *static_cast<float*>(m_ports[i].buffer) = m_ports[i].locked_value;
    }
    
    // if there is a new program queued, switch to it
    if (m_new_program) {
      select_program(m_program);
      for (size_t i = 0; i < m_ports.size(); ++i) {
        if (m_ports[i].rate == ControlRate && !m_ports[i].midi && 
            m_ports[i].direction == InputPort)
          m_ports[i].locked_value = *static_cast<float*>(m_ports[i].buffer);
      }
      m_program_is_valid = true;
      m_new_program = false;
    }
    
    pthread_mutex_unlock(&m_mutex);
  }
  
  m_desc->run(m_handle, nframes);
}


void LV2Host::deactivate() {
  assert(m_handle);
  m_desc->deactivate(m_handle);
}


char* LV2Host::configure(const char* key, const char* value) {
  if (m_inst_desc && m_inst_desc->configure) {
    char* result = m_inst_desc->configure(m_handle, key, value);
    if (!result) {
      m_configuration[key] = value;
      m_program_is_valid;
    }
    return result;
  }
  else
    cerr<<"This plugin has no configure() callback"<<endl;
  return 0;
}


unsigned int LV2Host::list_used_files(char*** keys, char*** filepaths) {
  if (m_inst_desc && m_inst_desc->list_used_files)
    return m_inst_desc->list_used_files(m_handle, keys, filepaths);
  else
    cerr<<"This plugin has no list_used_files() callback"<<endl;
  return 0;
}


const LV2_ProgramDescriptor* LV2Host::get_program(unsigned long index) {
  if (m_inst_desc && m_inst_desc->get_program)
    return m_inst_desc->get_program(m_handle, index);
  else
    cerr<<"This plugin has no get_program() callback"<<endl;
  return 0;
}


void LV2Host::select_program(unsigned long program) {
  if (m_inst_desc && m_inst_desc->select_program)
    m_inst_desc->select_program(m_handle, program);
}


void LV2Host::instrument_descriptor_callback(LV2_InstrumentFunction func){
  m_current_object->m_inst_desc = func();
}


const std::vector<int>& LV2Host::get_midi_map() const {
  return m_midimap;
}


const std::string& LV2Host::get_gui_path() const {
  return m_standalonegui;
}


const std::string& LV2Host::get_bundle_dir() const {
  return m_bundledir;
}


void LV2Host::queue_program(unsigned long program) {
  pthread_mutex_lock(&m_mutex);
  m_program = program;
  m_program_is_valid = true;
  m_new_program = true;
  pthread_mutex_unlock(&m_mutex);
}


void LV2Host::queue_control(unsigned long port, float value) {
  if (port < m_ports.size() && m_ports[port].direction == InputPort &&
      m_ports[port].rate == ControlRate && !m_ports[port].midi) {
    pthread_mutex_lock(&m_mutex);
    m_ports[port].locked_value = value;
    pthread_mutex_unlock(&m_mutex);
  }
}


void LV2Host::queue_midi(unsigned long port, const unsigned char* midi) {
  cerr<<__PRETTY_FUNCTION__<<" is not implemented yet!"<<endl;
}


const std::map<std::string, std::string>& LV2Host::get_configuration() const {
  return m_configuration;
}


bool LV2Host::program_is_valid() const {
  // XXX not threadsafe
  return m_program_is_valid;
}


unsigned long LV2Host::get_program() const {
  // XXX not threadsafe
  return m_program;
}


LV2Host* LV2Host::m_current_object(0);