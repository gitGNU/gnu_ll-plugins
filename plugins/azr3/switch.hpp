#ifndef SWITCH_HPP
#define SWITCH_HPP

#include <gtkmm.h>


class Switch : public Gtk::DrawingArea {
public:
  
  enum Type {
    BigRed,
    Green,
    Mini
  };
  
  Switch(Type type);
  
  void set_value(float value);
  
  Gtk::Adjustment& get_adjustment();
  
  void on_realize();
  bool on_expose_event(GdkEventExpose* event);
  bool on_button_press_event(GdkEventButton* event);

protected:
  
  Gtk::Adjustment m_adj;
  Glib::RefPtr<Gdk::Pixmap> m_pixmap;
  int m_width;
  int m_height;
  Type m_type;
};


#endif
