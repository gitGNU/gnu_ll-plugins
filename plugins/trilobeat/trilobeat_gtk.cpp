/****************************************************************************
    
    trilobeat_gtk.cpp - GUI for the Trilobeat LV2 plugin
    
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

#include <iostream>
#include <string>

#include <gtkmm.h>
#include <lv2gui.hpp>

#include "gridwidget.hpp"
#include "keynamewidget.hpp"
#include "trilobeat.peg"


using namespace sigc;
using namespace Gtk;


class TrilobeatGUI : public LV2::GUI<TrilobeatGUI, LV2::CommandGUI<true> > {
public:
  
  TrilobeatGUI(const std::string& URI)
    : m_keys(16, std::string(bundle_path()) + "keynamebg.png"),
      m_grid(32, 16, 16, std::string(bundle_path()) + "gridbg.png") {
    pack_start(m_keys);
    pack_start(m_grid);
    m_keys.signal_names_changed.
      connect(mem_fun(*this, &TrilobeatGUI::send_names));
    m_grid.signal_velocity_changed.
      connect(mem_fun(*this, &TrilobeatGUI::send_velocity));
  }
  
  void feedback(uint32_t argc, char const* const* argv) {
    std::cout<<"Trilobeat GUI received";
    for (uint32_t i = 0; i < argc; ++i)
      std::cout<<" "<<argv[i];
    std::cout<<std::endl;
    
    if (argc == 3 && !strcmp(argv[0], "keyname")) {
      int k = atoi(argv[1]);
      if (k >= 0 && k < 128) {
	m_keys.set_keyname(k, argv[2]);
	m_grid.set_key_active(k, true);
      }
    }
    
    else if (argc == 4 && !strcmp(argv[0], "velocity")) {
      m_grid.set_velocity(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    }
    
  }
  
protected:
  
  void send_names(std::string const& names) {
    std::cout<<__PRETTY_FUNCTION__<<std::endl;
    char const* cmd[] = { "new_names", 0 };
    cmd[1] = names.c_str();
    command(2, cmd);
  }

  void send_velocity(unsigned char key, unsigned step, unsigned char velocity) {
    char key_str[4];
    char step_str[20];
    char velocity_str[4];
    sprintf(key_str, "%d", key);
    sprintf(step_str, "%d", step);
    sprintf(velocity_str, "%d", velocity);
    char const* cmd[] = { "velocity", key_str, step_str, velocity_str };
    command(4, cmd);
  }
  
  KeyNameWidget m_keys;
  GridWidget m_grid;
  
};


static int _ = TrilobeatGUI::register_class((std::string(k_uri) + "/gui").c_str());

