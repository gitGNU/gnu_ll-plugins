/****************************************************************************

    labelslider.hpp

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
