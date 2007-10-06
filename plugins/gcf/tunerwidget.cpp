#include <iostream>
#include <cstdio>
#include <sstream>

#include "tunerwidget.hpp"


using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace std;


TunerWidget::TunerWidget(const string& bundle) 
  : m_freq(0.0),
    m_threshold(1.0 / 50) {
  
  m_string_freqs[0] = 82.4068892282;
  m_string_freqs[1] = 110.0000000000;
  m_string_freqs[2] = 146.8323839587;
  m_string_freqs[3] = 195.9977179909;
  m_string_freqs[4] = 246.9416506281;
  m_string_freqs[5] = 329.6275569129;
  
  m_freq = 82.4;
  
  set_size_request(100, 75);
  add_events(SCROLL_MASK | BUTTON_PRESS_MASK);
  m_bg.set_rgb(10000, 10000, 15000);
  RefPtr<Colormap> cmap = Colormap::get_system();
  cmap->alloc_color(m_bg);
  
  /*
  // compute the text size
  RefPtr<Pango::Layout> l = Pango::Layout::create(get_pango_context());
  l->set_text(m_control_names[Resonance]);
  Pango::FontDescription fd;
  fd.set_family("sans");
  fd.set_absolute_size(PANGO_SCALE * 100);
  l->set_font_description(fd);
  Pango::Rectangle r = l->get_logical_extents();
  int wanted_width = 48;
  m_text_size = PANGO_SCALE * 100 / 
    (r.get_width() / (wanted_width * PANGO_SCALE));
  */
}


TunerWidget::~TunerWidget() {

}


void TunerWidget::set_frequency(float freq) {
  m_freq = freq;
  queue_draw();
}


void TunerWidget::on_realize() {
  DrawingArea::on_realize();
}


bool TunerWidget::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  RefPtr<GC> gc = GC::create(win);
  int h = get_height();
  int w = get_width();
  gc->set_foreground(m_bg);
  win->draw_rectangle(gc, true, 0, 0, w, h);
  ::Cairo::RefPtr< ::Cairo::Context > cc = win->create_cairo_context();
  cc->set_line_join(::Cairo::LINE_JOIN_ROUND);
  cc->set_line_width(2.0);
  
  // find in which interval we are
  bool hit = false;
  unsigned str;
  for (str = 0; str < NUM_STRINGS; ++str) {
    if (m_freq < m_string_freqs[str] * (1 - m_threshold))
      break;
    else if (m_freq < m_string_freqs[str] * (1 + m_threshold)) {
      hit = true;
      break;
    }
    else if (str == NUM_STRINGS - 1 || m_freq < m_string_freqs[str] * 
	     sqrt(m_string_freqs[str + 1] / m_string_freqs[str]))
      break;
  }
  
  float w2 = w * 0.75;
  
  // draw red or green rects
  float yppad = (str == 0 ? 10 : 0);
  float ynpad = (str == NUM_STRINGS - 1 ? -10 : 0);
  cc->move_to(-10, h * (1 - (str) * 1 / 7.0) + yppad);
  cc->line_to(w2, h * (1 - (str) * 1 / 7.0) + yppad);
  cc->line_to(w * 7.0 / 8, h + yppad);
  cc->line_to(w + 10, h + 200);
  cc->line_to(w + 10, 0 - 200);
  cc->line_to(w * 7.0 / 8, 0 + ynpad);
  cc->line_to(w2, h * (1 - (str + 2) * 1 / 7.0) + ynpad);
  cc->line_to(-10, h * (1 - (str + 2) * 1 / 7.0) + ynpad);
  if (hit)
    cc->set_source_rgba(0, 0.5, 0, 1);
  else
    cc->set_source_rgba(0.5, 0, 0, 1);
  cc->fill_preserve();
  cc->set_source_rgba(1, 1, 1, 1);
  cc->stroke();
  cc->move_to(w2, h * (1 - (str + 1) * 1 / 7.0));
  cc->line_to(w * 7.0 / 8, h / 2.0);
  cc->line_to(w, h / 2.0);
  cc->stroke();
  
  // draw strings
  cc->set_source_rgba(1, 1, 1, 1);
  for (unsigned i = 0; i < NUM_STRINGS; ++i) {
    cc->move_to(0, h * (1 - (i + 1) * 1 / 7.0));
    cc->line_to(w2, h * (1 - (i + 1) * 1 / 7.0));
    cc->stroke();
  }
  
  // draw frequency marker
  float y;
  float f1 = m_string_freqs[str];
  float f0 = (str > 0 ? m_string_freqs[str - 1] : 
	      f1 * f1 / m_string_freqs[str + 1]);
  float f2 = (str < NUM_STRINGS - 1 ? m_string_freqs[str + 1] : 
	      f1 * f1 / m_string_freqs[str - 1]);
  if (m_freq < f0 || m_freq > f2)
    y = 2 * h;
  else if (m_freq <= f1)
    y = h - (h / 2.0) * (m_freq - f0) / (f1 - f0);
  else 
    y = (h / 2.0) - (h / 2.0) * (m_freq - f1) / (f2 - f1);
  cc->move_to(w * 7.0 / 8, y);
  cc->line_to(w, y);
  cc->set_source_rgba(1, 1, 0, 1);
  cc->stroke();
}


bool TunerWidget::on_button_press_event(GdkEventButton* event) {
  return true;
}


bool TunerWidget::on_scroll_event(GdkEventScroll* event) {
  return true;
}
