/****************************************************************************
    
    envelopeeditor.cpp - gtkmm widget for editing envelopes
    
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
#include <sstream>
#include <valarray>

#include <cairomm/cairomm.h>

#include "envelopeeditor.hpp"


using namespace Gtk;
using namespace std;


EnvelopeEditor::EnvelopeEditor()
  : m_loop_start(-1),
    m_loop_end(-1),
    m_margin(4),
    m_ruler_height(9.5),
    m_xscale(50),
    m_active_segment(-1),
    m_dragging(false),
    m_dirty(false),
    m_adj(0, 0, 200, 1, 10, 20) {
  
  set_size_request(200, 91);
  
  m_segments.push_back(Segment(0, 1, 0, Attack));
  m_segments.push_back(Segment(1, 1, 0, Decay, Exponential));
  m_segments.push_back(Segment(0.5, 1, 1));
  m_segments.push_back(Segment(0.5, 1, 1, Release, Exponential));
  
  m_loop_start = 2;
  m_loop_end = 3;
  
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MOTION_MASK | 
             Gdk::BUTTON2_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | 
             Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK);
  
  Gdk::Color bg;
  bg.set_rgb(10000, 10000, 15000);
  modify_bg(STATE_NORMAL, bg);
  
  using namespace Menu_Helpers;
  using namespace sigc;
  
  m_menu.items().
    push_back(MenuElem("New segment", 
                       mem_fun(*this, &EnvelopeEditor::new_segment)));
  m_menu.items().
    push_back(MenuElem("Delete segment", 
                       mem_fun(*this, &EnvelopeEditor::delete_segment)));
  m_menu.items().push_back(SeparatorElem());
  m_menu.items().
    push_back(MenuElem("Set loop start", 
                       mem_fun(*this, &EnvelopeEditor::set_loop_start)));
  m_menu.items().
    push_back(MenuElem("Set loop end", 
                       mem_fun(*this, &EnvelopeEditor::set_loop_end)));
  m_menu.items().
    push_back(MenuElem("Clear loop", 
                       mem_fun(*this, &EnvelopeEditor::clear_loop)));
  m_menu.items().push_back(SeparatorElem());
  
  Menu* lmod_menu = manage(new Menu);
  slot<void, SegmentType> set_lmod = 
    mem_fun(*this, &EnvelopeEditor::set_length_modulator);
  lmod_menu->items().push_back(MenuElem("Attack", bind(set_lmod, Attack)));
  lmod_menu->items().push_back(MenuElem("Decay", bind(set_lmod, Decay)));
  lmod_menu->items().push_back(MenuElem("Release", bind(set_lmod, Release)));
  lmod_menu->items().push_back(MenuElem("No modulation", 
                                        bind(set_lmod, Constant)));
  m_menu.items().push_back(MenuElem("Length modulator", *lmod_menu));

  Menu* curve_menu = manage(new Menu);
  slot <void, CurveType> set_curve = 
    mem_fun(*this, &EnvelopeEditor::set_curve_type);
  curve_menu->items().push_back(MenuElem("Linear", bind(set_curve, Linear)));
  curve_menu->items().push_back(MenuElem("Exponential", 
                                         bind(set_curve, Exponential)));
  curve_menu->items().push_back(MenuElem("Inverted exponential", 
                                         bind(set_curve, Exponential2)));
  m_menu.items().push_back(MenuElem("Curve type", *curve_menu));
  
  m_menu.items().push_back(SeparatorElem());
  m_menu.items().
    push_back(MenuElem("Apply", mem_fun(*this, &EnvelopeEditor::apply)));
  
  m_adj.signal_value_changed().
    connect(mem_fun(*this, &EnvelopeEditor::queue_draw));
}
 

bool EnvelopeEditor::set_string(const std::string& str) {
  istringstream iss(str);
  
  int loop_start, loop_end;
  iss>>loop_start>>loop_end;
  
  vector<Segment> new_segments;
  while (iss.good()) {
    Segment s;
    iss>>s.start>>s.length>>s.sustain_sens;
    string tmp;
    iss>>tmp;
    if (tmp == "c")
      s.type = Constant;
    else if (tmp == "a")
      s.type = Attack;
    else if (tmp == "d")
      s.type = Decay;
    else if (tmp == "r")
      s.type = Release;
    iss>>tmp;
    if (tmp == "l")
      s.curve = Linear;
    else if (tmp == "e")
      s.curve = Exponential;
    else if (tmp == "e2")
      s.curve = Exponential2;
    new_segments.push_back(s);
  }
  
  if (new_segments.size() == 0 || new_segments[0].start != 0 ||
      (loop_start != -1 && loop_start >= (int)new_segments.size()) ||
      (loop_end != -1 && loop_end > (int)new_segments.size()))
    return false;
  
  m_segments = new_segments;
  m_loop_start = loop_start;
  m_loop_end = loop_end;
  queue_draw();
  
  return true;
}


std::string EnvelopeEditor::get_string() const {
  ostringstream oss;
  
  if (m_loop_start < m_loop_end && m_loop_start != -1)
    oss<<m_loop_start<<" "<<m_loop_end<<" ";
  else
    oss<<"-1 -1 ";
  
  for (unsigned i = 0; i < m_segments.size(); ++i) {
    const Segment& s = m_segments[i];
    oss<<s.start<<" "<<s.length<<" "<<s.sustain_sens<<" ";
    if (s.type == Constant)
      oss<<"c ";
    else if (s.type == Attack)
      oss<<"a ";
    else if (s.type == Decay)
      oss<<"d ";
    else if (s.type == Release)
      oss<<"r ";
    if (s.curve == Linear)
      oss<<"l";
    else if (s.curve == Exponential)
      oss<<"e";
    else if (s.curve == Exponential2)
      oss<<"e2";
    if (i < m_segments.size() - 1)
      oss<<" ";
  }
  return oss.str();
}


Adjustment& EnvelopeEditor::get_adjustment() {
  return m_adj;
}
  

void EnvelopeEditor::on_size_allocate(Gtk::Allocation& a) {
  DrawingArea::on_size_allocate(a);
  m_adj.set_page_size(a.get_width());
  update_adjustment();
}


bool EnvelopeEditor::on_expose_event(GdkEventExpose* event) {
  
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  Cairo::RefPtr<Cairo::Context> cc(new Cairo::Context(gdk_cairo_create(win->gobj())));
  cc->set_line_join(Cairo::LINE_JOIN_ROUND);
  cc->translate(-m_adj.get_value(), 0);
  
  //int width = get_width();
  int height = get_height();
  double top = m_margin;
  double h = height - m_ruler_height - top - top;
  
  cc->set_source_rgb(0.3, 0.9, 1);
  
  // grid
  cc->move_to(0, h + top + top);
  cc->line_to(m_adj.get_upper(), h + top + top);
  for (int i = 0; i * m_xscale + top < m_adj.get_upper(); ++i) {
    cc->move_to(top + m_xscale * i, h + top + top);
    cc->line_to(top + m_xscale * i, h + top + top + 3);
  }
  cc->set_line_width(1.0);
  cc->stroke();
  cc->move_to(0, h / 2 + top);
  cc->line_to(m_adj.get_upper(), h / 2 + top);
  cc->set_line_width(0.5);
  cc->stroke();
  cc->move_to(0, h / 4 + top);
  cc->line_to(m_adj.get_upper(), h / 4 + top);
  cc->move_to(0, 3 * h / 4 + top);
  cc->line_to(m_adj.get_upper(), 3 * h / 4 + top);
  cc->set_line_width(0.2);
  cc->stroke();
  
  // curve
  cc->set_line_width(2);
  double xoffset = top;
  for (unsigned i = 0; i < m_segments.size(); ++i) {
    cc->move_to(xoffset, h * (1 - m_segments[i].start) + top);
    double l = m_xscale * m_segments[i].length;

    if (m_segments[i].curve == Linear) {
      cc->line_to(xoffset + l,
                  h * (1 - m_segments[(i+1) % m_segments.size()].start) + top);
    }
    
    else if (m_segments[i].curve == Exponential) {
      double end = m_segments[(i + 1) % m_segments.size()].start;
      double start = m_segments[i].start;
      double min = end < start ? end : start;
      double max = end > start ? end : start;
      double v = start < end ? 0.001 : 1;
      int sections = 20;
      double f = pow(start < end ? 1000 : 0.001, 1.0 / sections);
      for (int i = 0; i < sections; ++i) {
        v *= f;
        cc->line_to(xoffset + (i + 1) * l / sections, 
                    h * (1 - (min + (v - 0.001) / 0.999 * (max - min))) + top);
      }
    }

    else if (m_segments[i].curve == Exponential2) {
      double end = m_segments[(i + 1) % m_segments.size()].start;
      double start = m_segments[i].start;
      double min = end < start ? end : start;
      double max = end > start ? end : start;
      double v = start > end ? 0.001 : 1;
      int sections = 20;
      double f = pow(start > end ? 1000 : 0.001, 1.0 / sections);
      for (int i = 0; i < sections; ++i) {
        v *= f;
        cc->line_to(xoffset + (i + 1) * l / sections, 
                    h * (1 - (min + ((1 - v) - 0.001) / 0.999 * (max - min))) +
                    top);
      }
    }

    if (m_segments[i].type == Attack)
      cc->set_source_rgb(1.0, 0, 0);
    else if (m_segments[i].type == Decay)
      cc->set_source_rgb(1, 1, 0);
    else if (m_segments[i].type == Release)
      cc->set_source_rgb(0, 0.3, 1);
    else
      cc->set_source_rgb(1, 1, 1);
    cc->stroke_preserve();
    if ((int)i >= m_loop_start && (int)i < m_loop_end) {
      cc->save();
      cc->set_source_rgb(0, 0.7, 0);
      valarray<double> dashes(1);
      dashes[0] = 7;
      cc->set_dash(dashes, 0);
      cc->stroke();
      cc->restore();
    }
    cc->clear_path();
    

    xoffset += l;
  }
  
  // points and sustain bars
  xoffset = 4;
  for (unsigned i = 0; i < m_segments.size(); ++i) {

    if (m_segments[i].sustain_sens > 0) {
      double min = (1 - m_segments[i].sustain_sens) * m_segments[i].start;
      double max = m_segments[i].sustain_sens + min;
      cc->move_to(xoffset - 3, h * (1 - max) + top);
      cc->line_to(xoffset + 3, h * (1 - max) + top);
      cc->move_to(xoffset - 3, h * (1 - min) + top);
      cc->line_to(xoffset + 3, h * (1 - min) + top);
      cc->move_to(xoffset, h * (1 - max) + top);
      cc->line_to(xoffset, h * (1 - min) + top);
      cc->set_line_width(0.5);
      cc->set_source_rgb(0.3, 1, 0.3);
      cc->stroke();
    }
    
    double y = h * (1 - m_segments[i].start) + top;
    cc->arc(xoffset, y, 3, 0, 2 * M_PI);
    xoffset += m_xscale * m_segments[i].length;
    cc->set_line_width(2);
    if (m_segments[i].sustain_sens > 0)
      cc->set_source_rgb(0.30, 0.9, 0.4);
    else
      cc->set_source_rgb(0.30, 0.3, 0.6);
    cc->fill_preserve();
    if ((int)i == m_loop_start)
      cc->set_source_rgb(0, 0.7, 0);
    else if ((int)i == m_loop_end)
      cc->set_source_rgb(1, 0.5, 0);
    else
      cc->set_source_rgb(1, 1, 1);
    cc->stroke();
  }
  cc->arc(xoffset, h * (1 - m_segments[0].start) + top, 3, 0, 2 * M_PI);
  cc->set_source_rgb(0.30, 0.30, 0.60);
  cc->fill_preserve();
  if ((int)m_segments.size() == m_loop_end)
    cc->set_source_rgb(1, 0.5, 0);
  else
    cc->set_source_rgb(1, 1, 1);
  cc->stroke();

  return true;
}


bool EnvelopeEditor::on_motion_notify_event(GdkEventMotion* event) {
  
  double eventx = event->x + m_adj.get_value();
  
  if (m_dragging && m_active_segment > 0) {

    double xdiff = (eventx - m_pix_drag_x) / m_xscale;
    double ydiff = (m_pix_drag_y - event->y) / 
      (get_height() - 2 * m_margin - m_ruler_height);
    //double total;
    
    if (xdiff < -m_drag_length + 0.001)
      xdiff = -m_drag_length + 0.001;
    
    if (m_active_segment < (int)m_segments.size()) {
      double total = m_segments[m_active_segment - 1].length + 
        m_segments[m_active_segment].length;
      
      if (xdiff > total - m_drag_length - 0.001 && !m_move)
        xdiff = total - m_drag_length - 0.001;
      
      if (!m_move)
        m_segments[m_active_segment].length = (total - m_drag_length) - xdiff;
      if ((m_active_segment == m_loop_start && 
           m_loop_end == (int)m_segments.size() ||
           (m_active_segment == m_loop_end && m_loop_start == 0)))
        ydiff = 0;
      m_segments[m_active_segment].start = m_drag_y + ydiff;
      if (m_segments[m_active_segment].start > 1)
        m_segments[m_active_segment].start = 1;
      if (m_segments[m_active_segment].start < 0)
        m_segments[m_active_segment].start = 0;
      if (m_active_segment == m_loop_start && m_loop_start < m_loop_end &&
          m_loop_end < (int)m_segments.size())
        m_segments[m_loop_end].start = m_segments[m_active_segment].start;
      if (m_active_segment == m_loop_end && m_loop_start < m_loop_end)
        m_segments[m_loop_start].start = m_segments[m_active_segment].start;
    }
    m_segments[m_active_segment - 1].length = m_drag_length + xdiff;
    
    update_adjustment();
    set_dirty();
    queue_draw();
  }
  return true;
}


bool EnvelopeEditor::on_button_release_event(GdkEventButton* event) {
  if (event->button == 1 || event->button == 2)
    m_dragging = false;
  return true;
}


bool EnvelopeEditor::on_button_press_event(GdkEventButton* event) {

  double eventx = event->x + m_adj.get_value();

  unsigned segment;
  double xoffset = m_margin;
  for (segment = 0; segment < m_segments.size(); ++segment) {
    xoffset += m_xscale * m_segments[segment].length;
    if (eventx < xoffset)
      break;
  }
  
  // button 1 or 2 moves the point (2 without changing the length of the next
  // segment)
  if (event->button == 1 || event->button == 2) {
    if (event->state & GDK_SHIFT_MASK && segment < m_segments.size()) {
      m_segments[segment].type = 
        SegmentType((m_segments[segment].type + 1) % NumTypes);
      set_dirty();
      queue_draw();
    }
    if (event->state & GDK_CONTROL_MASK && segment < m_segments.size()) {
      m_segments[segment].curve =
        CurveType((m_segments[segment].curve + 1) % NumCurves);
      set_dirty();
      queue_draw();
    }
    else {
      xoffset = m_margin;
      for (segment = 0; segment < m_segments.size() + 1; ++segment) {
        if (segment > 0 && pow(eventx - xoffset, 2) + 
            pow(event->y - y2p(m_segments[segment % m_segments.size()].start), 
                2) < 25) {
          m_active_segment = segment;
          m_dragging = true;
          m_pix_drag_x = int(eventx);
          m_pix_drag_y = int(event->y);
          m_drag_length = m_segments[segment - 1].length;
          m_drag_y = m_segments[segment].start;
          m_move = (event->button == 2);
          break;
        }
        xoffset += m_segments[segment % m_segments.size()].length * m_xscale;
      }
    }
  }
  
  // button 3 brings up the menu
  else if (event->button == 3 && segment < m_segments.size()) {
    m_click_x = (eventx - xoffset) / m_xscale + m_segments[segment].length;
    m_click_y = 1 - (event->y - m_margin) / 
      (get_height() - 2 * m_margin - m_ruler_height);
    m_active_segment = segment;
    m_menu.popup(event->button, event->time);
  }
  
  return true;
}


bool EnvelopeEditor::on_scroll_event(GdkEventScroll* event) {
  
  double eventx = event->x + m_adj.get_value();
  
  // shift is not pressed, zoom
  if (!(event->state & GDK_SHIFT_MASK)) {
    double x = p2x(int(eventx));
    if (event->direction == GDK_SCROLL_UP) {
      m_xscale *= 1.1111111;
      if (m_xscale >= 10000)
        m_xscale = 10000;
    }
    else if (event->direction == GDK_SCROLL_DOWN) {
      m_xscale *= 0.9;
      if (m_xscale < 10)
        m_xscale = 10;
    }
    update_adjustment();
    x = x2p(x) - event->x;
    x = x > 0 ? x : 0;
    double tmp = m_adj.get_upper() - m_adj.get_page_size();
    x = x < tmp ? x : tmp;
    m_adj.set_value(x > 0 ? x : 0);
    queue_draw();
    return true;
  }
  
  // find the active segment
  unsigned segment;
  double xoffset = 0;
  for (segment = 0; segment < m_segments.size(); ++segment) {
    xoffset += m_xscale * m_segments[segment].length;
    if (eventx < xoffset)
      break;
  }
  if (segment >= m_segments.size() || segment == 0)
    return true;
  
  // shift is pressed, change the sustain level sensitivity
  if (event->direction == GDK_SCROLL_UP && event->state & GDK_SHIFT_MASK) {
    m_segments[segment].sustain_sens += 0.03;
    if (m_segments[segment].sustain_sens > 1)
      m_segments[segment].sustain_sens = 1;
  }
  
  else if (event->direction == GDK_SCROLL_DOWN && 
           event->state & GDK_SHIFT_MASK) {
    m_segments[segment].sustain_sens -= 0.03;
    if (m_segments[segment].sustain_sens < 0)
      m_segments[segment].sustain_sens = 0;
  }
  
  // sync the loop boundaries
  if ((int)segment == m_loop_start && m_loop_start < m_loop_end) {
    if (m_loop_end < (int)m_segments.size())
      m_segments[m_loop_end].sustain_sens = m_segments[segment].sustain_sens;
    else
      m_segments[segment].sustain_sens = 0;
  }
  else if ((int)segment == m_loop_end && m_loop_start < m_loop_end) {
    if (m_loop_start > 0)
      m_segments[m_loop_start].sustain_sens = 
        m_segments[segment].sustain_sens;
    else
      m_segments[segment].sustain_sens = 0;
  }
  
  set_dirty();
  queue_draw();
  
  return true;
}


void EnvelopeEditor::update_adjustment() {
  double length = m_margin * 2 + 20;
  for (unsigned i = 0; i < m_segments.size(); ++i)
    length += m_segments[i].length * m_xscale;
  m_adj.set_upper(ceil(length) > get_width() ? ceil(length) : get_width());
  if (m_adj.get_value() + m_adj.get_page_size() > m_adj.get_upper())
    m_adj.set_value(m_adj.get_upper() - m_adj.get_page_size());
}


void EnvelopeEditor::new_segment() {
  double length = m_segments[m_active_segment].length - m_click_x;
  Segment s(m_click_y, length, 0, m_segments[m_active_segment].type, 
            m_segments[m_active_segment].curve);
  m_segments[m_active_segment].length -= length;
  m_segments.insert(m_segments.begin() + m_active_segment + 1, s);
  if (m_loop_start > m_active_segment)
    ++m_loop_start;
  if (m_loop_end > m_active_segment)
    ++m_loop_end;
  update_adjustment();
  set_dirty();
  queue_draw();
}


void EnvelopeEditor::delete_segment() {
  if (m_active_segment > 0) {
    m_segments.erase(m_segments.begin() + m_active_segment);
    if (m_loop_start > m_active_segment)
      --m_loop_start;
    if (m_loop_end > m_active_segment)
      --m_loop_end;
    update_adjustment();
    set_dirty();
    queue_draw();
  }
}
 

int EnvelopeEditor::x2p(double x) {
  return int(m_margin + m_xscale * x);
}


int EnvelopeEditor::y2p(double y) {
  return int(m_margin + (1 - y) * 
             (get_height() - 2 * m_margin - m_ruler_height));
}


double EnvelopeEditor::p2x(int p) {
  return (p - m_margin) / m_xscale;
}


double EnvelopeEditor::p2y(int p) {
  return 1 - (p - m_margin) / (get_height() - 2 * m_margin - m_ruler_height);
}


void EnvelopeEditor::set_loop_start() {
  m_loop_start = m_active_segment;
  if (m_loop_end > 0 && m_loop_start < m_loop_end) {
    if (m_loop_end < (int)m_segments.size()) {
      m_segments[m_loop_end].start = m_segments[m_loop_start].start;
      m_segments[m_loop_end].sustain_sens = 
        m_segments[m_loop_start].sustain_sens;
    }
    else {
      m_segments[m_loop_start].start = 0;
      m_segments[m_loop_start].sustain_sens = 0;
    }
  }
  set_dirty();
  queue_draw();
}


void EnvelopeEditor::set_loop_end() {
  m_loop_end = m_active_segment + 1;
  if (m_loop_start >= 0 && m_loop_start < m_loop_end) {
    if (m_loop_end < (int)m_segments.size()) {
      m_segments[m_loop_end].start = m_segments[m_loop_start].start;
      m_segments[m_loop_end].sustain_sens = 
        m_segments[m_loop_start].sustain_sens;
    }
    else {
      m_segments[m_loop_start].start = 0;
      m_segments[m_loop_start].sustain_sens = 0;
    }
  }
  set_dirty();
  queue_draw();
}


void EnvelopeEditor::clear_loop() {
  m_loop_end = m_loop_start = -1;
  set_dirty();
  queue_draw();
}


void EnvelopeEditor::set_length_modulator(SegmentType type) {
  m_segments[m_active_segment].type = type;
  set_dirty();
  queue_draw();
}


void EnvelopeEditor::set_curve_type(CurveType curve) {
  m_segments[m_active_segment].curve = curve;
  set_dirty();
  queue_draw();
}


void EnvelopeEditor::set_dirty() {
  if (!m_dirty) {
    m_dirty = true;
    Gdk::Color bg;
    bg.set_rgb(20000, 10000, 10000);
    modify_bg(STATE_NORMAL, bg);
  }
}


void EnvelopeEditor::apply() {
  m_dirty = false;
  Gdk::Color bg;
  bg.set_rgb(10000, 10000, 15000);
  modify_bg(STATE_NORMAL, bg);
  signal_apply();
  queue_draw();
}
