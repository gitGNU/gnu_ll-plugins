/****************************************************************************
    
    sineshaperwidget.cpp - A GUI for the Sineshaper LV2 synth plugin
    
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

#include "sineshaperwidget.hpp"


using namespace Gtk;


SineshaperWidget::SineshaperWidget()
  : HBox(false, 12) {
  
  VBox* knob_vbox = manage(new VBox(false, 12));
  
  Table* table = manage(new Table(3, 2));
  table->set_spacings(12);
  table->attach(*init_tuning_controls(), 0, 1, 0, 1);
  table->attach(*init_osc2_controls(), 1, 2, 0, 1);
  table->attach(*init_vibrato_controls(), 0, 1, 1, 2);
  table->attach(*init_portamento_controls(), 1, 2, 1, 2);
  table->attach(*init_tremolo_controls(), 0, 1, 2, 3);
  table->attach(*init_envelope_controls(), 1, 2, 2, 3);
  
  HBox* knob_hbox = manage(new HBox(false, 12));
  knob_hbox->pack_start(*init_amp_controls());
  knob_hbox->pack_start(*init_delay_controls());
  
  knob_vbox->pack_start(*table);
  knob_vbox->pack_start(*init_shaper_controls());
  knob_vbox->pack_start(*knob_hbox);
  
  VBox* preset_vbox = manage(new VBox(false, 12));
  preset_vbox->pack_start(*init_preset_list());
  preset_vbox->pack_start(*manage(new Button("Save preset")));
  preset_vbox->pack_start(*manage(new Button("About Sineshaper")));
  
  pack_start(*knob_vbox);
  pack_start(*preset_vbox);
}


Widget* SineshaperWidget::init_tuning_controls() {
  Frame* frame = manage(new Frame);
  frame->set_label("Tuning");
  frame->set_shadow_type(SHADOW_IN);
  return frame;
}


Widget* SineshaperWidget::init_osc2_controls() {
  Frame* frame = manage(new Frame);
  frame->set_label("Oscillator 2");
  frame->set_shadow_type(SHADOW_IN);
  return frame;
}


Widget* SineshaperWidget::init_vibrato_controls() {
  Frame* frame = manage(new Frame);
  frame->set_label("Vibrato");
  frame->set_shadow_type(SHADOW_IN);
  return frame;
}


Widget* SineshaperWidget::init_portamento_controls() {
  Frame* frame = manage(new Frame);
  frame->set_label("Portamento");
  frame->set_shadow_type(SHADOW_IN);
  return frame;
}


Widget* SineshaperWidget::init_tremolo_controls() {
  Frame* frame = manage(new Frame);
  frame->set_label("Tremolo");
  frame->set_shadow_type(SHADOW_IN);
  return frame;
}


Widget* SineshaperWidget::init_envelope_controls() {
  Frame* frame = manage(new Frame);
  frame->set_label("Envelope");
  frame->set_shadow_type(SHADOW_IN);
  return frame;
}


Widget* SineshaperWidget::init_amp_controls() {
  Frame* frame = manage(new Frame);
  frame->set_label("Amp");
  frame->set_shadow_type(SHADOW_IN);
  return frame;
}


Widget* SineshaperWidget::init_delay_controls() {
  Frame* frame = manage(new Frame);
  frame->set_label("Delay");
  frame->set_shadow_type(SHADOW_IN);
  return frame;
}


Widget* SineshaperWidget::init_shaper_controls() {
  Frame* frame = manage(new Frame);
  frame->set_label("Shaper");
  frame->set_shadow_type(SHADOW_IN);
  return frame;
}


Widget* SineshaperWidget::init_preset_list() {
  Frame* frame = manage(new Frame);
  frame->set_label("Presets");
  frame->set_shadow_type(SHADOW_NONE);
  return frame;
}



