/****************************************************************************
    
    vgknob.hpp - simple gtkmm knob
    
    Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA

****************************************************************************/

#ifndef VGKNOB_HPP
#define VGKNOB_HPP

#include <gtkmm.h>
#include <cairomm/cairomm.h>


class VGKnob : public Gtk::DrawingArea {
public:
  
  VGKnob(float min, float max, float value, 
         float red, float green, float blue, bool integer, bool logarithmic);
  
  void set_value(float value);
  
  Gtk::Adjustment& get_adjustment();
  
protected:
  
  bool on_expose_event(GdkEventExpose* event);
  bool on_motion_notify_event(GdkEventMotion* event);
  bool on_button_press_event(GdkEventButton* event);
  bool on_scroll_event(GdkEventScroll* event);
  
  int draw_digit(Cairo::RefPtr<Cairo::Context>& cc, char digit);
  void draw_string(Cairo::RefPtr<Cairo::Context>& cc, const std::string& str,
                   float x, float y);
  double map_to_adj(double knob);
  double map_to_knob(double adj);
  
  Gtk::Adjustment m_adj;
  int m_click_offset;
  float m_value_offset;
  float m_red, m_green, m_blue;
  bool m_integer;
  bool m_logarithmic;
  float m_step;
};


#endif
