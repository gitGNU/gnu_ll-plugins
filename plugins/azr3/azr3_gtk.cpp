#include <iostream>

#include <gtkmm.h>

#include "lv2uiclient.hpp"
#include "panelfx.xpm"
#include "knob.hpp"
#include "switch.hpp"
#include "Globals.h"


using namespace Gtk;
using namespace Gdk;
using namespace Glib;


void add_knob(Fixed& fbox, LV2UIClient& lv2, unsigned long port, 
              float min, float max, float value, 
              RefPtr<Pixmap>& bg, int xoffset, int yoffset) {
  RefPtr<Pixmap> spix = Pixmap::create(bg, 44, 44);
  spix->draw_drawable(GC::create(spix), bg, xoffset, yoffset,0, 0, 44, 44);
  Knob* knob = manage(new Knob(min, max, value));
  fbox.put(*knob, xoffset, yoffset);
  knob->get_window()->set_back_pixmap(spix, false);
  lv2.connect_adjustment(&knob->get_adjustment(), port);
}


void add_switch(Fixed& fbox, LV2UIClient& lv2, unsigned long port,
                int xoffset, int yoffset, Switch::Type type) {
  Switch* sw = manage(new Switch(type));
  fbox.put(*sw, xoffset, yoffset);
  lv2.connect_adjustment(&sw->get_adjustment(), port);
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
  window.add(fbox);
  window.show_all();
  RefPtr<Pixbuf> pixbuf = Pixbuf::create_from_xpm_data(panelfx);
  fbox.set_size_request(pixbuf->get_width(), pixbuf->get_height());
  RefPtr<Pixmap> pixmap = Pixmap::create(fbox.get_window(), 
                                         pixbuf->get_width(), 
                                         pixbuf->get_height());
  RefPtr<Bitmap> bitmap;
  pixbuf->render_pixmap_and_mask(pixmap, bitmap, 10);
  fbox.get_window()->set_back_pixmap(pixmap, false);
  
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
  
  // Mr Valve controls
  add_switch(fbox, lv2, n_mrvalve, 39, 332, Switch::Green);
  add_knob(fbox, lv2, n_drive, 0, 1, 0.5, pixmap, 44, 352);
  add_knob(fbox, lv2, n_set, 0, 1, 0.5, pixmap, 88, 352);
  add_knob(fbox, lv2, n_tone, 0, 1, 0.5, pixmap, 132, 352);
  add_knob(fbox, lv2, n_mix, 0, 1, 0.5, pixmap, 176, 352);
  
  // Speaker controls
  add_switch(fbox, lv2, n_speakers, 302, 332, Switch::Green);
  add_switch(fbox, lv2, n_speed, 323, 356, Switch::BigRed);
  add_knob(fbox, lv2, n_l_slow, 0, 1, 0.5, pixmap, 352, 352);
  add_knob(fbox, lv2, n_l_fast, 0, 1, 0.5, pixmap, 396, 352);
  add_knob(fbox, lv2, n_u_slow, 0, 1, 0.5, pixmap, 440, 352);
  add_knob(fbox, lv2, n_u_fast, 0, 1, 0.5, pixmap, 484, 352);
  add_knob(fbox, lv2, n_belt, 0, 1, 0.5, pixmap, 528, 352);
  add_knob(fbox, lv2, n_spread, 0, 1, 0.5, pixmap, 572, 352);
  add_switch(fbox, lv2, n_complex, 443, 331, Switch::Mini);
  add_switch(fbox, lv2, n_pedalspeed, 510, 331, Switch::Mini);
  
  window.hide();
  
  lv2.show_received.connect(mem_fun(window, &Gtk::Window::show_all));
  lv2.hide_received.connect(mem_fun(window, &Gtk::Window::hide));
  lv2.quit_received.connect(&Main::quit);
  window.signal_delete_event().connect(bind_return(hide(&Main::quit), true));
  
  lv2.send_update_request();
  
  kit.run();

  return 0;
}
