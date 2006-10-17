#include <string>

#include <cairomm/cairomm.h>

#include "vgknob.hpp"


using namespace std;


VGKnob::VGKnob(float min, float max, float value, 
               float red, float green, float blue, bool integer) 
  : m_adj(min, min, max),
    m_red(red),
    m_green(green),
    m_blue(blue),
    m_integer(integer) {
  
  set_size_request(37, 33);
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MOTION_MASK | 
             Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK);
  m_adj.signal_value_changed().connect(mem_fun(*this, &VGKnob::queue_draw));
  if (m_integer)
    m_adj.set_step_increment(1);
  else
    m_adj.set_step_increment((m_adj.get_upper() - m_adj.get_lower()) / 30);
  m_adj.set_value(value);
}


Gtk::Adjustment& VGKnob::get_adjustment() {
  return m_adj;
}
 
 
bool VGKnob::on_expose_event(GdkEventExpose* event) {
  
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  Cairo::RefPtr<Cairo::Context> cc = win->create_cairo_context();
  cc->set_line_join(Cairo::LINE_JOIN_ROUND);
  
  // disc
  cc->arc(19, 19, 16, 0.7 * M_PI, 2.3 * M_PI);
  cc->close_path();
  cc->set_source_rgba(0, 0, 0, 0.3);
  cc->stroke_preserve();
  cc->set_source_rgba(m_red, m_green, m_blue, 0.3);
  cc->fill();

  // display
  cc->move_to(7, 24);
  cc->line_to(7, 13);
  cc->line_to(32, 13);
  cc->line_to(32, 24);
  cc->close_path();
  cc->set_source_rgb(0.35, 0.35, 0.35);
  cc->fill();
  cc->set_line_width(1.0);
  cc->move_to(7, 24);
  cc->line_to(7, 13);
  cc->line_to(32, 13);
  cc->set_source_rgb(0, 0, 0);
  cc->stroke();
  cc->move_to(32, 13);
  cc->line_to(32, 24);
  cc->line_to(7, 24);
  cc->set_source_rgb(0.8, 0.8, 0.8);
  cc->stroke();
  
  // digits
  float value = m_adj.get_value();
  if (m_integer)
    value = floor(value + 0.5);
  string str;
  float offset = 10.5;
  float v = value;
  if (v >= 10000 || v <= -1000) {
    str = "****";
  }
  else {
    if (v < 0) {
      str += '-';
      v *= -1;
    }
    if (v >= 1000)
      str += (int(v) / 1000) + '0';
    if (v >= 100)
      str += ((int(v) % 1000) / 100) + '0';
    if (v >= 10)
      str += ((int(v) % 100) / 10) + '0';
    if (v >= 1 || m_integer)
      str += (int(v) % 10) + '0';
    if (m_integer)
      offset += (5 * (4 - str.length())) / 2;
    else if (str.length() < 4) {
      str += '.';
      offset -= 1;
      while (str.length() < 5) {
        v *= 10;
        str += (int(v) % 10) + '0';
      }
    }
  }
  draw_string(cc, str, offset, 15.5);

  double angle = M_PI * (0.75 + 1.5 * (value - m_adj.get_lower()) / 
                         (m_adj.get_upper() - m_adj.get_lower()));
  
  // circle
  cc->clear_path();
  cc->arc(20, 20, 17, 0.75 * M_PI, angle);
  cc->arc_negative(21, 21, 12, angle, 0.75 * M_PI);
  cc->close_path();
  cc->set_source_rgba(0, 0, 0, 0.3);
  cc->fill();
  
  cc->arc(19, 19, 17, 0.75 * M_PI, angle);
  cc->arc_negative(19, 19, 12, angle, 0.75 * M_PI);
  cc->close_path();
  cc->set_source_rgb(m_red, m_green, m_blue);
  cc->fill_preserve();
  
  // shadow and outline
  cc->clip();
  cc->clear_path();
  
  cc->arc(19, 19, 14, 0, 2 * M_PI);
  cc->set_source_rgba(0, 0, 0, 0.2);
  cc->fill();
  cc->arc(19, 19, 15, 0, 2 * M_PI);
  cc->arc_negative(19, 19, 25, 2 * M_PI, 0);
  cc->fill();
  
  cc->arc(17.5, 17.5, 12, 0, 2 * M_PI);
  cc->set_source_rgba(0, 0, 0, 0.3);
  cc->fill();
  cc->arc(17.5, 17.5, 17, 0, 2 * M_PI);
  cc->arc_negative(17.5, 17.5, 25, 2 * M_PI, 0);
  cc->fill();

  cc->clear_path();
  cc->arc(20.5, 20.5, 12, 0, 2 * M_PI);
  cc->set_source_rgba(1, 1, 1, 0.5);
  cc->fill();
  cc->arc(20.5, 20.5, 17, 0, 2 * M_PI);
  cc->arc_negative(17.5, 17.5, 25, 2 * M_PI, 0);
  cc->fill();
  
  cc->reset_clip();
  cc->arc(19, 19, 17, 0.75 * M_PI, angle);
  cc->arc_negative(19, 19, 12, angle, 0.75 * M_PI);
  cc->close_path();
  cc->set_source_rgb(0, 0, 0);
  cc->stroke();
  
  return true;
}


