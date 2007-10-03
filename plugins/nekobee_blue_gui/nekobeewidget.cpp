#include <cstdio>
#include <sstream>

#include "nekobeewidget.hpp"


using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace std;


NekobeeWidget::NekobeeWidget(const string& bundle) 
  : m_bundle(bundle),
    m_active_control(NumControls) {
  
  set_size_request(190, 127);
  m_bar_colour.set_rgb(20000, 40000, 65000);
  m_text_colour.set_rgb(60000, 60000, 65000);
  RefPtr<Colormap> cmap = Colormap::get_system();
  cmap->alloc_color(m_bar_colour);
  cmap->alloc_color(m_text_colour);
  add_events(SCROLL_MASK | BUTTON_PRESS_MASK);
  
  for (unsigned i = 0; i < NumControls; ++i)
    m_controls[i] = 0.0;
  
  m_control_names[Tuning] = "Tuning";
  m_min_values[Tuning] = 0.5;
  m_max_values[Tuning] = 2.0;
  m_signals[Tuning] = &signal_tuning_changed;
  
  m_control_names[Cutoff] = "Cutoff";
  m_min_values[Cutoff] = 0.0;
  m_max_values[Cutoff] = 40.0;
  m_signals[Cutoff] = &signal_cutoff_changed;

  m_control_names[Resonance] = "Resonance";
  m_min_values[Resonance] = 0.0;
  m_max_values[Resonance] = 0.95;
  m_signals[Resonance] = &signal_resonance_changed;

  m_control_names[EnvMod] = "EnvMod";
  m_min_values[EnvMod] = 0.0;
  m_max_values[EnvMod] = 1.0;
  m_signals[EnvMod] = &signal_envmod_changed;

  m_control_names[Decay] = "Decay";
  m_min_values[Decay] = 0.000009;
  m_max_values[Decay] = 0.0007;
  m_signals[Decay] = &signal_decay_changed;

  m_control_names[Accent] = "Accent";
  m_min_values[Accent] = 0.0;
  m_max_values[Accent] = 1.0;
  m_signals[Accent] = &signal_accent_changed;

  m_control_names[Volume] = "Volume";
  m_min_values[Volume] = 0.0;
  m_max_values[Volume] = 1.0;
  m_signals[Volume] = &signal_volume_changed;

  m_control_names[Waveform] = "Waveform";
  m_min_values[Waveform] = 0.0;
  m_max_values[Waveform] = 1.0;
  m_signals[Waveform] = &signal_waveform_changed;
  
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
}


NekobeeWidget::~NekobeeWidget() {

}

  
void NekobeeWidget::set_waveform(float waveform) {
  m_controls[Waveform] = waveform;
  queue_draw();
}


void NekobeeWidget::set_tuning(float tuning) {
  m_controls[Tuning] = tuning;
  queue_draw();
}


void NekobeeWidget::set_cutoff(float cutoff) {
  m_controls[Cutoff] = cutoff;
  queue_draw();
}


void NekobeeWidget::set_resonance(float resonance) {
  m_controls[Resonance] = resonance;
  queue_draw();
}


void NekobeeWidget::set_envmod(float envmod) {
  m_controls[EnvMod] = envmod;
  queue_draw();
}


void NekobeeWidget::set_decay(float decay) {
  m_controls[Decay] = decay;
  queue_draw();
}


void NekobeeWidget::set_accent(float accent) {
  m_controls[Accent] = accent;
  queue_draw();
}


void NekobeeWidget::set_volume(float volume) {
  m_controls[Volume] = volume;
  queue_draw();
}


