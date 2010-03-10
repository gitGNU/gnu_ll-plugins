/****************************************************************************
    
    beep_gtk.cpp - A GUI for the Beep LV2 synth
    
    Copyright (C) 2006-2008  Lars Luthman <lars.luthman@gmail.com>
    
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
#include <lv2gui.hpp>
// XXX - Really shouldn't rely on internal headers of lv2-c++-tools
#include <lv2cxx_common/lv2_event_helpers.h>

#include "beep.peg"


using namespace std;
using namespace Gtk;
using namespace sigc;
using namespace LV2;

class BeepGUI : public GUI<BeepGUI, 
			   URIMap<true>, WriteMIDI<true>, WriteOSC<true> > {
public:
  
  BeepGUI(const std::string& URI) 
    : m_osc_button("Click me!"),
      m_midi_button("Or me!") {
    pack_start(m_osc_button);
    pack_start(m_midi_button);
    m_osc_button.signal_clicked().
      connect(mem_fun(*this, &BeepGUI::do_stuff));
    m_midi_button.signal_clicked().
      connect(mem_fun(*this, &BeepGUI::do_other_stuff));
  }
  
  ~BeepGUI() {

  }
  
  void do_stuff() {
    write_osc(b_msg_input, "/", "ifs", 42, 3.14159, "hello world");
  }
  
  void do_other_stuff() {
    uint8_t data[] = { 0x90, 0x40, 0x60 };
    write_midi(b_msg_input, 3, data);
  }
  
protected:
  
  Button m_osc_button;
  Button m_midi_button;

};


static int _ = BeepGUI::register_class((string(b_uri) + "/gui").c_str());
