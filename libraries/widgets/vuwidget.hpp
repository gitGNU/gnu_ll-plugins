/****************************************************************************
    
    vuwidget.hpp - simple VU meter
    
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

#ifndef VUWIDGET_HPP
#define VUWIDGET_HPP

#include <string>
#include <vector>

#include <gtkmm.h>


class VUWidget : public Gtk::DrawingArea {
public:
  
  VUWidget();
  
  void set_value(float value);
  
protected:
  
  bool on_expose_event(GdkEventExpose* event);

  float m_value;
  Gdk::Color m_bg, m_fg1, m_fg2, m_fg3;

};


#endif
