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
#include "euphoriawidget.hpp"


using namespace std;
using namespace Gtk;
using namespace Glib;


int main(int argc, char** argv) {
  
  // create the objects
  LV2UIClient lv2(argc, argv, true);
  if (!lv2.is_valid()) {
    cerr<<"Could not start OSC listener. You are not running the "<<endl
        <<"program manually, are you? It's supposed to be started by "<<endl
        <<"the plugin host."<<endl;
    return 1;
  }
  
  cerr<<"Running, "<<lv2.get_identifier()<<endl;
  
  Main kit(argc, argv);
  Window main_win;
  EuphoriaWidget euph;
  main_win.set_title(string("Euphoria: ") + lv2.get_identifier());
  main_win.add(euph);
  
  lv2.show_received.connect(mem_fun(main_win, &Window::show_all));
  lv2.hide_received.connect(mem_fun(main_win, &Window::hide));
  lv2.quit_received.connect(&Main::quit);
  main_win.signal_delete_event().connect(bind_return(hide(&Main::quit), true));
  
  // start
  lv2.send_update_request();
  Main::run();

  return 0;
}

