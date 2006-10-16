#include <cairomm/cairomm.h>

#include "vgknob.hpp"


VGKnob::VGKnob(float min, float max, float value, 
               float red, float green, float blue) 
  : m_adj(value, min, max),
    m_red(red),
    m_green(green),
    m_blue(blue) {
  
  set_size_request(37, 33);
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MOTION_MASK | 
             Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK);
  m_adj.signal_value_changed().connect(mem_fun(*this, &VGKnob::queue_draw));
}
 
 
bool VGKnob::on_expose_event(GdkEventExpose* event) {
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  
  double angle = M_PI * (0.75 + 1.5 * m_adj.get_value() / 
                         (m_adj.get_upper() - m_adj.get_lower()));
  
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
  
  // circle
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
  
  // thing
  /*
  cc->arc(19, 19, 18, 0.25 * M_PI, 0.75 * M_PI);
  cc->arc_negative(19, 19, 11, 0.75 * M_PI, 0.25 * M_PI);
  cc->close_path();
  cc->set_source_rgb(0.9, 0.9, 0.9);
  cc->fill_preserve();
  cc->set_source_rgb(0, 0, 0);
  cc->stroke();
  */
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
    m_adj.set_value(m_adj.get_value() + 
                    (m_adj.get_upper() - m_adj.get_lower()) / 30);
  else if (event->direction == GDK_SCROLL_DOWN)
    m_adj.set_value(m_adj.get_value() - 
                    (m_adj.get_upper() - m_adj.get_lower()) / 30);
}
