/****************************************************************************
    
    patternwidget.cpp - Simple sequencer widget
    
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
#include <cassert>

using namespace std;


#include "patternwidget.hpp"


using namespace Gtk;
using namespace Gdk;
using namespace Glib;


PatternWidget::PatternWidget(unsigned int steps, 
                             unsigned int sheight, unsigned int swidth)
  : m_white("#FFFFFF"),
    m_black("#000000"),
    m_grey1("#AAAAAA"),
    m_grey2("#666666"),
    m_green1("#A0FFA0"),
    m_green2("#006000"),
    m_steps(steps),
    m_sheight(sheight),
    m_swidth(swidth) {
  
  set_size_request(steps * (swidth + 1) - 1, 12 * (sheight + 1) - 1 + 25);
  
  RefPtr<Colormap> cmap = Colormap::get_system();
  cmap->alloc_color(m_white);
  cmap->alloc_color(m_black);
  cmap->alloc_color(m_grey1);
  cmap->alloc_color(m_grey2);
  cmap->alloc_color(m_green1);
  cmap->alloc_color(m_green2);
  
  add_events(KEY_PRESS_MASK | KEY_RELEASE_MASK |
             BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON1_MOTION_MASK);
  show_all();
}


bool PatternWidget::on_key_press_event(GdkEventKey* event) {
  return true;
}


bool PatternWidget::on_key_release_event(GdkEventKey* event) {
  return true;
}


bool PatternWidget::on_button_press_event(GdkEventButton* event) {
  return true;
}


bool PatternWidget::on_button_release_event(GdkEventButton* event) {
  return true;
}


bool PatternWidget::on_motion_notify_event(GdkEventMotion* event) {
  return true;
}


void PatternWidget::on_realize() {
  DrawingArea::on_realize();
  m_win = get_window();
  m_gc = GC::create(m_win);
  modify_bg(STATE_NORMAL, m_white);
}


bool PatternWidget::on_expose_event(GdkEventExpose* event) {
  
  // draw grid
  m_gc->set_foreground(m_grey1);
  for (unsigned int i = 1; i < m_steps; ++i)
    m_win->draw_line(m_gc, (m_swidth + 1) * i - 1, 0, 
                     (m_swidth + 1) * i - 1, get_height());
  for (unsigned int i = 1; i < 12; ++i)
    m_win->draw_line(m_gc, 0, (m_sheight + 1) * i - 1, 
                     get_width(), (m_sheight + 1) * i - 1);
  
  // draw octave chooser
  int y = (m_sheight + 1) * 12 - 1;
  m_gc->set_foreground(m_grey2);
  m_win->draw_rectangle(m_gc, true, 0, y, get_width(), get_height());
  m_gc->set_foreground(m_white);
  for (unsigned int i = 0; i < m_steps; ++i) {
    for (unsigned int o = 0; o < 11; ++o) {
      m_win->draw_line(m_gc, (m_swidth + 1) * i + 1, y + 2 + o * 2,
                       (m_swidth + 1) * i + m_swidth - 2, y + 2 + o * 2);
    }
  }
  
  return true;
}


