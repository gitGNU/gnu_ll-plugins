/****************************************************************************
    
    transitioneditor.cpp - gtkmm widget for editing a transition matrix for
                           a 2D Markov chain
    
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
#include <valarray>

#include <cairomm/cairomm.h>

#include "transitioneditor.hpp"


using namespace Gtk;
using namespace std;


TransitionEditor::TransitionEditor()
  : m_margin(0),
    m_active_cell(C_NONE),
    m_dragging(false) {
  
  m_data[C_00] = 0.5;
  m_data[C_01] = 0.5;
  m_data[C_10] = 0.5;
  m_data[C_11] = 0.5;
  
  set_size_request(91, 91);
  
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MOTION_MASK | 
             Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
             Gdk::SCROLL_MASK);
  
  Gdk::Color bg;
  bg.set_rgb(10000, 10000, 15000);
  modify_bg(STATE_NORMAL, bg);
  
  using namespace Menu_Helpers;
  using namespace sigc;
  
  /*
  m_menu.items().
    push_back(MenuElem("New point", 
                       mem_fun(*this, &TransitionEditor::new_point)));
  m_menu.items().
    push_back(MenuElem("Delete point", 
                       mem_fun(*this, &TransitionEditor::delete_point)));
  m_menu.items().push_back(SeparatorElem());
  m_menu.items().
    push_back(MenuElem("Apply", mem_fun(*this, &TransitionEditor::apply)));
  */
}
 

bool TransitionEditor::on_expose_event(GdkEventExpose* event) {
  
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  Cairo::RefPtr<Cairo::Context> cc(new Cairo::Context(gdk_cairo_create(win->gobj())));
  cc->set_line_join(Cairo::LINE_JOIN_ROUND);
  
  int width = get_width();
  int height = get_height();
  double top = m_margin;
  double h = height - m_margin * 2;
  double w = width - m_margin * 2;
  
  cc->set_source_rgb(0.3, 0.9, 1);
  
  // grid
  cc->move_to(0, h / 2 + top);
  cc->line_to(width, h / 2 + top);
  cc->set_line_width(0.5);
  cc->stroke();
  cc->move_to(0, h / 4 + top);
  cc->line_to(width, h / 4 + top);
  cc->move_to(0, 3 * h / 4 + top);
  cc->line_to(width, 3 * h / 4 + top);
  cc->set_line_width(0.2);
  cc->stroke();
  cc->move_to(w / 2 + top, 0);
  cc->line_to(w / 2 + top, height);
  cc->set_line_width(0.5);
  cc->stroke();
  cc->move_to(w / 4 + top, 0);
  cc->line_to(w / 4 + top, height);
  cc->move_to(3 * w / 4 + top, 0);
  cc->line_to(3 * w / 4 + top, height);
  cc->set_line_width(0.2);
  cc->stroke();
  
  // circles
  cc->translate(x2p(0), y2p(0));
  cc->scale(w / 2.0, h / 2.0);
  cc->arc(-0.5, -0.5, 0.44 * m_data[C_00], 0, 2 * M_PI);
  if (m_dragging && m_active_cell == C_00)
    cc->set_source_rgb(1, 1, 0.6);
  cc->fill();
  cc->set_source_rgb(0.3, 0.9, 1);
  cc->arc(-0.5, 0.5, 0.44 * m_data[C_01], 0, 2 * M_PI);
  if (m_dragging && m_active_cell == C_01)
    cc->set_source_rgb(1, 1, 0.6);
  cc->fill();
  cc->set_source_rgb(0.3, 0.9, 1);
  cc->arc(0.5, -0.5, 0.44 * m_data[C_10], 0, 2 * M_PI);
  if (m_dragging && m_active_cell == C_10)
    cc->set_source_rgb(1, 1, 0.6);
  cc->fill();
  cc->set_source_rgb(0.3, 0.9, 1);
  cc->arc(0.5, 0.5, 0.44 * m_data[C_11], 0, 2 * M_PI);
  if (m_dragging && m_active_cell == C_11)
    cc->set_source_rgb(1, 1, 0.6);
  cc->fill();
  
  return true;
}


bool TransitionEditor::on_motion_notify_event(GdkEventMotion* event) {
  if (m_dragging) {
    float diff = (m_pix_drag_y - event->y) / 200.0;
    m_data[m_active_cell] = m_drag_value + diff;
    m_data[m_active_cell] = (m_data[m_active_cell] > 1 ? 
                             1 : m_data[m_active_cell]);
    m_data[m_active_cell] = (m_data[m_active_cell] < 0 ? 
                             0 : m_data[m_active_cell]);
    if (m_active_cell == C_00)
      m_data[C_01] = 1 - m_data[C_00];
    else if (m_active_cell == C_01)
      m_data[C_00] = 1 - m_data[C_01];
    else if (m_active_cell == C_10)
      m_data[C_11] = 1 - m_data[C_10];
    else if (m_active_cell == C_11)
      m_data[C_10] = 1 - m_data[C_11];
    queue_draw();
  }
  return true;
}


bool TransitionEditor::on_button_release_event(GdkEventButton* event) {
  if (event->button == 1)
    m_dragging = false;
  queue_draw();
  return true;
}


bool TransitionEditor::on_button_press_event(GdkEventButton* event) {
  if (p2x(int(event->x)) <= 0) {
    if (p2y(int(event->y)) > 0)
      m_active_cell = C_00;
    else
      m_active_cell = C_01;
  }
  else {
    if (p2y(int(event->y)) > 0)
      m_active_cell = C_10;
    else
      m_active_cell = C_11;
  }
  
  m_dragging = true;
  m_pix_drag_x = int(event->x);
  m_pix_drag_y = int(event->y);
  m_drag_value = m_data[m_active_cell];
  
  return true;
}


bool TransitionEditor::on_scroll_event(GdkEventScroll* event) {
  return true;
}
 

double TransitionEditor::x2p(double x) {
  return get_width() / 2.0 + (get_width() / 2.0  - m_margin) * x;
}


double TransitionEditor::y2p(double y) {
  return m_margin + (1 - y) * (get_height() / 2.0 - m_margin);
}


double TransitionEditor::p2x(int p) {
  return (p - get_width() / 2.0) / (get_width() / 2.0 - m_margin);
}


double TransitionEditor::p2y(int p) {
  return 1 - (p - m_margin) / (get_height() / 2.0 - m_margin);
}


