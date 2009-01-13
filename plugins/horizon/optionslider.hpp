/****************************************************************************

    optionslider.hpp

    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

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
