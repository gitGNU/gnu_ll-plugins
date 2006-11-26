/****************************************************************************
    
    euphoriaguiplugin.cpp - A GUI for the Euphoria LV2 synth
    
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

#include <iostream>

#include <gtkmm.h>

#include "lv2gtk2gui.hpp"
#include "euphoriawidget.hpp"


using namespace std;
using namespace Gtk;
using namespace sigc;


class EuphoriaGUI : public LV2GTK2GUI {
public:
  
  EuphoriaGUI(LV2Controller& ctrl, const std::string& URI, 
              const std::string& bundle_path, Widget*& widget) {
    widget = &m_euph;
    m_euph.signal_control_changed.
      connect(mem_fun(ctrl, &LV2Controller::set_control));
  }
  
  void set_control(uint32_t port, float value) {
    m_euph.set_control(port, value);
  }

protected:

  EuphoriaWidget m_euph;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2gtk2gui<EuphoriaGUI>("http://ll-plugins.nongnu.org/lv2/dev/euphoria/0.0.0");
}
