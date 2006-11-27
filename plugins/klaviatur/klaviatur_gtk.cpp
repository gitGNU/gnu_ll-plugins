/****************************************************************************
    
    euphoria_gtk.cpp - GUI for the Euphoria LV2 synth
    
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

#include <iostream>
#include <string>

#include <gtkmm.h>

#include "lv2gtk2gui.hpp"
#include "klaviatur.peg"
#include "keyboard.hpp"


using namespace sigc;
using namespace Gtk;


class KlaviaturGUI : public LV2GTK2GUI {
public:
  
  KlaviaturGUI(LV2Controller& ctrl, const std::string& URI, 
               const std::string& bundle_path, Widget*& widget) {

    m_kb.set_flags(m_kb.get_flags() | CAN_FOCUS);
    m_kb.signal_key_on().
      connect(bind(mem_fun(*this, &KlaviaturGUI::handle_keypress), ref(ctrl)));
    m_kb.signal_key_off().
      connect(bind(mem_fun(*this, &KlaviaturGUI::handle_keyrelease),ref(ctrl)));
    widget = &m_kb;
  }
  
protected:

  void handle_keypress(unsigned char key, LV2Controller& ctrl) {
    unsigned char data[3] = { 0x90, key + 36, 64 };
    ctrl.send_midi(k_midi_input, 3, data);
  }
  
  
  void handle_keyrelease(unsigned char key, LV2Controller& ctrl) {
    unsigned char data[3] = { 0x80, key + 36, 64 };
    ctrl.send_midi(k_midi_input, 3, data);
  }

  Keyboard m_kb;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2gtk2gui<KlaviaturGUI>("http://ll-plugins.nongnu.org/lv2/dev/klaviatur/0.0.0");
}