void NekobeeWidget::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Pixbuf> pixbuf = Pixbuf::create_from_file(m_bundle + "drawing.png");
  RefPtr<Pixmap> pixmap = Pixmap::create(get_window(), 
					 pixbuf->get_width(), 
					 pixbuf->get_height());
  RefPtr<Bitmap> bitmap;
  pixbuf->render_pixmap_and_mask(pixmap, bitmap, 10);
  RefPtr<Style> s = get_style()->copy();
  s->set_bg_pixmap(STATE_NORMAL, pixmap);
  s->set_bg_pixmap(STATE_ACTIVE, pixmap);
  s->set_bg_pixmap(STATE_PRELIGHT, pixmap);
  s->set_bg_pixmap(STATE_SELECTED, pixmap);
  s->set_bg_pixmap(STATE_INSENSITIVE, pixmap);
  set_style(s);
  get_window()->shape_combine_mask(bitmap, 0, 0);
}


bool NekobeeWidget::on_expose_event(GdkEventExpose* event) {
  
  RefPtr<Gdk::Window> win = get_window();
  RefPtr<GC> gc = GC::create(win);
  gc->set_foreground(m_bar_colour);
  
  ::Cairo::RefPtr< ::Cairo::Context > cc = win->create_cairo_context();
  cc->set_line_join(::Cairo::LINE_JOIN_ROUND);
  
  // bars
  draw_bar(cc, 61, 65, 76, 84, 46, 123, 2, 65, Tuning);
  draw_bar(cc, 113, 84, 119.5, 75.767, 164, 95.267, 143, 123, Cutoff);
  draw_bar(cc, 121.55, 73.233, 128, 65, 187, 65, 166, 92.733, Resonance);
  draw_bar(cc, 128, 61, 113, 42, 143, 3, 187, 61, Volume);
  draw_bar(cc, 110, 40, 97, 40, 97, 2, 140, 2, Decay);
  draw_bar(cc, 93, 40, 79, 40, 49, 2, 93, 2, EnvMod);
  draw_bar(cc, 76, 42, 61, 61, 2, 61, 46, 3, Accent);
  
  // waveforms
  cc->set_line_width(2.5);
  cc->move_to(71, 110);
  cc->line_to(71, 101);
  cc->line_to(90, 110);
  cc->line_to(90, 101);
  if (m_controls[Waveform] <= 0) {
    if (m_active_control == Waveform)
      cc->set_source_rgba(0.9, 0.9, 1.0, 1.0);
    else
      cc->set_source_rgba(0.3, 0.6, 1.0, 1.0);
  }
  else
    cc->set_source_rgba(0.17, 0.35, 1.0, 1.0);
  cc->stroke();
  cc->move_to(100, 110);
  cc->line_to(100, 101);
  cc->line_to(109.5, 101);
  cc->line_to(109.5, 110);
  cc->line_to(119, 110);
  cc->line_to(119, 101);
  if (m_controls[Waveform] > 0)
    if (m_active_control == Waveform)
      cc->set_source_rgba(0.9, 0.9, 1.0, 1.0);
    else
      cc->set_source_rgba(0.3, 0.6, 1.0, 1.0);
  else
    cc->set_source_rgba(0.17, 0.35, 1.0, 1.0);
  cc->stroke();
  
  // text
  if (m_active_control != NumControls) {
    gc->set_foreground(m_text_colour);
    RefPtr<Pango::Layout> l = Pango::Layout::create(get_pango_context());
    Pango::FontDescription fd;
    fd.set_family("sans");
    fd.set_absolute_size(m_text_size);
    l->set_font_description(fd);

    l->set_text(m_control_names[m_active_control]);
    Pango::Rectangle r = l->get_pixel_logical_extents();
    win->draw_layout(gc, 95 - r.get_width() / 2, 63 - r.get_height() / 2, l);
    
    if (m_active_control == Waveform) {
      if (m_controls[Waveform] <= 0)
	l->set_text("Saw");
      else
	l->set_text("Square");
    }
    else {
      char tmp[10];
      sprintf(tmp, "%.3f", m_controls[m_active_control]);
      l->set_text(tmp);
    }
    r = l->get_pixel_logical_extents();
    win->draw_layout(gc, 95 - r.get_width() / 2, 73 - r.get_height() / 2, l);
  }
  
}


