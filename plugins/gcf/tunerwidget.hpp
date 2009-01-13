#ifndef TUNERWIDGET_HPP
#define TUNERWIDGET_HPP

#include <string>

#include <cairomm/cairomm.h>
#include <gtkmm.h>


class TunerWidget : public Gtk::DrawingArea {
public:
  
  TunerWidget(const std::string& bundle);
  ~TunerWidget();
  
  void set_frequency(float freq);
  
protected:
  
  enum {
    NUM_STRINGS = 6
  };
  
  void on_realize();
  bool on_expose_event(GdkEventExpose* event);
  bool on_button_press_event(GdkEventButton* event);
  bool on_scroll_event(GdkEventScroll* event);
  
  Gdk::Color m_bg;
  
  float m_freq;
  float m_string_freqs[NUM_STRINGS];
  float m_threshold;
  
};


#endif
