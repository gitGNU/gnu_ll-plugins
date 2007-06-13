#ifndef SAMPLEVIEW_HPP
#define SAMPLEVIEW_HPP

#include <gtkmm.h>


class SampleModel;


class SampleView : public Gtk::DrawingArea {
public:
  
  SampleView();
  
  void set_model(SampleModel* model);
  
  Gtk::Adjustment& get_scroll_adjustment();
  
protected:
  
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_scroll_event(GdkEventScroll* event);
  virtual void on_size_allocate(Gtk::Allocation& allocation);

  Gdk::Color m_bg, m_fg, m_bgl, m_bgd, m_fgl, m_fgd;
  
  SampleModel* m_model;
  
  Gtk::Adjustment m_scroll_adj;
  
  int m_scale;
  
};


#endif
