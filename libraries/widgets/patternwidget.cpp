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
#include <list>
#include <sstream>

#include "patternwidget.hpp"


using namespace Gtk;
using namespace Gdk;
using namespace Glib;
using namespace std;


PatternWidget::PatternWidget(unsigned int steps, 
                             unsigned int sheight, unsigned int swidth)
  : m_white("#FFFFFF"),
    m_black("#000000"),
    m_grey1("#E0E0E0"),
    m_grey2("#666666"),
    m_grey3("#333333"),
    m_green1("#00FF00"),
    m_green2("#006000"),
    m_purple("#FF00FF"),
    m_steps(steps),
    m_sheight(sheight),
    m_swidth(swidth),
    m(steps) {
  
  m_vel_table.push_back(7);
  m_vel_table.push_back(19);
  m_vel_table.push_back(31);
  m_vel_table.push_back(43);
  m_vel_table.push_back(55);
  m_vel_table.push_back(67);
  m_vel_table.push_back(79);
  m_vel_table.push_back(91);
  m_vel_table.push_back(103);
  m_vel_table.push_back(115);
  m_vel_table.push_back(127);
  
  set_size_request(526, 265);
  
  RefPtr<Colormap> cmap = Colormap::get_system();
  cmap->alloc_color(m_white);
  cmap->alloc_color(m_black);
  cmap->alloc_color(m_grey1);
  cmap->alloc_color(m_grey2);
  cmap->alloc_color(m_grey3);
  cmap->alloc_color(m_green1);
  cmap->alloc_color(m_green2);
  cmap->alloc_color(m_purple);
  
  add_events(KEY_PRESS_MASK | KEY_RELEASE_MASK | SCROLL_MASK |
             BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON1_MOTION_MASK);
  show_all();
}


string PatternWidget::get_string() const {
  ostringstream oss;
  oss<<m.size();
  for (unsigned i = 0; i < m.size(); ++i) {
    oss<<" "
       <<(m[i].on ? 1 : 0)<<" "
       <<int(m[i].note)<<" "
       <<(m[i].slide ? 1 : 0)<<" "
       <<int(m_vel_table[m[i].velocity]);
  }
  return oss.str();
}


bool PatternWidget::on_key_press_event(GdkEventKey* event) {
  return true;
}


bool PatternWidget::on_key_release_event(GdkEventKey* event) {
  return true;
}


bool PatternWidget::on_button_press_event(GdkEventButton* event) {
  int step = x2step(event->x);
  int line = y2line(event->y);
  if (step >= 0 && step < (int)m_steps && line >= 0 && line < 12) {
    if (event->button == 1) {
      if (m[step].on && m[step].note % 12 == line) {
        m[step].on = false;
        m[step].note = 64;
        m[step].slide = false;
        signal_sequence_changed();
      }
      else {
        m[step].on = true;
        m[step].note = line + 12 * (m[step].note / 12);
        signal_sequence_changed();
      }
    }
    else if (event->button == 3) {
      if (m[step].on) {
        if (m[step].slide)
          m[step].slide = false;
        else
          m[step].slide = true;
        signal_sequence_changed();
      }
    }
    queue_draw();
  }
  return true;
}


bool PatternWidget::on_button_release_event(GdkEventButton* event) {
  return true;
}


bool PatternWidget::on_motion_notify_event(GdkEventMotion* event) {
  return true;
}


bool PatternWidget::on_scroll_event(GdkEventScroll* event) {
  int step = x2step(event->x);

  if (step >= 0 && step < (int)m_steps && m[step].on && event->y > 228) {
    if (event->direction == GDK_SCROLL_UP && m[step].velocity < 10) {
      m[step].velocity += 1;
      signal_sequence_changed();
    }
    else if (event->direction == GDK_SCROLL_DOWN && m[step].velocity > 0) {
      m[step].velocity -= 1;
      signal_sequence_changed();
    }
    queue_draw();
  }
  
  else if (step >= 0 && step < (int)m_steps && m[step].on && event->y > 192) {
    if (event->direction == GDK_SCROLL_UP && m[step].note < 120) {
      m[step].note += 12;
      signal_sequence_changed();
    }
    else if (event->direction == GDK_SCROLL_DOWN && m[step].note > 11) {
      m[step].note -= 12;
      signal_sequence_changed();
    }
    queue_draw();
  }
  
  return true;
}


void PatternWidget::on_realize() {
  DrawingArea::on_realize();
  m_win = get_window();
  m_gc = GC::create(m_win);
  modify_bg(STATE_NORMAL, m_white);
}


