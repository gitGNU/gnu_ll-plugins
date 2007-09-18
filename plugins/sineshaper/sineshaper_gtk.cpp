/****************************************************************************
    
    sineshaper_gtk.cpp - A GUI for the Sineshaper LV2 synth
    
    Copyright (C) 2006-2007  Lars Luthman <lars.luthman@gmail.com>
    
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
#include "sineshaperwidget.hpp"


using namespace std;
using namespace Gtk;
using namespace sigc;


class SineshaperGUI : public LV2GTK2GUI {
public:
  
  SineshaperGUI(LV2Controller& ctrl, const std::string& URI, 
              const std::string& bundle_path)
    : m_sshp(bundle_path),
      m_ctrl(ctrl) {
    
    pack_start(m_sshp);

    m_sshp.signal_control_changed.
      connect(mem_fun(*this, &SineshaperGUI::request_control_change));
    //m_sshp.signal_program_selected.
    //  connect(mem_fun(ctrl, &LV2Controller::set_program));
  }
  
  void set_control(uint32_t port, float value) {
    m_sshp.set_control(port, value);
  }

  void add_program(unsigned char number, const char* name) {
    //m_sshp.add_program(number, name);
  }
  
  void remove_program(unsigned char number) {
    //m_sshp.remove_program(number);
  }
  
  void clear_programs() {
    //m_sshp.clear_programs();
  }
  
  void set_program(unsigned char number) {
    //m_sshp.set_program(number);
  }
  
protected:
  
  void request_control_change(uint32_t port, float value) {
    m_ctrl.write(port, sizeof(float), &value);
  }
  
  SineshaperWidget m_sshp;
  LV2Controller& m_ctrl;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2gtk2gui<SineshaperGUI>("http://ll-plugins.nongnu.org/lv2/dev/sineshaper/0.0.0/gui");
}
