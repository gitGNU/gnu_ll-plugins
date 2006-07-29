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

#include "knob.hpp"
#include "cknob.xpm"


using namespace Gtk;
using namespace Gdk;
using namespace Glib;
using namespace std;


Knob::Knob(float min, float max, float value) 
  : m_adj(value, min, max) {
  set_size_request(44, 44);
  add_events(EXPOSURE_MASK | BUTTON1_MOTION_MASK | BUTTON_PRESS_MASK);
  m_adj.signal_value_changed().connect(mem_fun(*this, &Knob::queue_draw));
}
 
 
void Knob::set_value(float value) {
  m_adj.set_value(value);
}
  

Gtk::Adjustment& Knob::get_adjustment() {
  return m_adj;
}


void Knob::on_realize() {
  DrawingArea::on_realize();
  unsigned pixsize = (44 * 1804 + 1) / 8;
  char bits[pixsize];
  memset(bits, 0, sizeof(char) * pixsize);
  m_bitmap = Bitmap::create(bits, 44, 1804);
  m_pixmap = Pixmap::create_from_xpm(Colormap::get_system(), m_bitmap, cknob);
}


bool Knob::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  RefPtr<GC> gc = GC::create(win);
  
  float value = (m_adj.get_value() - m_adj.get_lower()) / 
    (m_adj.get_upper() - m_adj.get_lower());
  value = value < 0 ? 0 : value;
  value = value > 1 ? 1 : value;
  value = 40 * value;
  
  gc->set_clip_mask(m_bitmap);
  gc->set_clip_origin(0, -44 * int(value));
  win->draw_drawable(gc, m_pixmap, 0, 44 * int(value), 0, 0, 44, 44);
  return true;
}


bool Knob::on_motion_notify_event(GdkEventMotion* event) {
  float value = m_value_offset + ((m_click_offset - event->y) / 200.0) * 
    (m_adj.get_upper() - m_adj.get_lower());
  value = value < m_adj.get_lower() ? m_adj.get_lower() : value;
  value = value > m_adj.get_upper() ? m_adj.get_upper() : value;
  m_adj.set_value(value);
  return true;
}


bool Knob::on_button_press_event(GdkEventButton* event) {
  m_click_offset = (int)event->y;
  m_value_offset = m_adj.get_value();
  return true;
}
