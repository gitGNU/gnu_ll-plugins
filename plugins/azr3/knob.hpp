#ifndef KNOB_HPP
#define KNOB_HPP

#include <gtkmm.h>


class Knob : public Gtk::DrawingArea {
public:
  
  Knob(float min, float max, float value);
  
  void set_value(float value);
  
  Gtk::Adjustment& get_adjustment();
  
  void on_realize();
  bool on_expose_event(GdkEventExpose* event);
  bool on_motion_notify_event(GdkEventMotion* event);
  bool on_button_press_event(GdkEventButton* event);

protected:
  
  Gtk::Adjustment m_adj;
  Glib::RefPtr<Gdk::Pixmap> m_pixmap;
  Glib::RefPtr<Gdk::Bitmap> m_bitmap;
  int m_click_offset;
  float m_value_offset;
};


#endif
