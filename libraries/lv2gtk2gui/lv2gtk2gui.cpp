/****************************************************************************
    
    lv2gtk2gui.cpp - Wrapper library to make it easier to write LV2 GUIs
                     in C++
    
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

#include <cstring>
#include <iostream>

#include "lv2gtk2gui.hpp"


using namespace std;


LV2Controller::LV2Controller()
  : m_wfunc(0),
    m_cfunc(0),
    m_ctrl(0),
    m_instdesc(0),
    m_progdesc(0),
    m_mididesc(0) {

}


void LV2Controller::write(uint32_t port, uint32_t buffer_size, 
			  const void* buffer) {
  if (m_wfunc)
    m_wfunc(m_ctrl, port, buffer_size, buffer);
}


void LV2Controller::command(uint32_t argc, const char* const* argv) {
  if (m_cfunc)
    m_cfunc(m_ctrl, argc, argv);
}


void LV2Controller::set_program(unsigned char number) {
  if (m_progdesc)
    m_progdesc->set_program(m_ctrl, number);
}


LV2Controller::LV2Controller(LV2UI_Write_Function wfunc, 
			     LV2UI_Command_Function cfunc,
                             LV2UI_Controller ctrl,
			     const LV2_Host_Feature** features)
  : m_wfunc(wfunc),
    m_cfunc(cfunc),
    m_ctrl(ctrl),
    m_progdesc(0) {
  
  for (int i = 0; features[i]; ++i) {
    if (!strcmp(features[i]->URI, "http://ll-plugins.nongnu.org/lv2/namespace#program")) {
      m_progdesc = static_cast<LV2_ProgramControllerDescriptor*>(features[i]->data);
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
  
  
  /* Tell the GUI that a control rate float port has changed. You should
     not use this directly. */
  void port_event(LV2UI_Handle instance, uint32_t port, uint32_t buffer_size,
		  const void* buffer) {
    static_cast<LV2GTK2GUI*>(instance)->port_event(port, buffer_size, buffer);
  }
  

  void feedback(LV2UI_Handle instance, uint32_t argc, const char* const* argv) {
    static_cast<LV2GTK2GUI*>(instance)->feedback(argc, argv);
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
    if (!strcmp(URI, "http://ll-plugins.nongnu.org/lv2/namespace#program"))
      return &LV2GTK2GUI::m_program_ui_desc;
    return static_cast<LV2GTK2GUI*>(instance)->extension_data(URI);
  }
  
  
}  


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


