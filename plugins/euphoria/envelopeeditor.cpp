#include <valarray>

#include "envelopeeditor.hpp"


using namespace Gtk;
using namespace std;


EnvelopeEditor::EnvelopeEditor()
  : m_loop_start(-1),
    m_loop_end(-1),
    m_ruler_height(8.5),
    m_xscale(50) {
  
  set_size_request(200, 90);
  
  m_segments.push_back(Segment(0, 1, 0, Attack));
  m_segments.push_back(Segment(1, 1, 0, Decay, Exponential));
  m_segments.push_back(Segment(0.3, 0.7, 0));
  m_segments.push_back(Segment(0.3, 0.5, 0, Decay, Exponential2));
  m_segments.push_back(Segment(0.6, 0.5, 1, Attack, Exponential));
  m_segments.push_back(Segment(0.3, 0.7, 0));
  m_segments.push_back(Segment(0.3, 1, 1, Release, Exponential));
  
  m_loop_start = 2;
  m_loop_end = 6;
  
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MOTION_MASK | 
             Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK);
  
  Gdk::Color bg;
  bg.set_rgb(10000, 10000, 15000);
  modify_bg(STATE_NORMAL, bg);
}
 

bool EnvelopeEditor::set_string(const std::string& str) {
  return false;
}


std::string EnvelopeEditor::get_string() const {
  return "";
}
  

bool EnvelopeEditor::on_expose_event(GdkEventExpose* event) {
  
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  Cairo::RefPtr<Cairo::Context> cc = win->create_cairo_context();
  cc->set_line_join(Cairo::LINE_JOIN_ROUND);
  
  int width = get_width();
  int height = get_height();
  double top = 4;
  double h = height - m_ruler_height - top - top;
  
  cc->set_source_rgb(0.3, 0.9, 1);
  
  // grid
  cc->move_to(0, h + top + top);
  cc->line_to(width, h + top + top);
  for (int i = 0; i * m_xscale + top < width; ++i) {
    cc->move_to(top + m_xscale * i, h + top + top);
    cc->line_to(top + m_xscale * i, h + top + top + 3);
  }
  cc->set_line_width(1.0);
  cc->stroke();
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
  
  // curve
  cc->set_line_width(2);
  double xoffset = top;
  for (int i = 0; i < m_segments.size(); ++i) {
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
    if (i >= m_loop_start && i < m_loop_end) {
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
  for (int i = 0; i < m_segments.size(); ++i) {

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
    cc->set_source_rgb(1, 1, 1);
    cc->stroke();
  }
  cc->arc(xoffset, h * (1 - m_segments[0].start) + top, 3, 0, 2 * M_PI);
  cc->set_source_rgb(0.30, 0.30, 0.60);
  cc->fill_preserve();
  cc->set_source_rgb(1, 1, 1);
  cc->stroke();

  return true;
}


bool EnvelopeEditor::on_motion_notify_event(GdkEventMotion* event) {
  return true;
}


bool EnvelopeEditor::on_button_press_event(GdkEventButton* event) {

  int segment;
  double xoffset = 0;
  for (segment = 0; segment < m_segments.size(); ++segment) {
    xoffset += m_xscale * m_segments[segment].length;
    if (event->x < xoffset)
      break;
  }
  
  if (event->button == 1) {
    if (event->state & GDK_SHIFT_MASK && segment < m_segments.size()) {
      m_segments[segment].type = 
        SegmentType((m_segments[segment].type + 1) % NumTypes);
      queue_draw();
    }
  }
  
  return true;
}


bool EnvelopeEditor::on_scroll_event(GdkEventScroll* event) {
  
  if (!(event->state & GDK_SHIFT_MASK)) {
    if (event->direction == GDK_SCROLL_DOWN) {
      m_xscale *= 1.1111111;
      if (m_xscale >= 10000)
        m_xscale = 10000;
    }
    else if (event->direction == GDK_SCROLL_UP) {
      m_xscale *= 0.9;
      if (m_xscale < 10)
        m_xscale = 10;
    }
    queue_draw();
    return true;
  }
  
  int segment;
  double xoffset = 0;
  for (segment = 0; segment < m_segments.size(); ++segment) {
    xoffset += m_xscale * m_segments[segment].length;
    if (event->x < xoffset)
      break;
  }
  
  if (segment >= m_segments.size())
    return true;
  
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
  
  queue_draw();
  
  return true;
}


