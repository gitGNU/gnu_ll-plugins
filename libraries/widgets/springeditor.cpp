/****************************************************************************
    
    springeditor.cpp - gtkmm widget for editing spring oscillators
    
    Copyright (C) 2007  Lars Luthman <lars.luthman@gmail.com>
    
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
#include <valarray>

#include <cairomm/cairomm.h>

#include "springeditor.hpp"


using namespace Gtk;
using namespace std;


SpringEditor::SpringEditor()
  : m_projection(2.0 * M_PI / 3.0),
    m_xstart(-0.1),
    m_ystart(0.5),
    m_margin(4),
    m_active_spring(-1),
    m_dragging(false),
    m_dirty(false) {
  
  set_size_request(91, 91);
  
  m_springs.push_back(Spring(0, 2, 2));
  m_springs.push_back(Spring(M_PI/3, 0.5, 0.5));
  m_springs.push_back(Spring(4*M_PI/3, 2, 2));
  
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MOTION_MASK | 
             Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
             Gdk::SCROLL_MASK);
  
  Gdk::Color bg;
  bg.set_rgb(10000, 10000, 15000);
  modify_bg(STATE_NORMAL, bg);
  
  using namespace Menu_Helpers;
  using namespace sigc;
  
  m_menu.items().
    push_back(MenuElem("New spring", 
                       mem_fun(*this, &SpringEditor::new_spring)));
  m_menu.items().
    push_back(MenuElem("Delete spring", 
                       mem_fun(*this, &SpringEditor::delete_spring)));
  m_menu.items().push_back(SeparatorElem());
  m_menu.items().
    push_back(MenuElem("Apply", mem_fun(*this, &SpringEditor::apply)));
}
 

bool SpringEditor::set_string(const std::string& str) {
  return false;
}


std::string SpringEditor::get_string() const {
  return "";
}


bool SpringEditor::on_expose_event(GdkEventExpose* event) {
  
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  Cairo::RefPtr<Cairo::Context> cc(new Cairo::Context(gdk_cairo_create(win->
								       gobj())));
  cc->set_line_join(Cairo::LINE_JOIN_ROUND);
  
  int width = get_width();
  int height = get_height();
  double top = m_margin;
  double h = height - m_margin * 2;
  double w = width - m_margin * 2;
  
  cc->set_source_rgb(0.3, 0.9, 1);
  
  cc->translate(x2p(0), y2p(0));
  cc->scale(w / 2.0 , h / 2.0);
  
  // grid
  cc->set_line_width(0.5 / (w / 2.0));
  cc->arc(0, 0, 1, 0, 2 * M_PI);
  cc->stroke();
  cc->move_to(-1, 0);
  cc->line_to(1, 0);
  cc->stroke();
  cc->move_to(0, -1);
  cc->line_to(0, 1);
  cc->stroke();
  cc->set_line_width(0.2 / (w / 2.0));
  cc->arc(0, 0, 0.5, 0, 2 * M_PI);
  cc->stroke();
  cc->move_to(-sqrt(0.5), -sqrt(0.5));
  cc->line_to(sqrt(0.5), sqrt(0.5));
  cc->stroke();
  cc->move_to(-sqrt(0.5), sqrt(0.5));
  cc->line_to(sqrt(0.5), -sqrt(0.5));
  cc->stroke();
  
  // springs
  cc->set_line_width(2.0 / (w / 2.0));
  cc->set_source_rgb(1, 1, 1);
  for (unsigned i = 0; i < m_springs.size(); ++i) {
    double a = cos(m_springs[i].angle);
    double b = sin(m_springs[i].angle);
    double c = m_xstart;
    double d = m_ystart;
    double l = sqrt(pow(c - a, 2) + pow(d - b, 2));
    double m = 8.0 / (w / 2.0);
    cc->move_to(a, b);
    cc->line_to(a + (c - a) * m / l, b + (d - b) * m / l);
    double orth_x = -(2.0 / (w / 2.0)) * (d - b) / l;
    double orth_y = (2.0 / (w / 2.0)) * (c - a) / l;
    a = a + (c - a) * m / l;
    b = b + (d - b) * m / l;
    c = c - (c - a) * m / l;
    d = d - (d - b) * m / l;
    unsigned coils = (unsigned)(m_springs[i].length * 10);
    double cl = 1.0 / (2 * coils);
    int sign = 1;
    for (unsigned i = 0; i < coils; ++i) {
      cc->line_to(a + (2 * i + 1) * cl * (c - a) + sign * orth_x, 
		  b + (2 * i + 1) * cl * (d - b) + sign * orth_y);
      cc->line_to(a + (2 * i + 2) * cl * (c - a),
		  b + (2 * i + 2) * cl * (d - b));
      sign *= -1;
    }
    cc->line_to(m_xstart, m_ystart);
    cc->stroke();
  }
  for (unsigned i = 0; i < m_springs.size(); ++i) {
    cc->arc(cos(m_springs[i].angle), sin(m_springs[i].angle), 
	    3.0 / (w / 2.0), 0, 2 * M_PI);
    cc->set_line_width(2 / (w / 2.0));
    cc->set_source_rgb(0.30, 0.3, 0.6);
    cc->fill_preserve();
    cc->set_source_rgb(1, 1, 1);
    cc->stroke();
  }
  cc->arc(m_xstart, m_ystart, 3.0 / (w / 2.0), 0, 2 * M_PI);
  cc->set_line_width(2 / (w / 2.0));
  cc->set_source_rgb(0.30, 0.3, 0.6);
  cc->fill_preserve();
  cc->set_source_rgb(1, 1, 1);
  cc->stroke();
  
  return true;
}


bool SpringEditor::on_motion_notify_event(GdkEventMotion* event) {
  return true;
}


bool SpringEditor::on_button_release_event(GdkEventButton* event) {
  return true;
}


bool SpringEditor::on_button_press_event(GdkEventButton* event) {

  // button 3 brings up the menu
  if (event->button == 3) {
    m_click_x = p2x(int(event->x));
    m_click_y = p2y(int(event->y));
    //m_active_point = point;
    m_menu.popup(event->button, event->time);
  }
  
  return true;
}


bool SpringEditor::on_scroll_event(GdkEventScroll* event) {
  return true;
}


void SpringEditor::new_spring() {
  set_dirty();
  queue_draw();
}


void SpringEditor::delete_spring() {
  
}
 

double SpringEditor::x2p(double x) {
  return get_width() / 2.0 + (get_width() / 2.0  - m_margin) * x;
}


double SpringEditor::y2p(double y) {
  return m_margin + (1 - y) * (get_height() / 2.0 - m_margin);
}


double SpringEditor::p2x(int p) {
  return (p - get_width() / 2.0) / (get_width() / 2.0 - m_margin);
}


double SpringEditor::p2y(int p) {
  return 1 - (p - m_margin) / (get_height() / 2.0 - m_margin);
}


void SpringEditor::set_dirty() {
  if (!m_dirty) {
    m_dirty = true;
    Gdk::Color bg;
    bg.set_rgb(20000, 10000, 10000);
    modify_bg(STATE_NORMAL, bg);
  }
}


void SpringEditor::apply() {
  m_dirty = false;
  Gdk::Color bg;
  bg.set_rgb(10000, 10000, 15000);
  modify_bg(STATE_NORMAL, bg);
  signal_apply();
  queue_draw();
}
