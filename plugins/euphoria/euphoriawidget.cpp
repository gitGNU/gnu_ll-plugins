/****************************************************************************
    
    euphoriawidget.cpp - A GUI for the Euphoria LV2 synth
    
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

#include <cassert>
#include <iostream>

#include "euphoriawidget.hpp"
#include "vgknob.hpp"
#include "euphoria.peg"
#include "pdeditor.hpp"


using namespace Gtk;
using namespace std;
using namespace sigc;


void moo(Allocation& a, Widget* w) {
  w->set_size_request(-1, a.get_height());
}


EuphoriaWidget::EuphoriaWidget()
  : VBox(false, 6),
    m_program_store(ListStore::create(m_program_columns)),
    m_adj(e_n_ports, 0) {
  
  set_border_width(6);
  
  // initialise voice controls
  Frame* voice_frame = manage(new Frame("<b>Voice</b>"));
  dynamic_cast<Label*>(voice_frame->get_label_widget())->set_use_markup(true);
  voice_frame->set_shadow_type(SHADOW_IN);
  voice_frame->add(init_voice_controls());
  
  // initialise effect controls
  Frame* effects_frame = manage(new Frame("<b>Effects</b>"));
  dynamic_cast<Label*>(effects_frame->get_label_widget())->set_use_markup(true);
  effects_frame->set_shadow_type(SHADOW_IN);
  effects_frame->add(init_fx_controls());
  
  // initialise program list
  Frame* prog_frame = manage(new Frame("<b>Programs</b>"));
  dynamic_cast<Label*>(prog_frame->get_label_widget())->set_use_markup(true);
  prog_frame->set_shadow_type(SHADOW_NONE);
  prog_frame->add(init_program_controls());
  
  // main layout
  HBox* main_hbox = manage(new HBox(false, 6));
  VBox* main_vbox = manage(new VBox(false, 6));
  pack_start(*voice_frame, false, false);
  pack_start(*main_hbox);
  main_hbox->pack_start(*main_vbox, false, false);
  main_hbox->pack_start(*prog_frame);
  main_vbox->pack_start(*effects_frame, false, false);
  main_vbox->pack_start(init_global_controls(), false, false);
  main_vbox->pack_end(init_bottom_buttons(), false, false);
  main_vbox->pack_end(*manage(new HSeparator), false, false);
  
  // connect signals
  slot<void, const std::string&, const std::string&> configure = 
    signal_configure;
  m_shaper.signal_apply.
    connect(compose(bind<0>(configure, "shape"), 
                    mem_fun(m_shaper, &ShaperEditor::get_string)));
  m_shp_amount_env.signal_apply.
    connect(compose(bind<0>(configure, "shp_amount_env"), 
                    mem_fun(m_shp_amount_env, &EnvelopeEditor::get_string)));
  m_shp_amp_env.signal_apply.
    connect(compose(bind<0>(configure, "shp_amp_env"), 
                    mem_fun(m_shp_amp_env, &EnvelopeEditor::get_string)));
  m_phase.signal_apply.
    connect(compose(bind<0>(configure, "phase"), 
                    mem_fun(m_phase, &PDEditor::get_string)));
  m_pd_dist_env.signal_apply.
    connect(compose(bind<0>(configure, "pd_dist_env"), 
                    mem_fun(m_pd_dist_env, &EnvelopeEditor::get_string)));
  m_pd_amp_env.signal_apply.
    connect(compose(bind<0>(configure, "pd_amp_env"), 
                    mem_fun(m_pd_amp_env, &EnvelopeEditor::get_string)));
  
  show_all();
}


void EuphoriaWidget::add_program(int number, const string& name) {
  ListStore::iterator iter = m_program_store->children().begin();
  for ( ; iter != m_program_store->children().end(); ++iter) {
    if ((*iter)[m_program_columns.number] == number) {
      (*iter)[m_program_columns.name] = name;
      return;
    }
    if ((*iter)[m_program_columns.number] > number)
      break;
  }
  ListStore::iterator witer = m_program_store->insert(iter);
  (*witer)[m_program_columns.number] = number;
  (*witer)[m_program_columns.name] = name;
}


void EuphoriaWidget::remove_program(int number) {
  ListStore::iterator iter = m_program_store->children().begin();
  for ( ; iter != m_program_store->children().end(); ++iter) {
    if ((*iter)[m_program_columns.number] == number) {
      m_program_store->erase(iter);
      break;
    }
  }
}


void EuphoriaWidget::clear_programs() {
  m_program_store->clear();
}


void EuphoriaWidget::set_program(int number) {
  ListStore::iterator iter = m_program_store->children().begin();
  for ( ; iter != m_program_store->children().end(); ++iter) {
    if ((*iter)[m_program_columns.number] == number) {
      m_program_view.get_selection()->select(iter);
      break;
    }
  }
}


void EuphoriaWidget::set_control(uint32_t port, float value) {
  if (port < m_adj.size() && m_adj[port])
    m_adj[port]->set_value(value);
}


void EuphoriaWidget::configure(const std::string& key, 
                               const std::string& value) {
  if (key == "phase")
    m_phase.set_string(value);
  else if (key == "phase_env")
    m_pd_dist_env.set_string(value);
  else if (key == "shape")
    m_shaper.set_string(value);
  else if (key == "shape_env")
    m_shp_amount_env.set_string(value);
}


VBox* EuphoriaWidget::create_knob(const string& label, int port,
                                  float red, float green, float blue) {
  VBox* box = manage(new VBox(false, 1));
  VGKnob* knob = manage(new VGKnob(e_ports[port].min, e_ports[port].max,
                                   e_ports[port].default_value, 
                                   red, green, blue, e_ports[port].integer,
                                   e_ports[port].logarithmic));
  Adjustment& adj = knob->get_adjustment();
  adj.signal_value_changed().
    connect(compose(bind<0>(signal_control_changed, port),
                    mem_fun(adj, &Adjustment::get_value)));
  assert(m_adj[port] == 0);
  m_adj[port] = &adj;
  box->pack_start(*knob);
  Label* text = manage(new Label(string("<small>") + label + "</small>"));
  text->set_use_markup(true);
  box->pack_start(*text);
  return box;
}


void EuphoriaWidget::program_selection_changed() {
  TreeModel::iterator iter = m_program_view.get_selection()->get_selected();
  if (iter == m_program_store->children().end())
    signal_program_selected(-1);
  signal_program_selected((*iter)[m_program_columns.number]);
}


Widget& EuphoriaWidget::init_pd_controls() {

  Table* phase_tbl = manage(new Table(1, 4));
  phase_tbl->set_border_width(6);
  phase_tbl->set_spacings(6);
  
  VBox* ed_box = manage(new VBox);
  Notebook* phase_ed_nbk = manage(new Notebook);
  ScrolledWindow* ed_scw = manage(new ScrolledWindow);
  phase_ed_nbk->append_page(*ed_scw, "Phase");
  ed_scw->set_policy(POLICY_NEVER, POLICY_NEVER);
  ed_scw->set_shadow_type(SHADOW_IN);
  ed_scw->add(m_phase);
  ed_box->pack_start(*phase_ed_nbk, false, false);
  HBox* ed_hbox = manage(new HBox(true));
  ed_box->pack_start(*ed_hbox, false, false);
  phase_tbl->attach(*ed_box, 0, 1, 0, 1, 
                    AttachOptions(0), AttachOptions(0));
  Table* knob1_tbl = manage(new Table(2, 2));
  phase_tbl->attach(*knob1_tbl, 1, 2, 0, 1, AttachOptions(0), AttachOptions(0));
  knob1_tbl->set_spacings(3);
  knob1_tbl->attach(*create_knob("P.Dist", e_phase_dist), 
                    0, 1, 0, 1, AttachOptions(0));
  knob1_tbl->attach(*create_knob("Env", e_pd_env_sens),
                    0, 1, 1, 2, AttachOptions(0));
  knob1_tbl->attach(*create_knob("Vel", e_pd_vel_sens)
                    , 1, 2, 0, 1, AttachOptions(0));
  
  HScrollbar* env_bar = 
    manage(new HScrollbar(m_pd_dist_env.get_adjustment()));
  for (int i = 0; i < 4; ++i) {
    ToggleButton* btn = manage(new ToggleButton);
    env_bar->signal_size_allocate().connect(bind(&moo, btn));
    ed_hbox->pack_start(*btn);
  }
  
  VBox* env_box = manage(new VBox(false, 0));
  Notebook* phase_nbk = manage(new Notebook);
  env_box->pack_start(*phase_nbk);
  phase_tbl->attach(*env_box, 2, 3, 0, 1, 
                    FILL|EXPAND, AttachOptions(0));
  env_box->pack_start(*env_bar);

  ScrolledWindow* env_scw = manage(new ScrolledWindow());
  phase_nbk->append_page(*env_scw, "Phase distortion");
  env_scw->set_shadow_type(SHADOW_IN);
  env_scw->set_policy(POLICY_NEVER, POLICY_NEVER);
  env_scw->add(m_pd_dist_env);
  
  ScrolledWindow* env_amp_scw = manage(new ScrolledWindow());
  phase_nbk->append_page(*env_amp_scw, "Gain");
  env_amp_scw->set_shadow_type(SHADOW_IN);
  env_amp_scw->set_policy(POLICY_NEVER, POLICY_NEVER);
  env_amp_scw->add(m_pd_amp_env);

  Table* knob2_tbl = manage(new Table(2, 2));
  phase_tbl->attach(*knob2_tbl, 3, 4, 0, 1, AttachOptions(0), AttachOptions(0));
  knob2_tbl->set_spacings(3);
  knob2_tbl->attach(*create_knob("Att", e_pd_attack, 1, 0, 0), 
                    0, 1, 0, 1, AttachOptions(0));
  knob2_tbl->attach(*create_knob("Dec", e_pd_decay, 1, 1, 0), 
                    1, 2, 0, 1, AttachOptions(0));
  knob2_tbl->attach(*create_knob("Sus", e_pd_sustain, 0.3, 1, 0.3), 
                    0, 1, 1, 2, AttachOptions(0));
  knob2_tbl->attach(*create_knob("Rel", e_pd_release), 
                    1, 2, 1, 2, AttachOptions(0));
  
  return *phase_tbl;
}


Widget& EuphoriaWidget::init_shp_controls() {

  Table* shape_tbl = manage(new Table(1, 4));
  shape_tbl->set_border_width(6);
  shape_tbl->set_spacings(6);
  VBox* ed_box = manage(new VBox);
  m_shaper.set_size_request(91, 91);
  Notebook* shape_ed_nbk = manage(new Notebook);
  ScrolledWindow* ed_scw = manage(new ScrolledWindow);
  shape_ed_nbk->append_page(*ed_scw, "Shaper");
  ed_scw->set_policy(POLICY_NEVER, POLICY_NEVER);
  ed_scw->set_shadow_type(SHADOW_IN);
  ed_scw->add(m_shaper);
  ed_box->pack_start(*shape_ed_nbk, false, false);

  HBox* ed_hbox = manage(new HBox(true));
  ed_box->pack_start(*ed_hbox, false, false);
  shape_tbl->attach(*ed_box, 0, 1, 0, 1, 
                    AttachOptions(0), AttachOptions(0));
  
  Table* knob1_tbl = manage(new Table(2, 2));
  shape_tbl->attach(*knob1_tbl, 1, 2, 0, 1, AttachOptions(0), AttachOptions(0));
  knob1_tbl->set_spacings(3);
  knob1_tbl->attach(*create_knob("Shape", e_shape), 
                    0, 1, 0, 1, AttachOptions(0));
  knob1_tbl->attach(*create_knob("Env", e_shape_env_sens),
                    0, 1, 1, 2, AttachOptions(0));
  knob1_tbl->attach(*create_knob("Vel", e_shape_vel_sens),
                    1, 2, 0, 1, AttachOptions(0));
  knob1_tbl->attach(*create_knob("Smooth", e_shape_smoothness),
                    1, 2, 1, 2, AttachOptions(0));
  
  VBox* env_box = manage(new VBox(false, 0));
  Notebook* shape_env_nb = manage(new Notebook);
  HScrollbar* env_scb = manage(new HScrollbar(m_shp_amount_env.
                                              get_adjustment()));
  for (int i = 0; i < 4; ++i) {
    ToggleButton* btn = manage(new ToggleButton);
    env_scb->signal_size_allocate().connect(bind(&moo, btn));
    ed_hbox->pack_start(*btn);
  }
  env_box->pack_start(*shape_env_nb);
  
  ScrolledWindow* env_scw = manage(new ScrolledWindow());
  shape_env_nb->append_page(*env_scw, "Shaping amount");
  env_scw->set_shadow_type(SHADOW_IN);
  env_scw->set_policy(POLICY_NEVER, POLICY_NEVER);
  env_scw->add(m_shp_amount_env);

  ScrolledWindow* env_amp_scw = manage(new ScrolledWindow());
  shape_env_nb->append_page(*env_amp_scw, "Gain");
  env_amp_scw->set_shadow_type(SHADOW_IN);
  env_amp_scw->set_policy(POLICY_NEVER, POLICY_NEVER);
  env_amp_scw->add(m_shp_amp_env);



  env_box->pack_start(*env_scb);
  shape_tbl->attach(*env_box, 2, 3, 0, 1, 
                    FILL|EXPAND, AttachOptions(0));

  Table* knob2_tbl = manage(new Table(2, 2));
  shape_tbl->attach(*knob2_tbl, 3, 4, 0, 1, AttachOptions(0), AttachOptions(0));
  knob2_tbl->set_spacings(3);
  knob2_tbl->attach(*create_knob("Att", e_shape_attack, 1, 0, 0), 
                    0, 1, 0, 1, AttachOptions(0));
  knob2_tbl->attach(*create_knob("Dec", e_shape_decay, 1, 1, 0),
                    1, 2, 0, 1, AttachOptions(0));
  knob2_tbl->attach(*create_knob("Sus", e_shape_sustain, 0.3, 1, 0.3),
                    0, 1, 1, 2, AttachOptions(0));
  knob2_tbl->attach(*create_knob("Rel", e_shape_release),
                    1, 2, 1, 2, AttachOptions(0));

  return *shape_tbl;
}


Widget& EuphoriaWidget::init_fx_controls() {

  HBox* effect_hbox = manage(new HBox(true, 12));
  effect_hbox->set_border_width(6);
  
  // distortion
  Table* dist_tbl = manage(new Table(3, 2));
  effect_hbox->pack_start(*dist_tbl);
  dist_tbl->set_spacings(3);
  dist_tbl->attach(*manage(new ToggleButton("Distortion")), 0, 2, 0, 1);
  dist_tbl->attach(*create_knob("Drive", e_dist_drive, 1, 0, 0),
                   0, 1, 1, 2, EXPAND);
  dist_tbl->attach(*create_knob("Set", e_dist_set, 1, 0, 0),
                   1, 2, 1, 2, EXPAND);
  dist_tbl->attach(*create_knob("Tone", e_dist_tone, 1, 0, 0), 
                   0, 1, 2, 3, EXPAND);
  dist_tbl->attach(*create_knob("Mix", e_dist_mix, 1, 0, 0), 
                   1, 2, 2, 3, EXPAND);
  
  // chorus
  Table* chorus_tbl = manage(new Table(3, 2));
  effect_hbox->pack_start(*chorus_tbl);
  chorus_tbl->set_spacings(3);
  chorus_tbl->attach(*manage(new ToggleButton("Chorus")), 0, 2, 0, 1);
  chorus_tbl->attach(*create_knob("Freq", e_chorus_freq, 0.75, 0.1, 0.25),
                     0, 1, 1, 2, EXPAND);
  chorus_tbl->attach(*create_knob("Depth", e_chorus_depth, 0.75, 0.1, 0.25),
                     1, 2, 1, 2, EXPAND);
  chorus_tbl->attach(*create_knob("Delay", e_chorus_delay, 0.75, 0.1, 0.25),
                     0, 1, 2, 3, EXPAND);
  chorus_tbl->attach(*create_knob("Mix", e_chorus_mix, 0.75, 0.1, 0.25),
                     1, 2, 2, 3, EXPAND);
  
  // echo
  Table* echo_tbl = manage(new Table(3, 2));
  effect_hbox->pack_start(*echo_tbl);
  echo_tbl->set_spacings(3);
  echo_tbl->attach(*manage(new ToggleButton("Echo")), 0, 2, 0, 1);
  echo_tbl->attach(*create_knob("Delay", e_echo_delay, 0.25, 0.2, 0.75),
                   0, 1, 1, 2, EXPAND);
  echo_tbl->attach(*create_knob("Fdback", e_echo_feedback, 0.25, 0.2, 0.75),
                   1, 2, 1, 2, EXPAND);
  echo_tbl->attach(*create_knob("Pan", e_echo_pan, 0.25, 0.2, 0.75),
                   0, 1, 2, 3, EXPAND);
  echo_tbl->attach(*create_knob("Mix", e_echo_mix, 0.25, 0.2, 0.75),
                   1, 2, 2, 3, EXPAND);
  
  // reverb
  Table* reverb_tbl = manage(new Table(3, 2));
  effect_hbox->pack_start(*reverb_tbl);
  reverb_tbl->set_spacings(3);
  reverb_tbl->attach(*manage(new ToggleButton("Reverb")), 0, 2, 0, 1);
  reverb_tbl->attach(*create_knob("Time", e_reverb_time), 0, 1, 1, 2, EXPAND);
  reverb_tbl->attach(*create_knob("Room", e_reverb_room), 1, 2, 1, 2, EXPAND);
  reverb_tbl->attach(*create_knob("Damp", e_reverb_damping),
                     0, 1, 2, 3, EXPAND);
  reverb_tbl->attach(*create_knob("Mix", e_reverb_mix), 1, 2, 2, 3, EXPAND);
  
  return *effect_hbox;
}


Widget& EuphoriaWidget::init_voice_controls() {

  // voice controls
  VBox* voice_vbox = manage(new VBox(false, 6));
  voice_vbox->set_border_width(6);
  Notebook* voice_nbk = manage(new Notebook);
  voice_vbox->pack_start(*voice_nbk);

  voice_nbk->append_page(init_pd_controls(), "Phase distortion");

  voice_nbk->append_page(init_shp_controls(), "Waveshaping");


  voice_nbk->append_page(*manage(new HBox), "Markov synthesis");
  
  voice_vbox->pack_start(*manage(new HSeparator));
  
  HBox* voice_hbox = manage(new HBox(false, 6));
  voice_vbox->pack_start(*voice_hbox);
  Table* voice_knob_table = manage(new Table(2, 4));
  voice_knob_table->set_row_spacings(3);
  voice_knob_table->set_col_spacings(12);
  voice_hbox->pack_start(*voice_knob_table);
  
  Label* vibrato_lbl = manage(new Label("<small>Vibrato</small>"));
  vibrato_lbl->set_use_markup(true);
  voice_knob_table->attach(*vibrato_lbl, 0, 1, 0, 1, AttachOptions(0));
  HBox* vibrato_hbox = manage(new HBox(false, 3));
  voice_knob_table->attach(*vibrato_hbox, 0, 1, 1, 2, AttachOptions(0));
  vibrato_hbox->pack_start(*create_knob("Freq", e_vib_freq, 1.0, 0.5, 0));
  vibrato_hbox->pack_start(*create_knob("Depth", e_vib_depth, 1.0, 0.5, 0));
  
  Label* tremolo_lbl = manage(new Label("<small>Tremolo</small>"));
  tremolo_lbl->set_use_markup(true);
  voice_knob_table->attach(*tremolo_lbl, 1, 2, 0, 1, AttachOptions(0));
  HBox* tremolo_hbox = manage(new HBox(false, 3));
  voice_knob_table->attach(*tremolo_hbox, 1, 2, 1, 2, AttachOptions(0));
  tremolo_hbox->pack_start(*create_knob("Freq", e_trem_freq, 1, 0, 1));
  tremolo_hbox->pack_start(*create_knob("Depth", e_trem_depth, 1, 0, 1));

  Label* unison_lbl = manage(new Label("<small>Unison</small>"));
  unison_lbl->set_use_markup(true);
  voice_knob_table->attach(*unison_lbl, 2, 3, 0, 1, AttachOptions(0));
  HBox* unison_hbox = manage(new HBox(false, 3));
  voice_knob_table->attach(*unison_hbox, 2, 3, 1, 2, AttachOptions(0));
  unison_hbox->pack_start(*create_knob("Layers", e_unison_layers, 
                                       0.4, 0.4, 0.4));
  unison_hbox->pack_start(*create_knob("Spread", e_unison_spread, 
                                       0.4, 0.4, 0.4));

  Label* amp_lbl = manage(new Label("<small>Amp</small>"));
  amp_lbl->set_use_markup(true);
  voice_knob_table->attach(*amp_lbl, 3, 4, 0, 1, AttachOptions(0));
  HBox* amp_hbox = manage(new HBox(false, 3));
  voice_knob_table->attach(*amp_hbox, 3, 4, 1, 2, AttachOptions(0));
  amp_hbox->pack_start(*create_knob("Env", e_amp_env, 0, 1, 1));
  amp_hbox->pack_start(*create_knob("Vel", e_amp_vel_sens, 0, 1, 1));
  
  Table* note_tbl = manage(new Table(2, 3));
  note_tbl->set_spacings(3);
  voice_hbox->pack_end(*note_tbl, false, false);
  voice_hbox->pack_end(*manage(new VSeparator), false, false);
  note_tbl->attach(*manage(new ToggleButton("Poly")), 0, 1, 0, 1, 
                   FILL, AttachOptions(0));
  Label* voices_lbl = manage(new Label("<small>Number of Voices:</small>", 
                                       ALIGN_RIGHT));
  voices_lbl->set_use_markup(true);
  note_tbl->attach(*voices_lbl, 1, 2, 0, 1);
  SpinButton* voices_sbn = manage(new SpinButton);
  voices_sbn->set_increments(1, 10);
  voices_sbn->set_range(1, 10);
  voices_sbn->set_digits(0);
  note_tbl->attach(*voices_sbn, 2, 3, 0, 1, AttachOptions(0));
  note_tbl->attach(*manage(new ToggleButton("Tie")), 0, 1, 1, 2, 
                   FILL, AttachOptions(0));
  Label* slide_lbl = manage(new Label("<small>Slide speed:</small>", 
                                      ALIGN_RIGHT));
  slide_lbl->set_use_markup(true);
  note_tbl->attach(*slide_lbl, 1, 2, 1, 2);
  SpinButton* slide_sbn = manage(new SpinButton);
  slide_sbn->set_increments(0.01, 0.1);
  slide_sbn->set_range(0, 1);
  slide_sbn->set_digits(2);
  note_tbl->attach(*slide_sbn, 2, 3, 1, 2, AttachOptions(0));
  
  return *voice_vbox;
}


Widget& EuphoriaWidget::init_program_controls() {
  ScrolledWindow* prog_scw = manage(new ScrolledWindow);
  prog_scw->set_size_request(100, -1);
  prog_scw->set_shadow_type(SHADOW_IN);
  prog_scw->add(m_program_view);
  prog_scw->set_policy(POLICY_NEVER, POLICY_ALWAYS);
  m_program_view.set_model(m_program_store);
  m_program_view.append_column("Number", m_program_columns.number);
  m_program_view.append_column("Name", m_program_columns.name);
  m_program_view.set_reorderable(false);
  m_program_view.set_headers_visible(false);
  m_program_view.set_rules_hint(true);
  m_program_view.get_selection()->signal_changed().
    connect(mem_fun(*this, &EuphoriaWidget::program_selection_changed));
  
  return *prog_scw;
}


Widget& EuphoriaWidget::init_global_controls() {
  HBox* global_controls_box = manage(new HBox(false, 6));
  global_controls_box->pack_start(*create_knob("Gain", e_gain), true, false);
  global_controls_box->pack_start(*create_knob("Octave", e_octave), true, false);
  global_controls_box->pack_start(*create_knob("Tune", e_tune), true, false);
  global_controls_box->pack_start(*create_knob("Pan", e_pan), true, false);
  return *global_controls_box;
}


Widget& EuphoriaWidget::init_bottom_buttons() {
  HBox* bottom_box = manage(new HBox(false, 6));
  Button* save_prog_button = manage(new Button("Save program"));
  Image* save_image = manage(new Image);
  save_image->set(Stock::SAVE, ICON_SIZE_BUTTON);
  save_prog_button->set_image(*save_image);
  bottom_box->pack_start(*save_prog_button, false, false);
  Button* about_button = manage(new Button("About Euphoria"));
  Image* about_image = manage(new Image);
  about_image->set(Stock::ABOUT, ICON_SIZE_BUTTON);
  about_button->set_image(*about_image);
  bottom_box->pack_end(*about_button, false, false);
  
  return *bottom_box;
}
