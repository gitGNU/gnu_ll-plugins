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
#include "sineshaper.peg"


using namespace std;
using namespace Gtk;
using namespace sigc;


class SineshaperGUI : public LV2::GUI<SineshaperGUI, LV2::Programs<false> > {
public:
  
  SineshaperGUI(const std::string& URI)
    : m_sshp(bundle_path(), host_supports_programs()) {
    
    pack_start(m_sshp);

    m_sshp.signal_control_changed.
      connect(mem_fun(*this, &SineshaperGUI::write_control));
    m_sshp.signal_preset_changed.
      connect(mem_fun(*this, &SineshaperGUI::change_program));
    /*m_sshp.signal_save_preset.
    connect(mem_fun(ctrl, &LV2::Controller::request_save));*/
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
  
  SineshaperWidget m_sshp;
  
};


static int _ = SineshaperGUI::register_class((string(s_uri) + "/gui").c_str());