bool NekobeeWidget::on_button_press_event(GdkEventButton* event) {
  if (event->button != 1)
    return false;
  int x = event->x;
  int y = event->y;
  if (y > 63) {
    float k = (86 - 125) / float(78 - 46);
    float y0 = 125 - k * 46;
    if (y > y0 + k * x) {
      k = (125 - 86) / float(141 - 111);
      y0 = 86 - k * 111;
      if (y > y0 + k * x) {
	if (x < 95)
	  m_controls[Waveform] = 0;
	else
	  m_controls[Waveform] = 1;
	(*m_signals[Waveform])(m_controls[Waveform]);
	set_text(Waveform);
      }
    }
  }

  return true;
}


bool NekobeeWidget::on_scroll_event(GdkEventScroll* event) {
  
  // find the bar that is being scrolled
  int x = event->x;
  int y = event->y;
  Control c = NumControls;
  float k, y0;
  
  // lower half
  if (y > 63) {
    k = (86 - 125) / float(78 - 46);
    y0 = 125 - k * 46;
    if (y < y0 + k * x)
      c = Tuning;
    else {
      k = (92.733 - 73.233) / float(166 - 121.55);
      y0 = 73.233 - k * 121.55;
      if (y < y0 + k * x)
	c = Resonance;
      else {
	k = (125 - 86) / float(141 - 111);
	y0 = 86 - k * 111;
	if (y < y0 + k * x)
	  c = Cutoff;
	else
	  c = NumControls;
      }
    }
  }
  
  // upper half
  else {
    k = (40 - 1) / float(77 - 47);
    y0 = 1 - k * 47;
    if (y > y0 + k * x)
      c = Accent;
    else {
      k = (1 - 41) / float(142 - 112);
      y0 = 41 - k * 112;
      if (y > y0 + k * x)
	c = Volume;
      else if (x > 95)
	c = Decay;
      else
	c = EnvMod;
    }
  }
  
  if (c == NumControls)
    return true;
  
  float step = (m_max_values[c] - m_min_values[c]) * 0.1;
  if (event->state & GDK_SHIFT_MASK)
    step *= 0.1;
  if (event->direction == GDK_SCROLL_UP) {
    m_controls[c] += step;
    m_controls[c] = m_controls[c] > m_max_values[c] ? m_max_values[c] : m_controls[c];
    (*m_signals[c])(m_controls[c]);
    set_text(c);
  }
  else if (event->direction == GDK_SCROLL_DOWN) {
    m_controls[c] -= step;
    m_controls[c] = m_controls[c] < m_min_values[c] ? m_min_values[c] : m_controls[c];
    (*m_signals[c])(m_controls[c]);
    set_text(c);
  }
  return true;
}



void NekobeeWidget::draw_bar(::Cairo::RefPtr< ::Cairo::Context>& cc, 
			     float x1, float y1, float x2, float y2, 
			     float x3, float y3, float x4, float y4, 
			     Control c) {
  float value = (m_controls[c] - m_min_values[c]) / 
    (m_max_values[c] - m_min_values[c]);
  value = value < 0 ? 0 : value;
  value = value > 1 ? 1 : value;
  value *= 0.9;
  
  cc->move_to(x1, y1);
  cc->line_to(x2, y2);
  cc->line_to((1 - value) * x2 + value * x3, 
	      (1 - value) * y2 + value * y3);
  cc->line_to((1 - value) * x1 + value * x4, 
	      (1 - value) * y1 + value * y4);
  if (c == m_active_control)
    cc->set_source_rgba(0.9, 0.9, 1.0, 1.0);
  else
    cc->set_source_rgba(0.3, 0.6, 1.0, 1.0);
  cc->fill();
}


void NekobeeWidget::set_text(Control c) {
  m_active_control = c;
  m_text_connection.disconnect();
  m_text_connection = signal_timeout()
    .connect(bind_return(mem_fun(*this, &NekobeeWidget::hide_text), false),
	     3000);
  queue_draw();
}


void NekobeeWidget::hide_text() {
  m_active_control = NumControls;
  queue_draw();
}
