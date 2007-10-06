/****************************************************************************
    
    gcf_gtk.cpp - GUI for the GCF LV2 plugin
    
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

#include <iostream>
#include <string>

#include <gtkmm.h>

#include "lv2gui.hpp"
#include "tunerwidget.hpp"


using namespace sigc;
using namespace Gtk;


class GCFGUI : public LV2::GUI {
public:
  
  GCFGUI(LV2::Controller& ctrl, const std::string& URI, 
	 const std::string& bundle_path)
    : m_twd(bundle_path) {
    add(m_twd);
    m_twd.show();
  }

  void port_event(uint32_t port, uint32_t buffer_size, const void* buffer) {
    if (port == 1)
      m_twd.set_frequency(*static_cast<const float*>(buffer));
  }
  
protected:
  
  TunerWidget m_twd;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  LV2::register_lv2gtk2gui<GCFGUI>("http://ll-plugins.nongnu.org/lv2/dev/gcf/0/gui");
}
