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
  main_win.set_title(string("Euphoria: ") + lv2.get_identifier());
  VBox box(false, 3);
  box.set_border_width(9);
  main_win.add(box);
  
  cerr<<"do de do"<<endl;
  
  // controls
  HScale shape(0, 6, 1);
  box.pack_start(shape);
  lv2.connect_adjustment(shape.get_adjustment(), 1);
  HScale timbre(0, 1, 0.0001);
  box.pack_start(timbre);
  lv2.connect_adjustment(timbre.get_adjustment(), 2);
  HScale spread(0, 0.1, 0.0001);
  box.pack_start(spread);
  lv2.connect_adjustment(spread.get_adjustment(), 3);
  HScale width(0, 1, 0.0001);
  box.pack_start(width);
  lv2.connect_adjustment(width.get_adjustment(), 4);
  HScale gainmod(-1, 1, 0.0001);
  box.pack_start(gainmod);
  lv2.connect_adjustment(gainmod.get_adjustment(), 5);
  HScale freqmod(-1, 10, 0.0001);
  box.pack_start(freqmod);
  lv2.connect_adjustment(freqmod.get_adjustment(), 6);
  HScale timbremod(-1, 1, 0.0001);
  box.pack_start(timbremod);
  lv2.connect_adjustment(timbremod.get_adjustment(), 7);
  HScale decay(0.999, 0.99999, 0.00001);
  box.pack_start(decay);
  lv2.connect_adjustment(decay.get_adjustment(), 8);
  HScale release(0.01, 10, 0.01);
  box.pack_start(release);
  lv2.connect_adjustment(release.get_adjustment(), 9);

  cerr<<"do de do 2"<<endl;

  lv2.show_received.connect(mem_fun(main_win, &Window::show_all));
  lv2.hide_received.connect(mem_fun(main_win, &Window::hide));
  lv2.quit_received.connect(&Main::quit);
  main_win.signal_delete_event().connect(bind_return(hide(&Main::quit), true));
  
  cerr<<"do de do 3"<<endl;

  // start
  lv2.send_update_request();
  Main::run();
  
  cerr<<"do de do 4"<<endl;

  return 0;
}


