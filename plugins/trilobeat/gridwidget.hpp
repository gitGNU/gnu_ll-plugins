/****************************************************************************
    
    gridwidget.hpp - Simple drum sequencer widget
    
    Copyright (C) 2008 Lars Luthman <lars.luthman@gmail.com>
    
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

#ifndef GRIDWIDGET_HPP
#define GRIDWIDGET_HPP

#include <map>
#include <string>
#include <vector>

#include <gtkmm/drawingarea.h>
#include <sigc++/signal.h>


class GridWidget : public Gtk::DrawingArea {
public:
  
  GridWidget(unsigned int steps, unsigned int sheight, unsigned int swidth,
	     std::string const& background = "");
  
  void set_key_active(unsigned char key, bool active);
  
  void set_velocity(unsigned char key, unsigned step, unsigned char velocity);
  
  sigc::signal<void, unsigned char, unsigned, unsigned char> 
  signal_velocity_changed;
  
protected:
  
  /// @name Gtkmm event handlers
  // @{
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_key_press_event(GdkEventKey* event);
  virtual bool on_key_release_event(GdkEventKey* event);
  virtual void on_realize();
  // @}
  
  unsigned char row2key(int y);
  
  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Window> m_win;
  Gdk::Color m_dark, m_light, m_blue, m_yellow;
  
  unsigned int m_steps;
  unsigned int m_sheight;
  unsigned int m_swidth;
  std::vector<bool> m_active_keys;
  std::vector< std::vector<unsigned char> > m_seq;
  
  int m_active_x, m_active_y;
  
  int m_rows;
  
  std::string m_background;
  
};


#endif
