/****************************************************************************
    
    euphoriaguiplugin.cpp - A GUI for the Euphoria LV2 synth
    
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

#include <iostream>

#include <gtkmm.h>

#include "lv2gtk2gui.hpp"
#include "euphoriawidget.hpp"
#include "euphoria.peg"


using namespace std;
using namespace Gtk;
using namespace sigc;


class EuphoriaGUI : public LV2GTK2GUI {
public:
  
  EuphoriaGUI(LV2Controller& ctrl, const std::string& URI, 
              const std::string& bundle_path)
    : m_ctrl(ctrl) {
    
    pack_start(m_euph);

    m_euph.signal_control_changed.
      connect(mem_fun(*this, &EuphoriaGUI::control_changed));
    m_euph.signal_configure.
      connect(mem_fun(*this, &EuphoriaGUI::configure_changed));
    m_euph.signal_program_selected.
      connect(mem_fun(ctrl, &LV2Controller::request_program));
  }
  
  void port_event(uint32_t port, uint32_t buffer_size, const void* buffer) {
    m_euph.set_control(port, *static_cast<const float*>(buffer));
  }

  void feedback(uint32_t argc, const char* const* argv) {
    if (argc == 2)
      m_euph.configure(argv[0], argv[1]);
  }
  
  void add_program(unsigned char number, const char* name) {
    m_euph.add_program(number, name);
  }
  
  void remove_program(unsigned char number) {
    m_euph.remove_program(number);
  }
  
  void clear_programs() {
    m_euph.clear_programs();
  }
  
  void set_program(unsigned char number) {
    m_euph.set_program(number);
  }
  
protected:
  
  void control_changed(uint32_t port, float value) {
    m_ctrl.write(port, sizeof(float), &value);
  }
  
  void configure_changed(const string& key, const string& value) {
    const char* array[] = { key.c_str(), value.c_str() };
    m_ctrl.command(2, array);
  }
  
  EuphoriaWidget m_euph;
  LV2Controller& m_ctrl;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2gtk2gui<EuphoriaGUI>(string(e_uri) + "/gui");
}
