/****************************************************************************
    
    lv2guihost.cpp - Simple LV2 GUI plugin loader for Elven
    
    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>
    
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

#include <iostream>

#include <dlfcn.h>

#include "debug.hpp"
#include "lv2guihost.hpp"


using namespace std;


LV2GUIHost::LV2GUIHost(const std::string& gui_path, 
		       const std::string& gui_uri,
		       const std::string& plugin_uri, 
		       const std::string& bundle_path)
  : m_desc(0),
    m_ui(0),
    m_cwidget(0),
    m_widget(0),
    m_block_gui(false) {
  
  // open the module
  DBG2("Loading "<<gui_path);
  void* module = dlopen(gui_path.c_str(), RTLD_LAZY);
  if (!module) {
    DBG0("Could not load "<<gui_path<<": "<<dlerror());
    return;
  }
  
  // get the GUI descriptor
  LV2UI_DescriptorFunction func = 
    nasty_cast<LV2UI_DescriptorFunction>(dlsym(module, "lv2ui_descriptor"));
  if (!func) {
    DBG0("Could not find symbol lv2ui_descriptor in "<<gui_path);
    return;
  }
  for (unsigned i = 0; (m_desc = func(i)); ++i) {
    DBG2("Found GUI "<<m_desc->URI);
    if (!strcmp(gui_uri.c_str(), m_desc->URI))
      break;
    DBG2(m_desc->URI<<" does not match "<<gui_uri);
  }
  if (!m_desc) {
    DBG0(gui_path<<" did not contain the plugin GUI "<<gui_uri);
    return;
  }
  
  // build the feature list
  LV2_Feature** features = new LV2_Feature*[1];
  features[0] = 0;
  
  // create a GUI instance
  LV2UI_Controller ctrl = static_cast<LV2UI_Controller>(this);
  m_block_gui = true;
  m_ui = m_desc->instantiate(m_desc, plugin_uri.c_str(), bundle_path.c_str(),
			     &LV2GUIHost::_write_port, ctrl, 
			     reinterpret_cast<LV2UI_Widget*>(&m_cwidget),
			     const_cast<const LV2_Feature**>(features));
  m_block_gui = false;
  if (!m_ui || !m_cwidget) {
    DBG0("Could not create an UI instance");
    m_desc = 0;
    m_widget = 0;
    m_ui = 0;
    return;
  }
  
  m_widget = Glib::wrap(m_cwidget);
}

  
LV2GUIHost::~LV2GUIHost() {
  if (m_ui)
    m_desc->cleanup(m_ui);
}

  
bool LV2GUIHost::is_valid() const {
  return (m_ui != 0);
}
  

Gtk::Widget& LV2GUIHost::get_widget() {
  return *m_widget;
}
  

void LV2GUIHost::port_event(uint32_t index, uint32_t buffer_size, 
			    const void* buffer) {
  if (m_ui && m_desc && m_desc->port_event) {
    m_block_gui = true;
    m_desc->port_event(m_ui, index, buffer_size, buffer);
    m_block_gui = false;
  }
}
 
 
void LV2GUIHost::feedback(uint32_t argc, const char* const* argv) {
  /*if (m_ui && m_desc && m_desc->feedback) {
    m_block_gui = true;
    // XXX m_desc->feedback(m_ui, argc, argv);
    m_block_gui = false;
    }*/
}


void LV2GUIHost::program_added(unsigned char number, const char* name) {
  /*if (m_ui && m_desc && m_desc->program_added) {
    m_block_gui = true;
    // XXX m_desc->program_added(m_ui, number, name);
    m_block_gui = false;
    }*/
}

  
void LV2GUIHost::program_removed(unsigned char number) {
  /*if (m_ui && m_desc && m_desc->program_removed) {
    m_block_gui = true;
    // XXX m_desc->program_removed(m_ui, number);
    m_block_gui = false;
    }*/
}
 
 
void LV2GUIHost::programs_cleared() {
  /*if (m_ui && m_desc && m_desc->programs_cleared) {
    m_block_gui = true;
    // XXX m_desc->programs_cleared(m_ui);
    m_block_gui = false;
    }*/
}

  
void LV2GUIHost::current_program_changed(unsigned char number) {
  /*if (m_ui && m_desc && m_desc->current_program_changed) {
    m_block_gui = true;
    // XXX m_desc->current_program_changed(m_ui, number);
    m_block_gui = false;
    }*/
}


void LV2GUIHost::_write_port(LV2UI_Controller ctrl, uint32_t index, 
			     uint32_t buffer_size, const void* buffer) {
  LV2GUIHost* me = static_cast<LV2GUIHost*>(ctrl);
  if (me->m_block_gui)
    DBG1("GUI requested write to input port while a GUI callback was running");
  else {
    DBG2("GUI requested write to input port "<<index);
    me->write_port(index, buffer_size, buffer);
  }
}
  

void LV2GUIHost::_command(LV2UI_Controller ctrl, 
			  uint32_t argc, const char* const* argv) {

  cerr<<__PRETTY_FUNCTION__<<endl;
  cerr<<"POINTERS:"<<endl;
  for (unsigned i = 0; i < argc; ++i)
    cerr<<"  "<<(const void*)(argv[i])<<endl;

  LV2GUIHost* me = static_cast<LV2GUIHost*>(ctrl);
  if (me->m_block_gui)
    DBG1("GUI tried to send a command while a GUI callback was running");
  else {
    DBG2("GUI sent command:");
    for (unsigned i = 0; i < argc; ++i)
      DBG2("  '"<<argv[i]<<"'");
    me->command(argc, argv);
  }
}

  
void LV2GUIHost::_request_program(LV2UI_Controller ctrl, unsigned char number) {
  LV2GUIHost* me = static_cast<LV2GUIHost*>(ctrl);
  if (me->m_block_gui)
    DBG1("GUI requested program change while a GUI callback was running");
  else {
    DBG2("GUI requested program change to "<<int(number));
    me->request_program(number);
  }
}


void LV2GUIHost::_save_program(LV2UI_Controller ctrl, unsigned char number,
			       const char* name) {
  LV2GUIHost* me = static_cast<LV2GUIHost*>(ctrl);
  if (me->m_block_gui)
    DBG1("GUI requested program save while a GUI callback was running");
  else {
    DBG2("GUI requested program save to "<<int(number)<<", \""<<name<<"\"");
    me->save_program(number, name);
  }
}
