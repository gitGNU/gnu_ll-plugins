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
#include "num_yellow.xpm"


using namespace Gtk;
using namespace Gdk;
using namespace Glib;
using namespace std;


Knob::Knob(float min, float max, float value, 
           float dmin, float dmax, bool decimal) 
  : m_adj(value, min, max),
    m_dmin(dmin),
    m_dmax(dmax),
    m_decimal(decimal) {
  
  if (m_dmax >= 1000)
    m_decimal = false;
  
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
  m_digbit = Bitmap::create(bits, 5, 84);
  m_digpix = Pixmap::create_from_xpm(Colormap::get_system(), m_digbit, 
                                     num_yellow);
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
  
  draw_digits(win, gc);
  
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


void Knob::draw_digits(RefPtr<Gdk::Window>& win, RefPtr<GC>& gc) {
  
  int xoffset = 7;
  if (m_dmax >= 1000)
    xoffset += 5;
  
  float dvalue = (m_adj.get_value() - m_adj.get_lower()) / 
    (m_adj.get_upper() - m_adj.get_lower());
  dvalue *= (m_dmax - m_dmin);
  dvalue += m_dmin;
  
  if (dvalue >= 1000)
    draw_digit(win, gc, xoffset, int(dvalue) / 1000);
  xoffset += 5;
  if (dvalue >= 100)
    draw_digit(win, gc, xoffset, (int(dvalue) / 100) % 10);
  xoffset += 5;
  if (dvalue >= 10)
    draw_digit(win, gc, xoffset, (int(dvalue) / 10) % 10);
  xoffset += 5;
  draw_digit(win, gc, xoffset, int(dvalue) % 10);
  if (m_decimal) {
    draw_digit(win, gc, xoffset, 10);
    xoffset += 5;
    draw_digit(win, gc, xoffset, int(dvalue * 10) % 10);
  }
}


void Knob::draw_digit(RefPtr<Gdk::Window>& win, RefPtr<GC>& gc,
                      int xoffset, int digit) {
  //  cerr<<__PRETTY_FUNCTION__<<" with xoffset = "<<xoffset<<", digit = "<<digit<<endl;
  gc->set_clip_mask(m_digbit);
  gc->set_clip_origin(xoffset, 18 - digit * 7 + (digit < 10 ? 0 : 1));
  win->draw_drawable(gc, m_digpix, 0, digit * 7, 
                     xoffset, 18 + (digit < 10 ? 0 : 1), 5, 7);
}
