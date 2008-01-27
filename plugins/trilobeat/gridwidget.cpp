/****************************************************************************
    
    gridwidget.cpp - Simple drum sequencer widget
    
    Copyright (C) 2008 Lars Luthman <lars.luthman@gmail.com>
    
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
#include <sstream>
#include <string>

#include <gdk/gdkkeysyms.h>

#include "gridwidget.hpp"


using namespace Gtk;
using namespace Gdk;
using namespace Glib;


GridWidget::GridWidget(unsigned int steps, 
		       unsigned int sheight, unsigned int swidth,
		       std::string const& background)
  : m_steps(steps),
    m_sheight(sheight),
    m_swidth(swidth),
    m_active_keys(128),
    m_seq(128, std::vector<unsigned char>(m_steps)),
    m_active_x(-1),
    m_active_y(-1),
    m_rows(0),
    m_background(background) {
  
  m_blue.set_rgb(0, 39168, 65535);
  m_dark.set_rgb(0, 0, 30000);
  m_light.set_rgb(40000, 50000, 65535);
  m_yellow.set_rgb(65535, 65535, 0);
  Glib::RefPtr<Gdk::Colormap> cmap = Gdk::Colormap::get_system();
  cmap->alloc_color(m_blue);
  cmap->alloc_color(m_dark);
  cmap->alloc_color(m_light);
  cmap->alloc_color(m_yellow);
  
  set_size_request(m_steps * m_swidth - 1, sheight - 1);
  add_events(EXPOSURE_MASK | BUTTON_PRESS_MASK | SCROLL_MASK |
	     KEY_PRESS_MASK | KEY_RELEASE_MASK);
  set_flags(get_flags() | CAN_FOCUS);
}


void GridWidget::set_key_active(unsigned char key, bool active) {
  if (key > 127)
    return;
  m_active_keys[key] = active;
  int n = 0;
  for (unsigned i = 0; i < 128; ++i)
    n += m_active_keys[i] ? 1 : 0;
  m_rows = n;
  if (m_rows && m_active_y == -1)
    m_active_y = m_active_x = 0;
  n = n ? n : 1;
  set_size_request(m_steps * m_swidth - 1, n * m_sheight - 1);
  queue_draw();
}


void GridWidget::set_velocity(unsigned char key, 
			      unsigned step, unsigned char velocity) {
  if (key >= 128 || step >= m_steps || velocity >= 128)
    return;
  m_seq[key][step] = velocity;
  if (m_active_keys[key])
    queue_draw();
}


bool GridWidget::on_button_press_event(GdkEventButton* event) {
  if (event->button != 1)
    return false;
  int row = event->y / m_sheight;
  int step = event->x / m_swidth;
  if (step < 0 || step >= m_steps)
    return false;
  unsigned char key = row2key(row);
  if (key > 127)
    return false;
  m_active_y = row;
  m_active_x = step;
  if (m_seq[key][step])
    signal_velocity_changed(key, step, 0);
  else
    signal_velocity_changed(key, step, 64);
  return true;
}


bool GridWidget::on_expose_event(GdkEventExpose* event) {
  
  // draw notes
  int row = 0;
  for (int key = 127; key >= 0; --key) {
    if (m_active_keys[key]) {
      for (int step = 0; step < m_steps; ++step) {
	if (m_seq[key][step]) {
	  m_gc->set_foreground(m_blue);
	  m_win->draw_rectangle(m_gc, true, 
				m_swidth * step, m_sheight * row,
				m_swidth - 2, m_sheight - 2);
	  m_gc->set_foreground(m_dark);
	  m_win->draw_line(m_gc, 
			   m_swidth * step + m_swidth - 2, 
			   m_sheight * row + m_sheight - 2,
			   m_swidth * step + m_swidth - 2, 
			   m_sheight * row);
	  m_win->draw_line(m_gc, 
			   m_swidth * step + m_swidth - 2, 
			   m_sheight * row + m_sheight - 2,
			   m_swidth * step, 
			   m_sheight * row + m_sheight - 2);
	  m_gc->set_foreground(m_light);
	  m_win->draw_line(m_gc, m_swidth * step, m_sheight * row,
			   m_swidth * step, m_sheight * row + m_sheight - 2);
	  m_win->draw_line(m_gc, m_swidth * step, m_sheight * row,
			   m_swidth * step + m_swidth - 2, m_sheight * row);
	}
      }
      ++row;
    }
  }
  
  // draw active box
  if (m_active_y >= 0 && m_active_y < row && 
      m_active_x >= 0 && m_active_x < m_steps) {
    m_gc->set_foreground(m_yellow);
    m_win->draw_rectangle(m_gc, false, 
			  m_swidth * m_active_x - 1, m_sheight * m_active_y - 1,
			  m_swidth, m_sheight);
  }
  
  return true;
}


bool GridWidget::on_key_press_event(GdkEventKey* event) {
  
  switch (event->keyval) {
    
    // left, right, up and down arrow keys move the cursor
  case GDK_Left:
    if (m_active_x > 0) {
      --m_active_x;
      queue_draw();
    }
    break;
    
  case GDK_Right:
    if (m_active_x >= 0 && m_active_x < m_steps - 1) {
      ++m_active_x;
      queue_draw();
    }
    break;
    
  case GDK_Up:
    if (m_active_y > 0) {
      --m_active_y;
      queue_draw();
    }
    break;
    
  case GDK_Down:
    if (m_active_y >= 0 && m_active_y < m_rows - 1) {
      ++m_active_y;
      queue_draw();
    }
    break;
    
    // space toggles the note on and off
  case ' ':
    if (m_active_x >= 0 && m_active_x < m_steps) {
      unsigned char k = row2key(m_active_y);
      if (k < 128) {
	if (m_seq[k][m_active_x])
	  signal_velocity_changed(k, m_active_x, 0);
	else
	  signal_velocity_changed(k, m_active_x, 64);
      }
    }
    break;
  }
  
  return true;
}


bool GridWidget::on_key_release_event(GdkEventKey* event) {
  return true;
}


void GridWidget::on_realize() {
  DrawingArea::on_realize();
  m_win = get_window();
  m_gc = GC::create(m_win);
  if (m_background != "") {
    RefPtr<Pixbuf> pixbuf = Pixbuf::create_from_file(m_background);
    int w = pixbuf->get_width();
    int h = pixbuf->get_height();
    RefPtr<Pixmap> pixmap = Pixmap::create(get_window(), w, h);
    RefPtr<GC> gc = GC::create(pixmap);
    pixmap->draw_pixbuf(gc, pixbuf, 0, 0, 0, 0, w, h, RGB_DITHER_NONE, 0, 0);
    RefPtr<Style> s = get_style()->copy();
    s->set_bg_pixmap(STATE_NORMAL, pixmap);
    s->set_bg_pixmap(STATE_ACTIVE, pixmap);
    s->set_bg_pixmap(STATE_PRELIGHT, pixmap);
    s->set_bg_pixmap(STATE_SELECTED, pixmap);
    s->set_bg_pixmap(STATE_INSENSITIVE, pixmap);
    set_style(s);
  }
}


unsigned char GridWidget::row2key(int row) {
  if (row < 0)
    return -1;
  int c, i;
  for (i = 127, c = -1; i >= 0 && c < row; --i)
    c += m_active_keys[i] ? 1 : 0;
  if (i >= 0)
    return (unsigned char)(i + 1);
  return 128;
}