bool VGKnob::on_motion_notify_event(GdkEventMotion* event) {
  float value = m_value_offset + ((m_click_offset - event->y) / 200.0) * 
    (m_adj.get_upper() - m_adj.get_lower());
  value = value < m_adj.get_lower() ? m_adj.get_lower() : value;
  value = value > m_adj.get_upper() ? m_adj.get_upper() : value;
  m_adj.set_value(value);
  return true;
}


bool VGKnob::on_button_press_event(GdkEventButton* event) {
  m_click_offset = (int)event->y;
  m_value_offset = m_adj.get_value();
  return true;
}


bool VGKnob::on_scroll_event(GdkEventScroll* event) {
  if (event->direction == GDK_SCROLL_UP)
    m_adj.set_value(m_adj.get_value() + m_adj.get_step_increment());
  else if (event->direction == GDK_SCROLL_DOWN)
    m_adj.set_value(m_adj.get_value() - m_adj.get_step_increment());
}


int VGKnob::draw_digit(Cairo::RefPtr<Cairo::Context>& cc, char digit) {
  cc->save();
  cc->set_source_rgb(0.7, 0.9, 1.0);
  cc->set_line_width(1);
  
  int width = 0;
  
  switch (digit) {
  case '0':
    cc->rel_line_to(3, 0);
    cc->rel_line_to(0, 6);
    cc->rel_line_to(-3, 0);
    cc->rel_line_to(0, -6);
    width = 5;
    break;
    
  case '1':
    cc->rel_move_to(3, 0);
    cc->rel_line_to(0, 6);
    width = 5;
    break;
    
  case '2':
    cc->rel_line_to(3, 0);
    cc->rel_line_to(0, 3);
    cc->rel_line_to(-3, 0);
    cc->rel_line_to(0, 3);
    cc->rel_line_to(3, 0);
    width = 5;
    break;
    
  case '3':
    cc->rel_line_to(3, 0);
    cc->rel_line_to(0, 6);
    cc->rel_line_to(-3, 0);
    cc->rel_move_to(0, -3);
    cc->rel_line_to(3, 0);
    width = 5;
    break;
    
  case '4':
    cc->rel_line_to(0, 3);
    cc->rel_line_to(3, 0);
    cc->rel_move_to(0, -3);
    cc->rel_line_to(0, 6);
    width = 5;
    break;
    
  case '5':
    cc->rel_move_to(3, 0);
    cc->rel_line_to(-3, 0);
    cc->rel_line_to(0, 3);
    cc->rel_line_to(3, 0);
    cc->rel_line_to(0, 3);
    cc->rel_line_to(-3, 0);
    width = 5;
    break;
    
  case '6':
    cc->rel_move_to(3, 0);
    cc->rel_line_to(-3, 0);
    cc->rel_line_to(0, 6);
    cc->rel_line_to(3, 0);
    cc->rel_line_to(0, -3);
    cc->rel_line_to(-3, 0);
    width = 5;
    break;
    
  case '7':
    cc->rel_line_to(3, 0);
    cc->rel_line_to(0, 6);
    width = 5;
    break;
    
  case '8':
    cc->rel_line_to(3, 0);
    cc->rel_line_to(0, 6);
    cc->rel_line_to(-3, 0);
    cc->rel_line_to(0, -6);
    cc->rel_move_to(0, 3);
    cc->rel_line_to(3, 0);
    width = 5;
    break;
    
  case '9':
    cc->rel_move_to(0, 6);
    cc->rel_line_to(3, 0);
    cc->rel_line_to(0, -6);
    cc->rel_line_to(-3, 0);
    cc->rel_line_to(0, 3);
    cc->rel_line_to(3, 0);
    width = 5;
    break;
    
  case '.':
    cc->rel_move_to(0, 6);
    cc->rel_line_to(0, -1);
    width = 2;
    break;
    
  case '*':
    cc->rel_move_to(0, 1);
    cc->rel_line_to(3, 4);
    cc->rel_move_to(-3, 0);
    cc->rel_line_to(3, -4);
    cc->rel_move_to(-3, 2);
    cc->rel_line_to(3, 0);
    width = 5;
    break;
    
  case '-':
    cc->rel_move_to(0, 3);
    cc->rel_line_to(3, 0);
    width = 5;
    break;
    
  }
  
  cc->stroke();
  cc->restore();
  
  return width;
}


void VGKnob::draw_string(Cairo::RefPtr<Cairo::Context>& cc, 
                         const std::string& str, float x, float y) {
  cc->move_to(x, y);
  int xoffset = 0;
  for (int i = 0; i < str.length(); ++i) {
    xoffset += draw_digit(cc, str[i]);
    cc->move_to(x + xoffset, y);
  }
}
