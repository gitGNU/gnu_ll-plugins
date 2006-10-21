#include <iostream>
#include <valarray>

#include "shapereditor.hpp"


using namespace Gtk;
using namespace std;


ShaperEditor::ShaperEditor()
  : m_margin(4),
    m_active_point(-1),
    m_dragging(false),
    m_dirty(false) {
  
  set_size_request(91, 91);
  
  m_points.push_back(Point(-1, -1));
  m_points.push_back(Point(1, 1));
  
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MOTION_MASK | 
             Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
             Gdk::SCROLL_MASK);
  
  Gdk::Color bg;
  bg.set_rgb(10000, 10000, 15000);
  modify_bg(STATE_NORMAL, bg);
  
  using namespace Menu_Helpers;
  using namespace sigc;
  
  m_menu.items().
    push_back(MenuElem("New point", 
                       mem_fun(*this, &ShaperEditor::new_point)));
  m_menu.items().
    push_back(MenuElem("Delete point", 
                       mem_fun(*this, &ShaperEditor::delete_point)));
  m_menu.items().push_back(SeparatorElem());
  m_menu.items().
    push_back(MenuElem("Apply", mem_fun(*this, &ShaperEditor::apply)));
}
 

bool ShaperEditor::set_string(const std::string& str) {
  return false;
}


std::string ShaperEditor::get_string() const {
  return "";
}


bool ShaperEditor::on_expose_event(GdkEventExpose* event) {
  
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  Cairo::RefPtr<Cairo::Context> cc = win->create_cairo_context();
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
  
  // curve
  cc->set_line_width(2);
  double xoffset = top;
  for (int i = 0; i < m_points.size() - 1; ++i) {
    cc->move_to(x2p(m_points[i].x), y2p(m_points[i].y));
    cc->line_to(x2p(m_points[i + 1].x), y2p(m_points[i + 1].y));
    cc->set_source_rgb(1, 1, 1);
    cc->stroke();
  }
  
  // points
  xoffset = 4;
  for (int i = 0; i < m_points.size(); ++i) {
    cc->arc(x2p(m_points[i].x), y2p(m_points[i].y), 3, 0, 2 * M_PI);
    cc->set_line_width(2);
    cc->set_source_rgb(0.30, 0.3, 0.6);
    cc->fill_preserve();
    cc->set_source_rgb(1, 1, 1);
    cc->stroke();
  }

  return true;
}


bool ShaperEditor::on_motion_notify_event(GdkEventMotion* event) {
  
  if (m_dragging) {
    
    if (m_active_point > 0 && m_active_point < m_points.size() - 1) {
      double xdiff = (event->x - m_pix_drag_x) / (get_width() / 2.0- m_margin);
      m_points[m_active_point].x = m_drag_x + xdiff;
      if (m_points[m_active_point].x < m_points[m_active_point - 1].x)
        m_points[m_active_point].x = m_points[m_active_point - 1].x;
      if (m_points[m_active_point].x > m_points[m_active_point + 1].x)
        m_points[m_active_point].x = m_points[m_active_point + 1].x;
    }
    
    double ydiff = (m_pix_drag_y - event->y) / (get_height()/2.0 - m_margin);
    m_points[m_active_point].y = m_drag_y + ydiff;
    if (m_points[m_active_point].y > 1)
      m_points[m_active_point].y = 1;
    if (m_points[m_active_point].y < -1)
      m_points[m_active_point].y = -1;
    
    set_dirty();
    queue_draw();
  }
  return true;
}


bool ShaperEditor::on_button_release_event(GdkEventButton* event) {
  if (event->button == 1)
    m_dragging = false;
}


bool ShaperEditor::on_button_press_event(GdkEventButton* event) {

  int point;
  for (point = 0; point < m_points.size(); ++point) {
    if (pow(event->x - x2p(m_points[point].x), 2) + 
        pow(event->y - y2p(m_points[point].y), 2) < 25)
      break;
  }
  
  // button 1 moves the point
  if (event->button == 1 && point < m_points.size()) {
    m_active_point = point;
    m_dragging = true;
    m_pix_drag_x = int(event->x);
    m_pix_drag_y = int(event->y);
    m_drag_x = m_points[point].x;
    m_drag_y = m_points[point].y;
  }
  
  // button 3 brings up the menu
  else if (event->button == 3) {
    m_click_x = p2x(int(event->x));
    m_click_y = p2y(int(event->y));
    m_active_point = point;
    m_menu.popup(event->button, event->time);
  }
  
  return true;
}


