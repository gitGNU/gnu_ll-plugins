/****************************************************************************
    
    envelope_gtk.cpp - A GUI for the envelope LV2 plugin
    
    Copyright (C) 2006-2007  Lars Luthman <lars.luthman@gmail.com>
    
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

#include <cstring>
#include <iostream>

#include <gtkmm.h>

#include "lv2gui.hpp"
#include "envelopeeditor.hpp"


using namespace std;
using namespace Gtk;
using namespace sigc;


class EnvelopeGUI : public LV2::GUI<EnvelopeGUI> {
public:
  
  EnvelopeGUI(const std::string& URI) {
    add(m_ee);
    m_ee.signal_apply.connect(mem_fun(*this, &EnvelopeGUI::do_apply));
  }
  
  void feedback(uint32_t argc, const char* const* argv) {
    if (argc == 2 && !strcmp("envelope", argv[0]))
      m_ee.set_string(argv[1]);
  }
  
protected:
  
  void do_apply() {
    std::string tmp = m_ee.get_string();
    const char* argv[] = { "envelope", tmp.c_str() };
    // XXX command(2, argv);
  }
  
  
  EnvelopeEditor m_ee;
  
};


static int _ = EnvelopeGUI::register_class("http://ll-plugins.nongnu.org/lv2/dev/envelope/0/gui");
