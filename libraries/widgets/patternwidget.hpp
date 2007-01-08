/****************************************************************************
    
    patternwidget.hpp - Simple sequencer widget
    
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

#ifndef PATTERNWIDGET_HPP
#define PATTERNWIDGET_HPP

#include <map>
#include <vector>

#include <gtkmm/drawingarea.h>
#include <sigc++/signal.h>

#include "monostep.hpp"


class PatternWidget : public Gtk::DrawingArea {
public:
  
  PatternWidget(unsigned int steps, unsigned int sheight, unsigned int swidth);
  
  std::string get_string() const;
  
  sigc::signal<void> signal_sequence_changed;
  
protected:
  
  /// @name Gtkmm event handlers
  // @{
  virtual bool on_key_press_event(GdkEventKey* event);
  virtual bool on_key_release_event(GdkEventKey* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual bool on_scroll_event(GdkEventScroll* event);
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  // @}
  
  int x2step(double x);
  int y2line(double y);
  
  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Window> m_win;
  Gdk::Color m_white, m_black, m_grey1, m_grey2, m_grey3, m_green1, m_green2, m_purple;
  
  unsigned int m_steps;
  unsigned int m_sheight;
  unsigned int m_swidth;
  
  //std::vector<unsigned char> m_notes;
  //std::vector<bool> m_on;
  //std::vector<bool> m_slide;
  std::vector<MonoStep> m;
  
  std::vector<unsigned char> m_vel_table;
};


#endif
