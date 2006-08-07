/****************************************************************************
    
    AZR-3 - An LV2 synth plugin
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include <string>
#include <vector>

#include <gtkmm.h>
#include <pangomm.h>


class Textbox : public Gtk::DrawingArea {
public:
  
  Textbox(int width, int height, int lines);
  
  void set_string(int line, const std::string& str);
  
protected:
  
  bool on_expose_event(GdkEventExpose* event);

  int m_width;
  int m_height;
  std::vector<Glib::RefPtr<Pango::Layout> > m_strings;
  Gdk::Color m_color;
  
};


#endif