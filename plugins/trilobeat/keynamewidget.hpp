/****************************************************************************
    
    keynamewidget.hpp - Simple drum sequencer widget
    
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

#ifndef KEYNAMEWIDGET_HPP
#define KEYNAMEWIDGET_HPP

#include <map>
#include <string>
#include <vector>

#include <gtkmm/drawingarea.h>
#include <sigc++/signal.h>


class KeyNameWidget : public Gtk::DrawingArea {
public:
  
  KeyNameWidget(unsigned int sheight, std::string const& background = "");
  
  void set_keyname(unsigned char key, std::string const& name);
  
  sigc::signal<void, std::string const&> signal_names_changed;
  
protected:
  
  struct KeyInfo {
    KeyInfo(bool a = false, std::string const& n = "") : active(a), name(n) { }
    bool active;
    std::string name;
  };
  
  /// @name Gtkmm event handlers
  // @{
  bool on_button_press_event(GdkEventButton* event);
  void on_realize();
  bool on_expose_event(GdkEventExpose* event);
  void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context,
			     int x, int y, 
			     const Gtk::SelectionData& selection_data,
			     guint info, guint time);
// @}
  
  unsigned char row2key(int y);
  
  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Window> m_win;
  Gdk::Color m_white;
  Glib::RefPtr<Pango::Layout> m_layout;
  
  unsigned int m_sheight;
  std::vector<KeyInfo> m_keyinfo;

  std::string m_background;
  
};


#endif
