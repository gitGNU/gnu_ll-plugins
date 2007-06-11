#include <cstdio>
#include <iostream>

#include "samplemodel.hpp"
#include "sampleview.hpp"


using namespace std;


SampleView::SampleView() 
  : m_model(0),
    m_scroll_adj(0, 0, 0) {
  
  m_bg.set_rgb(55000, 55000, 60000);
  m_bgl.set_rgb(65000, 65000, 65000);
  m_bgd.set_rgb(20000, 20000, 40000);
  m_fg.set_rgb(0, 20000, 65000);
  Glib::RefPtr<Gdk::Colormap> cmap = Gdk::Colormap::get_system();
  cmap->alloc_color(m_bg);
  cmap->alloc_color(m_bgl);
  cmap->alloc_color(m_bgd);
  cmap->alloc_color(m_fg);
  
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON1_MOTION_MASK | 
             Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK);  
  
  m_scroll_adj.signal_value_changed().
    connect(mem_fun(*this, &SampleView::queue_draw));
}


void SampleView::set_model(SampleModel* model) {
  m_model = model;
  queue_draw();
  if (m_model)
    m_scroll_adj.set_upper(model->get_length() / 16 + 1);
  else
    m_scroll_adj.set_upper(0);
  m_scroll_adj.set_value(0);
}


Gtk::Adjustment& SampleView::get_scroll_adjustment() {
  return m_scroll_adj;
}

  
bool SampleView::on_expose_event(GdkEventExpose* event) {
  
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  gc->set_foreground(m_bg);
  int w = get_width();
  int h = get_height();
  
  // draw background
  win->draw_rectangle(gc, true, 0, 0, w, h);
  gc->set_foreground(m_bgd);
  win->draw_line(gc, w - 1, 0, w - 1, h - 1);
  win->draw_line(gc, 0, h - 1, w - 1, h - 1);
  gc->set_foreground(m_bgl);
  win->draw_line(gc, 0, 0, w, 0);
  win->draw_line(gc, 0, 0, 0, h);
  
  if (!m_model)
    return true;
  
  // draw audio
  int m = h / 2;
  gc->set_foreground(m_fg);
  const SampleModel::PeakData* peak = m_model->get_peak_data()[0];
  size_t scroll = m_scroll_adj.get_value();
  for (size_t i = scroll; i < m_model->get_length() / 16 + 1; ++i)
    win->draw_line(gc, i - scroll, m + int((m - 1) * peak[i].min), 
		   i - scroll, m + int((m - 1) * peak[i].max));

  return true;
}


bool SampleView::on_motion_notify_event(GdkEventMotion* event) {
  return true;
}


bool SampleView::on_button_press_event(GdkEventButton* event) {
  return true;
}


bool SampleView::on_scroll_event(GdkEventScroll* event) {
  return true;
}


void SampleView::on_size_allocate(Gtk::Allocation& a) {
  DrawingArea::on_size_allocate(a);
  m_scroll_adj.set_page_size(a.get_width());
}

