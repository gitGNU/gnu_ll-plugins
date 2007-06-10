#ifndef LABELSLIDER_HPP
#define LABELSLIDER_HPP

#include <gtkmm.h>


class LabelSlider : public Gtk::DrawingArea {
public:
  
  LabelSlider(double lower, double upper, double value, int digits);
  
  Gtk::Adjustment& get_adjustment();
  
protected:
  
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_scroll_event(GdkEventScroll* event);
  

  Gtk::Adjustment m_adj;
  
  Gdk::Color m_black, m_on, m_off;
  
  Glib::RefPtr<Pango::Layout> m_layout;
  
  int m_drag_x;
  double m_drag_value;
  int m_digits;
};


#endif
