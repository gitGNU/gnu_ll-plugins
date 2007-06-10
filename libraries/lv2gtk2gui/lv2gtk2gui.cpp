/****************************************************************************
    
    lv2gtk2gui.cpp - Wrapper library to make it easier to write LV2 GUIs
                     in C++
    
    Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
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

#include <cstring>
#include <iostream>

#include "lv2gtk2gui.hpp"


using namespace std;


LV2Controller::LV2Controller()
  : m_cfunc(0),
    m_ctrl(0),
    m_instdesc(0),
    m_progdesc(0),
    m_mididesc(0) {

}


void LV2Controller::set_control(uint32_t port, float value) {
  if (m_cfunc)
    m_cfunc(m_ctrl, port, value);
}


void LV2Controller::configure(const string& key, const string& value) {
  if (m_instdesc)
    m_instdesc->configure(m_ctrl, key.c_str(), value.c_str());
}


void LV2Controller::set_file(const string& key, const string& filename) {
  if (m_instdesc)
    m_instdesc->set_file(m_ctrl, key.c_str(), filename.c_str());
}
  

void LV2Controller::set_program(unsigned char number) {
  if (m_progdesc)
    m_progdesc->set_program(m_ctrl, number);
}


void LV2Controller::send_midi(uint32_t port, uint32_t size, 
                              const unsigned char* data) {
  if (m_mididesc)
    m_mididesc->send_midi(m_ctrl, port, size, data);
}


void LV2Controller::tell_plugin(uint32_t argc, const char* const* argv) {
  cerr<<__PRETTY_FUNCTION__<<endl;
  if (m_commdesc)
    m_commdesc->tell_plugin(m_ctrl, argc, argv);
}


LV2Controller::LV2Controller(LV2UI_Set_Control_Function cfunc, 
                             LV2UI_Controller ctrl,
			     const LV2_Host_Feature** features)
  : m_cfunc(cfunc),
    m_ctrl(ctrl),
    m_instdesc(0),
    m_progdesc(0),
    m_mididesc(0),
    m_commdesc(0) {
  
  for (int i = 0; features[i]; ++i) {
    if (!strcmp(features[i]->URI, "http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext")) {
      m_instdesc = static_cast<LV2_InstrumentControllerDescriptor*>(features[i]->data);
    }
    else if (!strcmp(features[i]->URI, "http://ll-plugins.nongnu.org/lv2/namespace#program")) {
      m_progdesc = static_cast<LV2_ProgramControllerDescriptor*>(features[i]->data);
    }
    else if (!strcmp(features[i]->URI, "http://ll-plugins.nongnu.org/lv2/ext/miditype")) {
      m_mididesc = static_cast<LV2_MIDIControllerDescriptor*>(features[i]->data);
    }
    else if (!strcmp(features[i]->URI, "http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension")) {
      m_commdesc = static_cast<LV2_GUICommControllerDescriptor*>(features[i]->data);
    }
  }
}


namespace LV2G2GSupportFunctions {
  

  DescList& get_lv2g2g_descriptors() {
    static DescList list;
    return list;
  }


  void delete_ui_instance(LV2UI_Handle instance) {
    delete static_cast<LV2GTK2GUI*>(instance);
  }
  
  
  void set_control(LV2UI_Handle instance, uint32_t port, float value) {
    static_cast<LV2GTK2GUI*>(instance)->set_control(port, value);
  }
  
  
  void configure(LV2UI_Handle instance, const char* key, const char* value) {
    static_cast<LV2GTK2GUI*>(instance)->configure(key, value);
  }

  
  void set_file(LV2UI_Handle instance, const char* key, const char* filename) {
    static_cast<LV2GTK2GUI*>(instance)->set_file(key, filename);
  }
  
  
  void add_program(LV2UI_Handle instance, unsigned char number, 
                   const char* name) {
    static_cast<LV2GTK2GUI*>(instance)->add_program(number, name);
  }

  
  void remove_program(LV2UI_Handle instance, unsigned char number) {
    static_cast<LV2GTK2GUI*>(instance)->remove_program(number);
  }
  

  void clear_programs(LV2UI_Handle instance) {
    static_cast<LV2GTK2GUI*>(instance)->clear_programs();
  }

  
  void set_program(LV2UI_Handle instance, unsigned char number) {
    static_cast<LV2GTK2GUI*>(instance)->set_program(number);
  }
  

  void* extension_data(LV2UI_Handle instance, const char* URI) {
    if (!strcmp(URI, 
                "http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext"))
      return &LV2GTK2GUI::m_instrument_ui_desc;
    else if (!strcmp(URI, 
                     "http://ll-plugins.nongnu.org/lv2/namespace#program"))
      return &LV2GTK2GUI::m_program_ui_desc;

    
    return static_cast<LV2GTK2GUI*>(instance)->extension_data(URI);
  }
  
  
}  


LV2_InstrumentUIDescriptor LV2GTK2GUI::m_instrument_ui_desc = {
  &LV2G2GSupportFunctions::configure,
  &LV2G2GSupportFunctions::set_file
};


LV2_ProgramUIDescriptor LV2GTK2GUI::m_program_ui_desc = {
  &LV2G2GSupportFunctions::add_program,
  &LV2G2GSupportFunctions::remove_program,
  &LV2G2GSupportFunctions::clear_programs,
  &LV2G2GSupportFunctions::set_program
};


extern "C" {

  const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index) {
    using namespace LV2G2GSupportFunctions;
    DescList descs = get_lv2g2g_descriptors();
    if (index >= descs.size())
      return 0;
    return descs[index];
  }

}