bool ShaperEditor::on_scroll_event(GdkEventScroll* event) {
  
  /*
  double event->x = event->x + m_adj.get_value();
  
  // shift is not pressed, zoom
  if (!(event->state & GDK_SHIFT_MASK)) {
    double x = p2x(int(event->x));
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
  
  // find the active point
  int point;
  double xoffset = 0;
  for (point = 0; point < m_points.size(); ++point) {
    xoffset += (get_width() - 2 * m_margin) * m_points[point].length;
    if (event->x < xoffset)
      break;
  }
  if (point >= m_points.size() || point == 0)
    return true;
  
  // shift is pressed, change the sustain level sensitivity
  if (event->direction == GDK_SCROLL_UP && event->state & GDK_SHIFT_MASK) {
    m_points[point].sustain_sens += 0.03;
    if (m_points[point].sustain_sens > 1)
      m_points[point].sustain_sens = 1;
  }
  
  else if (event->direction == GDK_SCROLL_DOWN && 
           event->state & GDK_SHIFT_MASK) {
    m_points[point].sustain_sens -= 0.03;
    if (m_points[point].sustain_sens < 0)
      m_points[point].sustain_sens = 0;
  }
  
  // sync the loop boundaries
  if (point == m_loop_start && m_loop_start < m_loop_end) {
    if (m_loop_end < m_points.size())
      m_points[m_loop_end].sustain_sens = m_points[point].sustain_sens;
    else
      m_points[point].sustain_sens = 0;
  }
  else if (point == m_loop_end && m_loop_start < m_loop_end) {
    if (m_loop_start > 0)
      m_points[m_loop_start].sustain_sens = 
        m_points[point].sustain_sens;
    else
      m_points[point].sustain_sens = 0;
  }
  
  set_dirty();
  queue_draw();
  */
  
  return true;
}


void ShaperEditor::new_point() {
  if (m_click_x < -1) m_click_x = -1;
  if (m_click_x > 1) m_click_x = 1;
  if (m_click_y < -1) m_click_y = -1;
  if (m_click_y > 1) m_click_y = 1;
  for (int i = 0; i < m_points.size() - 1; ++i) {
    if (m_click_x <= m_points[i + 1].x) {
      Point s(m_click_x, m_click_y);
      m_points.insert(m_points.begin() + i + 1, s);
      break;
    }
  }
  set_dirty();
  queue_draw();
}


void ShaperEditor::delete_point() {
  if (m_active_point > 0 && m_active_point < m_points.size() - 1) {
    m_points.erase(m_points.begin() + m_active_point);
    set_dirty();
    queue_draw();
  }
}
 

int ShaperEditor::x2p(double x) {
  return int(get_width() / 2.0 + (get_width() / 2.0  - m_margin) * x);
}


int ShaperEditor::y2p(double y) {
  return int(m_margin + (1 - y) * (get_height() / 2.0 - m_margin));
}


double ShaperEditor::p2x(int p) {
  return (p - get_width() / 2.0) / (get_width() / 2.0 - m_margin);
}


double ShaperEditor::p2y(int p) {
  return 1 - (p - m_margin) / (get_height() / 2.0 - m_margin);
}


void ShaperEditor::set_dirty() {
  if (!m_dirty) {
    m_dirty = true;
    Gdk::Color bg;
    bg.set_rgb(20000, 10000, 10000);
    modify_bg(STATE_NORMAL, bg);
  }
}


void ShaperEditor::apply() {
  m_dirty = false;
  Gdk::Color bg;
  bg.set_rgb(10000, 10000, 15000);
  modify_bg(STATE_NORMAL, bg);
  queue_draw();
}
