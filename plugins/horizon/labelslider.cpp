/****************************************************************************

    labelslider.cpp

    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#include <cstdio>
#include <iostream>

#include "labelslider.hpp"


using namespace std;


LabelSlider::LabelSlider(double lower, double upper, double value, int digits)
  : m_adj(value, lower, upper),
    m_drag_x(0),
    m_drag_value(value),
    m_digits(digits) {
  
  m_black.set_rgb(0, 0, 0);
  m_on.set_rgb(20000, 20000, 65500);
  Glib::RefPtr<Gdk::Colormap> cmap = Gdk::Colormap::get_system();
  cmap->alloc_color(m_black);
  cmap->alloc_color(m_on);
  
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MOTION_MASK | 
             Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK);
  
  m_adj.signal_value_changed().connect(mem_fun(this, &LabelSlider::queue_draw));
  m_adj.signal_changed().connect(mem_fun(this, &LabelSlider::queue_draw));
  
  m_layout = Pango::Layout::create(get_pango_context());

  char buf[50];
  snprintf(buf, 50, "<b><small>%0.*f</small></b>", m_digits, m_adj.get_value());
  m_layout->set_markup(buf);
  int lw, lh;
  m_layout->get_pixel_size(lw, lh);
  set_size_request(int(lw * 1.1), int(lh * 1.1));
}

  
Gtk::Adjustment& LabelSlider::get_adjustment() {
  return m_adj;
}


bool LabelSlider::on_expose_event(GdkEventExpose* event) {
  
  char buf[50];
  snprintf(buf, 50, "<b><small>%0.*f</small></b>", m_digits, m_adj.get_value());
  m_layout->set_markup(buf);
  int lw, lh;
  m_layout->get_pixel_size(lw, lh);

  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  gc->set_foreground(m_black);
  int w = get_width();
  int h = get_height();
  win->draw_rectangle(gc, false, 0, 0, w - 1, h - 1);
  int x = int((w - 2) * (m_adj.get_value() - m_adj.get_lower()) / 
	      (m_adj.get_upper() - m_adj.get_lower()));
  gc->set_foreground(m_on);
  win->draw_rectangle(gc, true, 1, 1, x, h - 2);
  gc->set_foreground(m_black);
  win->draw_layout(gc, (w - lw) / 2, (h - lh) / 2, m_layout);
  return true;
}


bool LabelSlider::on_motion_notify_event(GdkEventMotion* event) {
  double d = (m_adj.get_upper() - m_adj.get_lower()) * 
    (event->x - m_drag_x) / (get_width() - 2);
  double value = m_drag_value + d;
  value = value > m_adj.get_upper() ? m_adj.get_upper() : value;
  value = value < m_adj.get_lower() ? m_adj.get_lower() : value;
  m_adj.set_value(value);
  return true;
}


bool LabelSlider::on_button_press_event(GdkEventButton* event) {
  if (event->button == 1) {
    m_drag_x = int(event->x);
    m_drag_value = m_adj.get_value();
  }
  return true;
}


bool LabelSlider::on_scroll_event(GdkEventScroll* event) {
  double value = m_adj.get_value();
  if (event->direction == GDK_SCROLL_UP) {
    value += 0.001 * (m_adj.get_upper() - m_adj.get_lower());
    value = value > m_adj.get_upper() ? m_adj.get_upper() : value;
    m_adj.set_value(value);
  }
  else if (event->direction == GDK_SCROLL_DOWN) {
    value -= 0.001 * (m_adj.get_upper() - m_adj.get_lower());
    value = value < m_adj.get_lower() ? m_adj.get_lower() : value;
    m_adj.set_value(value);
  }
  return true;
}
