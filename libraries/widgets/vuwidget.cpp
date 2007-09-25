/****************************************************************************
    
    vuwidget.cpp - simple VU meter
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
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

#include "vuwidget.hpp"


VUWidget::VUWidget() 
  : m_value(0),
    m_peak(0) {
  set_size_request(16, 150);
  m_bg.set_rgb(10000, 10000, 15000);
  m_fg1.set_rgb(0, 65000, 65000);
  m_fg2.set_rgb(65000, 45000, 0);
  m_fg3.set_rgb(65000, 0, 0);
  m_fg1b.set_rgb(8500, 16000, 26000);
  m_fg2b.set_rgb(16000, 14000, 12500);
  m_fg3b.set_rgb(16000, 8500, 12500);
  Glib::RefPtr<Gdk::Colormap> cmap = Gdk::Colormap::get_system();
  cmap->alloc_color(m_bg);
  cmap->alloc_color(m_fg1);
  cmap->alloc_color(m_fg2);
  cmap->alloc_color(m_fg3);
  cmap->alloc_color(m_fg1b);
  cmap->alloc_color(m_fg2b);
  cmap->alloc_color(m_fg3b);
}

  
void VUWidget::set_value(float value) {
  m_value = value;
  if (m_value > m_peak) {
    m_peak = m_value;
    m_peak_connection.disconnect();
    m_peak_connection = Glib::signal_timeout().
      connect(bind_return(mem_fun(*this, &VUWidget::clear_peak), false), 3000);
  }
  queue_draw();
}
  

bool VUWidget::on_expose_event(GdkEventExpose* event) {
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  gc->set_foreground(m_bg);
  win->draw_rectangle(gc, true, 0, 0, get_width(), get_height());
  gc->set_foreground(m_fg1);
  unsigned int n = (get_height() - 4) / 3;
  int level = 1;
  bool active = true;
  for (unsigned i = 0; i < n; ++i) {
    if (m_value * 0.8 * n <= i) {
      active = false;
      if (level == 1)
	gc->set_foreground(m_fg1b);
      else if (level == 2)
	gc->set_foreground(m_fg2b);
      if (level == 3)
	gc->set_foreground(m_fg3b);
    }
    if (level == 1 && 0.6 * n <= i) {
      if (active)
	gc->set_foreground(m_fg2);
      else
	gc->set_foreground(m_fg2b);
      level = 2;
    }
    if (level == 2 && 0.8 * n <= i) {
      if (active)
	gc->set_foreground(m_fg3);
      else
	gc->set_foreground(m_fg3b);
      level = 3;
    }
    win->draw_rectangle(gc, true, 2, get_height() - (2 + 3 * i + 3), 
			get_width() - 4, 2);
  }
  
  if (m_peak > 0) {
    unsigned i = m_peak * 0.8 * n;
    if (i >= n)
      i = n - 1;
    if (m_peak * 0.8 <= 0.6)
      gc->set_foreground(m_fg1);
    else if (m_peak * 0.8 <= 0.8)
      gc->set_foreground(m_fg2);
    else
      gc->set_foreground(m_fg3);
    win->draw_rectangle(gc, true, 2, get_height() - (2 + 3 * i + 3), 
			get_width() - 4, 2);
  }

}


void VUWidget::clear_peak() {
  m_peak = 0;
  queue_draw();
}

