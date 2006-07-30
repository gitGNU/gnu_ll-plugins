/****************************************************************************
    
    AZR-3 - An LV2 synth plugin
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#include <iostream>
#include <vector>

#include <gtkmm.h>

#include "lv2uiclient.hpp"
#include "panelfx.xpm"
#include "voice.xpm"
#include "drawbar.hpp"
#include "knob.hpp"
#include "switch.hpp"
#include "Globals.h"


using namespace Gtk;
using namespace Gdk;
using namespace Glib;
using namespace std;


bool showing_fx_controls = true;
vector<Widget*> fx_widgets;
vector<Widget*> voice_widgets;


Knob* add_knob(Fixed& fbox, LV2UIClient& lv2, unsigned long port, 
              float min, float max, float value, 
              RefPtr<Pixmap>& bg, int xoffset, int yoffset) {
  RefPtr<Pixmap> spix = Pixmap::create(bg, 44, 44);
  spix->draw_drawable(GC::create(spix), bg, xoffset, yoffset,0, 0, 44, 44);
  Knob* knob = manage(new Knob(min, max, value));
  fbox.put(*knob, xoffset, yoffset);
  knob->get_window()->set_back_pixmap(spix, false);
  lv2.connect_adjustment(&knob->get_adjustment(), port);
  return knob;
}


Drawbar* add_drawbar(Fixed& fbox, LV2UIClient& lv2, unsigned long port, 
                     float min, float max, float value, 
                     RefPtr<Pixmap>& bg, int xoffset, int yoffset, 
                     Drawbar::Type type) {
  RefPtr<Pixmap> spix = Pixmap::create(bg, 22, 150);
  spix->draw_drawable(GC::create(spix), bg, xoffset, yoffset, 0, 0, 22, 150);
  Drawbar* db = manage(new Drawbar(min, max, value, type));
  fbox.put(*db, xoffset, yoffset);
  db->get_window()->set_back_pixmap(spix, false);
  lv2.connect_adjustment(&db->get_adjustment(), port);
  return db;
}


Switch* add_switch(Fixed& fbox, LV2UIClient& lv2, unsigned long port,
                int xoffset, int yoffset, Switch::Type type) {
  Switch* sw = manage(new Switch(type));
  fbox.put(*sw, xoffset, yoffset);
  lv2.connect_adjustment(&sw->get_adjustment(), port);
  return sw;
}


EventBox* add_clickbox(Fixed& fbox, int xoffset, int yoffset, 
                       int width, int height) {
  EventBox* eb = manage(new EventBox);
  eb->set_events(BUTTON_PRESS_MASK);
  eb->set_size_request(width, height);
  fbox.put(*eb, xoffset, yoffset);
  eb->set_visible_window(false);
  return eb;
}


bool change_mode(bool fx_mode, Fixed& fbox) {
  int x, y;
  if (fx_mode && !showing_fx_controls) {
    for (int i = 0; i < voice_widgets.size(); ++i) {
      voice_widgets[i]->translate_coordinates(fbox, 0, 0, x, y);
      fbox.move(*voice_widgets[i], x, y + 300);
    }
    for (int i = 0; i < fx_widgets.size(); ++i) {
      fx_widgets[i]->translate_coordinates(fbox, 0, 0, x, y);
      fbox.move(*fx_widgets[i], x, y - 300);
    }
  }
  else if (!fx_mode && showing_fx_controls) {
    for (int i = 0; i < fx_widgets.size(); ++i) {
      fx_widgets[i]->translate_coordinates(fbox, 0, 0, x, y);
      fbox.move(*fx_widgets[i], x, y + 300);
    }
    for (int i = 0; i < voice_widgets.size(); ++i) {
      voice_widgets[i]->translate_coordinates(fbox, 0, 0, x, y);
      fbox.move(*voice_widgets[i], x, y - 300);
    }
  }
  showing_fx_controls = fx_mode;
  return true;
}


int main(int argc, char** argv) {
  
  LV2UIClient lv2(argc, argv, true);
  if (!lv2.is_valid()) {
    cerr<<"Could not start OSC listener. You are not running the "<<endl
        <<"program manually, are you? It's supposed to be started by "<<endl
        <<"the plugin host."<<endl;
    return 1;
  }

  Main kit(argc, argv);
  Gtk::Window window;
  window.set_title(string("AZR-3: ") + lv2.get_identifier());
  Fixed fbox;
  fbox.set_has_window(true);
  Fixed vbox;
  vbox.set_has_window(true);
  fbox.put(vbox, 0, 615);
  voice_widgets.push_back(&vbox);
  window.add(fbox);
  RefPtr<Pixbuf> voicebuf = Pixbuf::create_from_xpm_data(voice);
  RefPtr<Pixbuf> pixbuf = Pixbuf::create_from_xpm_data(panelfx);
  fbox.set_size_request(pixbuf->get_width(), pixbuf->get_height());
  vbox.set_size_request(voicebuf->get_width(), voicebuf->get_height());
  window.show_all();
  RefPtr<Pixmap> pixmap = Pixmap::create(fbox.get_window(), 
                                         pixbuf->get_width(), 
                                         pixbuf->get_height());
  RefPtr<Pixmap> voicepxm = Pixmap::create(vbox.get_window(),
                                           voicebuf->get_width(),
                                           voicebuf->get_height());
  window.hide();
  RefPtr<Bitmap> bitmap;
  pixbuf->render_pixmap_and_mask(pixmap, bitmap, 10);
  voicebuf->render_pixmap_and_mask(voicepxm, bitmap, 10);
  fbox.get_window()->set_back_pixmap(pixmap, false);
  vbox.get_window()->set_back_pixmap(voicepxm, false);
  window.set_resizable(false);
  
  // keyboard split switch
  add_switch(fbox, lv2, n_split, 537, 49, Switch::Mini);
  
  // upper knobs
  add_switch(fbox, lv2, n_mono, 59, 92, Switch::BigRed);
  add_knob(fbox, lv2, n_click, 0, 1, 0.5, pixmap, 88, 88);
  add_knob(fbox, lv2, n_bender, 0, 1, 0.5, pixmap, 132, 88);
  add_knob(fbox, lv2, n_sustain, 0, 1, 0.5, pixmap, 176, 88);
  add_knob(fbox, lv2, n_shape, 0, 1, 0.5, pixmap, 220, 88);
  add_knob(fbox, lv2, n_perc, 0, 1, 0.5, pixmap, 308, 88);
  add_knob(fbox, lv2, n_percvol, 0, 1, 0.5, pixmap, 352, 88);
  add_knob(fbox, lv2, n_percfade, 0, 1, 0.5, pixmap, 396, 88);
  add_knob(fbox, lv2, n_vol1, 0, 1, 0.5, pixmap, 484, 88);
  add_knob(fbox, lv2, n_vol2, 0, 1, 0.5, pixmap, 528, 88);
  add_knob(fbox, lv2, n_vol3, 0, 1, 0.5, pixmap, 572, 88);
  add_knob(fbox, lv2, n_master, 0, 1, 0.5, pixmap, 616, 88);

  // perc and sustain switches
  add_switch(fbox, lv2, n_1_perc, 16, 173, Switch::Mini);
  add_switch(fbox, lv2, n_2_perc, 279, 173, Switch::Mini);
  add_switch(fbox, lv2, n_3_perc, 542, 173, Switch::Mini);
  add_switch(fbox, lv2, n_1_sustain, 16, 214, Switch::Mini);
  add_switch(fbox, lv2, n_2_sustain, 279, 214, Switch::Mini);
  add_switch(fbox, lv2, n_3_sustain, 542, 214, Switch::Mini);
  
  // drawbars for channel 1
  add_drawbar(fbox, lv2, n_1_db1, 0, 1, 0.5, pixmap, 42, 159, Drawbar::Brown);
  add_drawbar(fbox, lv2, n_1_db2, 0, 1, 0.5, pixmap, 66, 159, Drawbar::Brown);
  add_drawbar(fbox, lv2, n_1_db3, 0, 1, 0.5, pixmap, 90, 159, Drawbar::White);
  add_drawbar(fbox, lv2, n_1_db4, 0, 1, 0.5, pixmap, 114, 159, Drawbar::White);
  add_drawbar(fbox, lv2, n_1_db5, 0, 1, 0.5, pixmap, 138, 159, Drawbar::Black);
  add_drawbar(fbox, lv2, n_1_db6, 0, 1, 0.5, pixmap, 162, 159, Drawbar::White);
  add_drawbar(fbox, lv2, n_1_db7, 0, 1, 0.5, pixmap, 186, 159, Drawbar::Black);
  add_drawbar(fbox, lv2, n_1_db8, 0, 1, 0.5, pixmap, 210, 159, Drawbar::Black);
  add_drawbar(fbox, lv2, n_1_db9, 0, 1, 0.5, pixmap, 234, 159, Drawbar::White);
  
  // drawbars for channel 2
  add_drawbar(fbox, lv2, n_2_db1, 0, 1, 0.5, pixmap, 306, 159, Drawbar::Brown);
  add_drawbar(fbox, lv2, n_2_db2, 0, 1, 0.5, pixmap, 330, 159, Drawbar::Brown);
  add_drawbar(fbox, lv2, n_2_db3, 0, 1, 0.5, pixmap, 354, 159, Drawbar::White);
  add_drawbar(fbox, lv2, n_2_db4, 0, 1, 0.5, pixmap, 378, 159, Drawbar::White);
  add_drawbar(fbox, lv2, n_2_db5, 0, 1, 0.5, pixmap, 402, 159, Drawbar::Black);
  add_drawbar(fbox, lv2, n_2_db6, 0, 1, 0.5, pixmap, 426, 159, Drawbar::White);
  add_drawbar(fbox, lv2, n_2_db7, 0, 1, 0.5, pixmap, 450, 159, Drawbar::Black);
  add_drawbar(fbox, lv2, n_2_db8, 0, 1, 0.5, pixmap, 474, 159, Drawbar::Black);
  add_drawbar(fbox, lv2, n_2_db9, 0, 1, 0.5, pixmap, 498, 159, Drawbar::White);

  // drawbars for channel 3
  add_drawbar(fbox, lv2, n_3_db1, 0, 1, 0.5, pixmap, 570, 159, Drawbar::Brown);
  add_drawbar(fbox, lv2, n_3_db2, 0, 1, 0.5, pixmap, 594, 159, Drawbar::Brown);
  add_drawbar(fbox, lv2, n_3_db3, 0, 1, 0.5, pixmap, 618, 159, Drawbar::White);
  add_drawbar(fbox, lv2, n_3_db4, 0, 1, 0.5, pixmap, 642, 159, Drawbar::White);
  add_drawbar(fbox, lv2, n_3_db5, 0, 1, 0.5, pixmap, 666, 159, Drawbar::Black);
  
  // mode switcher
  Widget* eb = add_clickbox(fbox, 14, 319, 14, 44);
  eb->signal_button_press_event().connect(hide(bind(bind(&change_mode, 
                                                         ref(fbox)), false)));
  fx_widgets.push_back(eb);
  
  // Mr Valve controls
  fx_widgets.push_back(add_switch(fbox, lv2, n_mrvalve,
                                  39, 332, Switch::Green));
  fx_widgets.push_back(add_knob(fbox, lv2, n_drive, 
                                0, 1, 0.5, pixmap, 44, 352));
  fx_widgets.push_back(add_knob(fbox, lv2, n_set, 0, 1, 0.5, pixmap, 88, 352));
  fx_widgets.push_back(add_knob(fbox, lv2, n_tone, 0, 
                                1, 0.5, pixmap, 132, 352));
  fx_widgets.push_back(add_knob(fbox, lv2, n_mix, 0, 1, 
                                0.5, pixmap, 176, 352));
  
  // Speaker controls
  fx_widgets.push_back(add_switch(fbox, lv2, n_speakers, 
                                  302, 332, Switch::Green));
  fx_widgets.push_back(add_switch(fbox, lv2, n_speed, 
                                  323, 356, Switch::BigRed));
  fx_widgets.push_back(add_knob(fbox, lv2, n_l_slow, 
                                0, 1, 0.5, pixmap, 352, 352));
  fx_widgets.push_back(add_knob(fbox, lv2, n_l_fast, 
                                0, 1, 0.5, pixmap, 396, 352));
  fx_widgets.push_back(add_knob(fbox, lv2, n_u_slow, 
                                0, 1, 0.5, pixmap, 440, 352));
  fx_widgets.push_back(add_knob(fbox, lv2, n_u_fast, 
                                0, 1, 0.5, pixmap, 484, 352));
  fx_widgets.push_back(add_knob(fbox, lv2, n_belt, 
                                0, 1, 0.5, pixmap, 528, 352));
  fx_widgets.push_back(add_knob(fbox, lv2, n_spread, 
                                0, 1, 0.5, pixmap, 572, 352));
  fx_widgets.push_back(add_switch(fbox, lv2, n_complex, 
                                  443, 331, Switch::Mini));
  fx_widgets.push_back(add_switch(fbox, lv2, 
                                  n_pedalspeed, 510, 331, Switch::Mini));

  // mode switcher 2
  Widget* eb2 = add_clickbox(vbox, 14, 53, 14, 44);
  eb2->signal_button_press_event().connect(hide(bind(bind(&change_mode, 
                                                         ref(fbox)), true)));

  // vibrato controls
  add_switch(vbox, lv2, n_1_vibrato, 39, 17, Switch::Green);
  add_knob(vbox, lv2, n_1_vstrength, 0, 1, 0.5, voicepxm, 88, 37);
  add_knob(vbox, lv2, n_1_vmix, 0, 1, 0.5, voicepxm, 176, 37);
  add_switch(vbox, lv2, n_2_vibrato, 302, 17, Switch::Green);
  add_knob(vbox, lv2, n_2_vstrength, 0, 1, 0.5, voicepxm, 352, 37);
  add_knob(vbox, lv2, n_2_vmix, 0, 1, 0.5, voicepxm, 440, 37);
  
  
  lv2.show_received.connect(mem_fun(window, &Gtk::Window::show_all));
  lv2.hide_received.connect(mem_fun(window, &Gtk::Window::hide));
  lv2.quit_received.connect(&Main::quit);
  window.signal_delete_event().connect(bind_return(hide(&Main::quit), true));
  
  lv2.send_update_request();
  
  kit.run();

  return 0;
}
