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
#include <lv2_event_helpers.h>

#include "beep.peg"


using namespace std;
using namespace Gtk;
using namespace sigc;


class BeepGUI : public LV2::GUI<BeepGUI, LV2::UriMapExt<true>, LV2::WriteMidi> {
public:
  
  BeepGUI(const std::string& URI) 
    : m_button("Click me!") {
    pack_start(m_button);
    m_button.signal_clicked().connect(mem_fun(*this, &BeepGUI::do_something));
  }
  
  ~BeepGUI() {

  }
  
  void do_something() {
    uint8_t midi_data[] = { 0x90, 0x20, 0x40 };
    write_midi(b_msg_input, 3, midi_data);
  }
  
protected:
  
  Button m_button;

};


static int _ = BeepGUI::register_class((string(b_uri) + "/gui").c_str());
