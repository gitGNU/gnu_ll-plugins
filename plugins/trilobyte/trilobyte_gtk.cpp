/****************************************************************************
    
    trilobyte_gtk.cpp - GUI for the Trilobyte sequencer
    
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
#include <string>

#include <gtkmm.h>

#include "lv2gtk2gui.hpp"
#include "trilobyte.peg"
#include "patternwidget.hpp"


using namespace sigc;
using namespace Gtk;


class TrilobyteGUI : public LV2GTK2GUI {
public:
  
  TrilobyteGUI(LV2Controller& ctrl, const std::string& URI, 
               const std::string& bundle_path, Widget*& widget)
    : m_pat(32, 15, 15) {
    widget = &m_pat;
  }
  
protected:

  PatternWidget m_pat;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2gtk2gui<TrilobyteGUI>(k_uri);
}
