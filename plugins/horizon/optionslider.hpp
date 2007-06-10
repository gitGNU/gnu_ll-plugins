#ifndef OPTIONSLIDER_HPP
#define OPTIONSLIDER_HPP

#include <string>
#include <vector>

#include "labelslider.hpp"


class OptionSlider : public LabelSlider {
public:
  
  OptionSlider(double lower, double upper, double value, int digits, 
	       const std::vector<std::string>& options);
  
  void set_option(int option);

private:

  bool on_expose_event(GdkEventExpose* event);
  bool on_motion_notify_event(GdkEventMotion* event);
  bool on_button_press_event(GdkEventButton* event);
  bool on_scroll_event(GdkEventScroll* event);
  
  int m_option;
  int m_noptions;
  std::string m_prefix;
  Gdk::Color m_bg;
  Gtk::Menu m_menu;
  
  std::vector<std::string> m_labels;
};


#endif
