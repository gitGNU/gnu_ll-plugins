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

#include "lv2gui.hpp"
#include "sineshaperwidget.hpp"


using namespace std;
using namespace Gtk;
using namespace sigc;


class SineshaperGUI : public LV2::GUI {
public:
  
  SineshaperGUI(LV2::Controller& ctrl, const std::string& URI, 
              const std::string& bundle_path)
    : m_sshp(bundle_path),
      m_ctrl(ctrl) {
    
    pack_start(m_sshp);

    m_sshp.signal_control_changed.
      connect(mem_fun(*this, &SineshaperGUI::request_control_change));
    m_sshp.signal_preset_changed.
      connect(mem_fun(ctrl, &LV2::Controller::request_program));
    m_sshp.signal_save_preset.
      connect(mem_fun(ctrl, &LV2::Controller::request_save));
  }
  
  void port_event(uint32_t port, uint32_t buffer_size, const void* buffer) {
    //cerr<<__PRETTY_FUNCTION__<<" "<<port<<", "<<value<<endl;
    m_sshp.set_control(port, *static_cast<const float*>(buffer));
  }

  void program_added(unsigned char number, const char* name) {
    m_sshp.add_preset(number, name);
  }
  
  void program_removed(unsigned char number) {
    m_sshp.remove_preset(number);
  }
  
  void programs_cleared() {
    m_sshp.clear_presets();
  }
  
  void current_program_changed(unsigned char number) {
    m_sshp.set_preset(number);
  }
  
protected:
  
  void request_control_change(uint32_t port, float value) {
    m_ctrl.write(port, sizeof(float), &value);
  }
  
  SineshaperWidget m_sshp;
  LV2::Controller& m_ctrl;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  LV2::register_lv2gtk2gui<SineshaperGUI>("http://ll-plugins.nongnu.org/lv2/dev/sineshaper/0.0.0/gui");
}
