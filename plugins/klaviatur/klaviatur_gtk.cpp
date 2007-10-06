/****************************************************************************
    
    klaviatur_gtk.cpp - GUI for the Klaviatur LV2 plugin
    
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
#include "klaviatur.peg"
#include "keyboard.hpp"


using namespace sigc;
using namespace Gtk;


class KlaviaturGUI : public LV2::GUI {
public:
  
  KlaviaturGUI(LV2::Controller& ctrl, const std::string& URI, 
               const std::string& bundle_path) 
    : m_cc(0, 128, 1),
      m_pitch(-8192, 8192, 1),
      m_vel(1, 128, 1) {

    pack_start(m_vbox);
    
    // initialise control widgets
    m_kb.set_flags(m_kb.get_flags() | CAN_FOCUS);
    m_cc.set_digits(0);
    m_cc.set_draw_value(true);
    m_cc.set_value_pos(POS_RIGHT);
    m_cc.set_value(0);
    m_pitch.set_digits(0);
    m_pitch.set_draw_value(true);
    m_pitch.set_value_pos(POS_RIGHT);
    m_pitch.set_value(0);
    m_vel.set_digits(0);
    m_vel.set_draw_value(true);
    m_vel.set_value_pos(POS_RIGHT);
    m_vel.set_value(64);
    m_cc_sbn.set_range(0, 127);
    m_cc_sbn.set_increments(1, 16);
    m_cc_sbn.set_digits(0);
    m_cc_sbn.set_snap_to_ticks(true);
    
    // layout
    Table* table = manage(new Table(3, 3));
    table->set_border_width(6);
    table->set_spacings(6);
    table->attach(*manage(new Label("CC:", ALIGN_LEFT)), 0, 1, 0, 1, FILL); 
    table->attach(m_cc_sbn, 1, 2, 0, 1, FILL);
    table->attach(m_cc, 2, 3, 0, 1);
    table->attach(*manage(new Label("Pitch:", ALIGN_LEFT)), 0, 2, 1, 2, FILL);
    table->attach(m_pitch, 2, 3, 1, 2);
    table->attach(*manage(new Label("Velocity:", ALIGN_LEFT)),0, 2, 2, 3, FILL);
    table->attach(m_vel, 2, 3, 2, 3);
    Expander* exp = manage(new Expander("Controls"));
    exp->add(*table);
    m_vbox.pack_start(*exp);
    m_vbox.pack_start(m_kb);
    m_kb.grab_focus();
    
    // connect signals
    m_kb.signal_key_on().
      connect(bind(mem_fun(*this, &KlaviaturGUI::handle_keypress), ref(ctrl)));
    m_kb.signal_key_off().
      connect(bind(mem_fun(*this, &KlaviaturGUI::handle_keyrelease),ref(ctrl)));
    m_cc.signal_value_changed().
      connect(bind(mem_fun(*this, &KlaviaturGUI::handle_cc_change), ref(ctrl)));
    m_cc_sbn.signal_value_changed().
      connect(bind(mem_fun(*this, &KlaviaturGUI::handle_cc_change), ref(ctrl)));
    m_pitch.signal_value_changed().
      connect(bind(mem_fun(*this, &KlaviaturGUI::handle_pitch_change), 
                   ref(ctrl)));
  }
  
protected:

  void handle_keypress(unsigned char key, LV2::Controller& ctrl) {
    unsigned char data[3] = { 0x90, key + 36, int(m_vel.get_value()) };
    ctrl.write(k_midi_input, 3, data);
  }
  
  
  void handle_keyrelease(unsigned char key, LV2::Controller& ctrl) {
    unsigned char data[3] = { 0x80, key + 36, 64 };
    ctrl.write(k_midi_input, 3, data);
  }
  
  
  void handle_cc_change(LV2::Controller& ctrl) {
    unsigned char data[3] = { 0xB0, int(m_cc_sbn.get_value()),
                              int(m_cc.get_value()) };
    ctrl.write(k_midi_input, 3, data);
  }
  
  
  void handle_pitch_change(LV2::Controller& ctrl) {
    int value = int(m_pitch.get_value()) + 8192;
    unsigned char data[3] = { 0xE0, int(value & 127), int(value >> 7) };
    ctrl.write(k_midi_input, 3, data);
  }
  
  
  HScale m_pitch;
  HScale m_cc;
  HScale m_vel;
  SpinButton m_cc_sbn;
  Keyboard m_kb;
  VBox m_vbox;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  LV2::register_lv2gtk2gui<KlaviaturGUI>(std::string(k_uri) + "/gui");
}
