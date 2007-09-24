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
  : m_value(0) {
  set_size_request(16, 150);
  m_bg.set_rgb(10000, 10000, 15000);
  m_fg1.set_rgb(0, 65000, 65000);
  m_fg2.set_rgb(65000, 45000, 0);
  m_fg3.set_rgb(65000, 0, 0);
  Glib::RefPtr<Gdk::Colormap> cmap = Gdk::Colormap::get_system();
  cmap->alloc_color(m_bg);
  cmap->alloc_color(m_fg1);
  cmap->alloc_color(m_fg2);
  cmap->alloc_color(m_fg3);
}

  
void VUWidget::set_value(float value) {
  m_value = value;
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
  for (unsigned i = 0; i < n; ++i) {
    if (m_value * 0.8 * n <= i)
      break;
    if (level == 1 && 0.6 * n <= i) {
      gc->set_foreground(m_fg2);
      level = 2;
    }
    if (level == 2 && 0.8 * n <= i) {
      gc->set_foreground(m_fg3);
      level = 3;
    }
    win->draw_rectangle(gc, true, 2, get_height() - (2 + 3 * i + 3), 
			get_width() - 4, 2);
  }
}
