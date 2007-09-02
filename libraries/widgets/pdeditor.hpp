/****************************************************************************
    
    pdeditor.hpp - gtkmm widget for editing phase distortion functions
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
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

#ifndef PDEDITOR_HPP
#define PDEDITOR_HPP

#include <string>
#include <vector>

#include <gtkmm.h>


class PDEditor : public Gtk::DrawingArea {
public:
  
  PDEditor();
  
  bool set_string(const std::string& str);
  std::string get_string() const;
  
  sigc::signal<void> signal_apply;

protected:
  
  struct Point {
    
    Point(double _o = 0)
      : o(_o),
        x(_o),
        y(_o) {

    }
    
    double o;
    double x;
    double y;
  };
  
  
  bool on_expose_event(GdkEventExpose* event);
  bool on_motion_notify_event(GdkEventMotion* event);
  bool on_button_release_event(GdkEventButton* event);
  bool on_button_press_event(GdkEventButton* event);
  bool on_scroll_event(GdkEventScroll* event);
  
  void new_point();
  void delete_point();
  void set_dirty();
  void apply();
  
  double x2p(double x);
  double y2p(double y);
  double p2x(int p);
  double p2y(int p);
  
  std::vector<Point> m_points;
  
  double m_margin;
  double m_click_x, m_click_y;
  int m_active_point;
  bool m_dragging;
  bool m_odragging;
  int m_pix_drag_x, m_pix_drag_y;
  double m_drag_x, m_drag_y;
  
  bool m_wave;
  
  bool m_dirty;
  
  Gtk::Menu m_menu;
  
};


#endif
