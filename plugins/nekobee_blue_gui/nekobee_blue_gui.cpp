/****************************************************************************
    
    nekobee_blue_gui.cpp - A GUI for the Nekobee LV2 synth
    
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

#include <iostream>

#include <gtkmm.h>

#include "lv2gtk2gui.hpp"
#include "nekobeewidget.hpp"


using namespace std;
using namespace Gtk;
using namespace sigc;


class NekobeeBlueGUI : public LV2GTK2GUI {
public:
  
  NekobeeBlueGUI(LV2Controller& ctrl, const std::string& URI, 
		 const std::string& bundle_path)
    : m_nkb(bundle_path),
      m_ctrl(ctrl) {
    
    pack_start(m_nkb);
    
    m_nkb.signal_waveform_changed.
      connect(bind<0>(mem_fun(*this, &NekobeeBlueGUI::request_control_change), 1));
    m_nkb.signal_tuning_changed.
      connect(bind<0>(mem_fun(*this, &NekobeeBlueGUI::request_control_change), 2));
    m_nkb.signal_cutoff_changed.
      connect(bind<0>(mem_fun(*this, &NekobeeBlueGUI::request_control_change), 3));
    m_nkb.signal_resonance_changed.
      connect(bind<0>(mem_fun(*this, &NekobeeBlueGUI::request_control_change), 4));
    m_nkb.signal_envmod_changed.
      connect(bind<0>(mem_fun(*this, &NekobeeBlueGUI::request_control_change), 5));
    m_nkb.signal_decay_changed.
      connect(bind<0>(mem_fun(*this, &NekobeeBlueGUI::request_control_change), 6));
    m_nkb.signal_accent_changed.
      connect(bind<0>(mem_fun(*this, &NekobeeBlueGUI::request_control_change), 7));
    m_nkb.signal_volume_changed.
      connect(bind<0>(mem_fun(*this, &NekobeeBlueGUI::request_control_change), 8));
  }
  
  void port_event(uint32_t port, uint32_t buffer_size, const void* buffer) {
    if (port == 1)
      m_nkb.set_waveform(*static_cast<const float*>(buffer));
    else if (port == 2)
      m_nkb.set_tuning(*static_cast<const float*>(buffer));
    else if (port == 3)
      m_nkb.set_cutoff(*static_cast<const float*>(buffer));
    else if (port == 4)
      m_nkb.set_resonance(*static_cast<const float*>(buffer));
    else if (port == 5)
      m_nkb.set_envmod(*static_cast<const float*>(buffer));
    else if (port == 6)
      m_nkb.set_decay(*static_cast<const float*>(buffer));
    else if (port == 7)
      m_nkb.set_accent(*static_cast<const float*>(buffer));
    else if (port == 8)
      m_nkb.set_volume(*static_cast<const float*>(buffer));
  }

protected:
  
  void request_control_change(uint32_t port, float value) {
    m_ctrl.write(port, sizeof(float), &value);
  }
  
  NekobeeWidget m_nkb;
  LV2Controller& m_ctrl;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2gtk2gui<NekobeeBlueGUI>("http://ll-plugins.nongnu.org/lv2/dev/nekobee_blue_gui/0.0.0");
}
