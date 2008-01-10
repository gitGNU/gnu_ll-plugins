/****************************************************************************
    
    lv2guihost.hpp - Simple LV2 GUI plugin loader for Elven
    
    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>
    
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

#ifndef LV2GUIHOST_HPP
#define LV2GUIHOST_HPP

#include <string>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "lv2.h"
#include "lv2-gui.h"
#include "lv2-gui-programs.h"


class LV2GUIHost {
public:
  
  LV2GUIHost(const std::string& gui_path, const std::string& gui_uri,
	     const std::string& plugin_uri, const std::string& bundle_path);
  
  ~LV2GUIHost();
  
  bool is_valid() const;
  
  Gtk::Widget& get_widget();
  
  void port_event(uint32_t index, uint32_t buffer_size, const void* buffer);
  
  void feedback(uint32_t argc, const char* const* argv);
  
  void program_added(unsigned char number, const char* name);
  
  void program_removed(unsigned char number);
  
  void programs_cleared();
  
  void current_program_changed(unsigned char number);
  
  sigc::signal<void, uint32_t, uint32_t, const void*> write_port;
  
  sigc::signal<void, uint32_t, const char* const*> command;

  sigc::signal<void, unsigned char> request_program;

  sigc::signal<void, unsigned char, const char*> save_program;

protected:
  
  static void _write_port(LV2UI_Controller ctrl, uint32_t index, 
			  uint32_t buffer_size, const void* buffer);
  
  static void _command(LV2UI_Controller ctrl, 
		       uint32_t argc, const char* const* argv);
  
  static void _request_program(LV2UI_Controller ctrl, unsigned char number);

  static void _save_program(LV2UI_Controller ctrl, unsigned char number,
			    const char* name);
  
  
  /** This is needed to cast void* (returned by dlsym()) to a function
      pointer. */
  template <typename A, typename B> A nasty_cast(B b) {
    union {
      A a;
      B b;
    } u;
    u.b = b;
    return u.a;
  }

  
  const LV2UI_Descriptor* m_desc;
  LV2UI_Handle m_ui;
  GtkWidget* m_cwidget;
  Gtk::Widget* m_widget;
  
  bool m_block_gui;
  
  LV2UI_Programs_HDesc m_ui_hdesc;
};


#endif
