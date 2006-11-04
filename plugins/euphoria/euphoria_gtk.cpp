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


void set_program(int program) {
  cerr<<"Program "<<program<<" selected!"<<endl;
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
  EuphoriaWidget euph(lv2);
  main_win.set_title(string("Euphoria: ") + lv2.get_identifier());
  main_win.add(euph);
  
  lv2.show_received.connect(mem_fun(main_win, &Window::show_all));
  lv2.hide_received.connect(mem_fun(main_win, &Window::hide));
  lv2.quit_received.connect(&Main::quit);
  lv2.add_program_received.connect(mem_fun(euph, &EuphoriaWidget::add_program));
  lv2.remove_program_received.
    connect(mem_fun(euph, &EuphoriaWidget::remove_program));
  lv2.clear_programs_received.
    connect(mem_fun(euph, &EuphoriaWidget::clear_programs));
  euph.signal_program_selected.connect(mem_fun(lv2,&LV2UIClient::send_program));
  euph.signal_shape_changed.
    connect(bind<0>(mem_fun(lv2, &LV2UIClient::send_configure), "shape"));
  main_win.signal_delete_event().connect(bind_return(hide(&Main::quit), true));
  
  // start
  lv2.send_update_request();
  Main::run();

  return 0;
}