bool PatternWidget::on_expose_event(GdkEventExpose* event) {
  
  /*
  // draw grid
  m_gc->set_foreground(m_grey1);
  for (unsigned int i = 1; i < m_steps; ++i)
    m_win->draw_line(m_gc, (m_swidth + 1) * i - 1, 0, 
                     (m_swidth + 1) * i - 1, get_height());
  for (unsigned int i = 1; i < 12; ++i)
    m_win->draw_line(m_gc, 0, (m_sheight + 1) * i - 1, 
                     get_width(), (m_sheight + 1) * i - 1);
  */
  
  // draw notes
  bool sliding = false;
  int sliding_from = 0;
  int x = 14;
  for (unsigned i = 0; i < m_steps; ++i) {
    int key = m[i].note % 12;
    if (m[i].on) {

      m_gc->set_foreground(m_black);
      m_win->draw_rectangle(m_gc, false, (m_swidth + 1) * i + x, 
                            (11 - key) * (m_sheight + 1) - 1, 
                            m_swidth + 1, m_sheight + 1);

      m_gc->set_foreground(m_green1);
      m_win->draw_rectangle(m_gc, true, (m_swidth + 1) * i + x + 1,
                            (11 - key) * (m_sheight + 1), 
                            m_swidth, m_sheight);
      
      if (sliding) {
        int kmin = key;
        kmin = sliding_from < kmin ? sliding_from : kmin;
        int kmax = key;
        kmax = sliding_from > kmin ? sliding_from : kmax;
        m_win->draw_rectangle(m_gc, true, x + (m_swidth + 1) * i - 1, 
                              (11 - kmax) * (m_sheight + 1), m_swidth / 6,
                              (kmax - kmin + 1) * (m_sheight + 1) - 1);
        m_gc->set_foreground(m_black);
        if (key > kmin) {
          m_win->draw_line(m_gc, x + (m_swidth + 1) * i - 1, 
                           (11 - kmin) * (m_sheight + 1) - 1,
                           x + (m_swidth + 1) * i - 1,
                           (11 - key) * (m_sheight + 1) - 1);
          m_win->draw_line(m_gc, x + (m_swidth + 1) * i - 1,
                           (12 - kmin) * (m_sheight + 1) - 1,
                           x + (m_swidth + 1) * i - 1 + m_swidth / 6,
                           (12 - kmin) * (m_sheight + 1) - 1);
          m_win->draw_line(m_gc, x + (m_swidth + 1) * i - 1 + m_swidth / 6, 
                           (12 - kmin) * (m_sheight + 1) - 1,
                           x + (m_swidth + 1) * i - 1 + m_swidth / 6,
                           (12 - key) * (m_sheight + 1) - 1);
        }
        else if (key < kmax) {
          m_win->draw_line(m_gc, x + (m_swidth + 1) * i - 1, 
                           (12 - kmax) * (m_sheight + 1) - 1,
                           x + (m_swidth + 1) * i - 1,
                           (12 - key) * (m_sheight + 1) - 1);
          m_win->draw_line(m_gc, x + (m_swidth + 1) * i - 1,
                           (11 - kmax) * (m_sheight + 1) - 1,
                           x + (m_swidth + 1) * i - 1 + m_swidth / 6,
                           (11 - kmax) * (m_sheight + 1) - 1);
          m_win->draw_line(m_gc, x + (m_swidth + 1) * i - 1 + m_swidth / 6, 
                           (11 - kmax) * (m_sheight + 1) - 1,
                           x + (m_swidth + 1) * i - 1 + m_swidth / 6,
                           (11 - key) * (m_sheight + 1) - 1);
        }
      }
      
    }
    
    sliding = m[i].on && m[i].slide;
    sliding_from = key;
  }
  
  /*
  // draw octave chooser
  m_gc->set_foreground(m_grey2);
  m_win->draw_rectangle(m_gc, true, 0, y, get_width(), get_height());
  m_gc->set_foreground(m_grey3);
  for (unsigned int i = 0; i < m_steps; ++i) {
    for (unsigned int o = 0; o < 11; ++o) {
      m_win->draw_line(m_gc, (m_swidth + 1) * i + 1, y + 3 + o * 3,
                       (m_swidth + 1) * i + m_swidth - 2, y + 3 + o * 3);
    }
  }
  */
  
  // draw octave lines
  int y = 195;
  m_gc->set_foreground(m_green1);
  for (unsigned i = 0; i < m_steps; ++i) {
    if (m[i].on) {
      int octave = m[i].note / 12;
      m_win->draw_line(m_gc, x + 1 + (m_swidth + 1) * i + 1, 
                       y + (10 - octave) * 3,
                       x + 1 + (m_swidth + 1) * i + m_swidth - 2, 
                       y + (10 - octave) * 3);
    }
  }

  // draw velocity lines
  y = 232;
  m_gc->set_foreground(m_purple);
  for (unsigned i = 0; i < m_steps; ++i) {
    if (m[i].on) {
      int vel = m[i].velocity;
      m_win->draw_line(m_gc, x + 1 + (m_swidth + 1) * i + 1, 
                       y + (10 - vel) * 3,
                       x + 1 + (m_swidth + 1) * i + m_swidth - 2, 
                       y + (10 - vel) * 3);
    }
  }
  
  return true;
}


int PatternWidget::x2step(double x) {
  if (x < 15)
    return -1;
  return int((x - 15) / (m_swidth + 1));
}


int PatternWidget::y2line(double y) {
  if (y >= (m_sheight + 1) * 12 - 1)
    return -1;
  return int(((m_sheight + 1) * 12 - 1 - y) / (m_sheight + 1));
}

