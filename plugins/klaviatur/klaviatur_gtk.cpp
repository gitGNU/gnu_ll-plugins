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

#include "lv2uiclient.hpp"
#include "klaviatur.peg"
#include "keyboard.hpp"


using namespace std;
using namespace Gtk;
using namespace Glib;
using namespace sigc;


void handle_keypress(unsigned char key, LV2UIClient& lv2) {
  //cerr<<"key on: "<<int(key)<<endl;
  unsigned char data[3] = { 0x90, key + 36, 64 };
  lv2.send_midi(k_midi_input, 3, data);
}


void handle_keyrelease(unsigned char key, LV2UIClient& lv2) {
  //cerr<<"key off: "<<int(key)<<endl;
  unsigned char data[3] = { 0x80, key + 36, 64 };
  lv2.send_midi(k_midi_input, 3, data);
}


int main(int argc, char** argv) {
  
  // create the objects
  LV2UIClient lv2(argc, argv, true);
  if (!lv2.is_valid()) {
    cerr<<"Could not start OSC listener. You are not running the "<<endl
        <<"program manually, are you? It's supposed to be started by "<<endl
        <<"the plugin host."<<endl;
    return 1;
  }
  
  Main kit(argc, argv);
  Window main_win;
  Keyboard kb;
  kb.set_flags(kb.get_flags() | CAN_FOCUS);
  main_win.add(kb);
  
  main_win.set_title(string("Klaviatur: ") + lv2.get_identifier());
  
  lv2.show_received.connect(mem_fun(main_win, &Window::show_all));
  lv2.hide_received.connect(mem_fun(main_win, &Window::hide));
  lv2.quit_received.connect(&Main::quit);
  kb.signal_key_on().connect(bind(&handle_keypress, ref(lv2)));
  kb.signal_key_off().connect(bind(&handle_keyrelease, ref(lv2)));
  main_win.signal_delete_event().connect(bind_return(hide(&Main::quit), true));
  
  // start
  lv2.send_update_request();
  Main::run();

  return 0;
}


