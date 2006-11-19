#include <iostream>

#include "euphoriawidget.hpp"
#include "vgknob.hpp"
#include "euphoria.peg"
#include "pdeditor.hpp"


using namespace Gtk;
using namespace std;


void moo(Allocation& a, Widget* w) {
  w->set_size_request(-1, a.get_height());
}


EuphoriaWidget::EuphoriaWidget(LV2UIClient& lv2)
  : VBox(false, 6),
    m_program_store(ListStore::create(m_program_columns)) {
  
  set_border_width(6);
  
  Frame* voiceFrame = manage(new Frame("<b>Voice</b>"));
  dynamic_cast<Label*>(voiceFrame->get_label_widget())->set_use_markup(true);
  voiceFrame->set_shadow_type(SHADOW_IN);
  Frame* effectsFrame = manage(new Frame("<b>Effects</b>"));
  dynamic_cast<Label*>(effectsFrame->get_label_widget())->set_use_markup(true);
  effectsFrame->set_shadow_type(SHADOW_IN);
  HBox* globalControlsBox = manage(new HBox(false, 6));
  ScrolledWindow* progScw = manage(new ScrolledWindow);
  progScw->set_size_request(100, -1);
  progScw->set_shadow_type(SHADOW_IN);
  progScw->add(m_program_view);
  progScw->set_policy(POLICY_NEVER, POLICY_ALWAYS);
  Frame* progFrame = manage(new Frame("<b>Programs</b>"));
  dynamic_cast<Label*>(progFrame->get_label_widget())->set_use_markup(true);
  progFrame->add(*progScw);
  progFrame->set_shadow_type(SHADOW_NONE);
  HBox* bottomBox = manage(new HBox(false, 6));
  
  HBox* mainHBox = manage(new HBox(false, 6));
  VBox* mainVBox = manage(new VBox(false, 6));
  pack_start(*voiceFrame, false, false);
  pack_start(*mainHBox);
  mainHBox->pack_start(*mainVBox, false, false);
  mainHBox->pack_start(*progFrame);
  mainVBox->pack_start(*effectsFrame, false, false);
  mainVBox->pack_start(*globalControlsBox, false, false);
  mainVBox->pack_end(*bottomBox, false, false);
  mainVBox->pack_end(*manage(new HSeparator), false, false);
  
  // voice controls
  VBox* voiceVBox = manage(new VBox(false, 6));
  voiceVBox->set_border_width(6);
  voiceFrame->add(*voiceVBox);
  Table* voiceTable = manage(new Table(2, 4));
  voiceVBox->pack_start(*voiceTable);
  voiceTable->set_spacings(6);
  
  VBox* phaseEBox = manage(new VBox);
  ScrolledWindow* phaseEScrw = manage(new ScrolledWindow);
  phaseEScrw->set_policy(POLICY_NEVER, POLICY_NEVER);
  phaseEScrw->set_shadow_type(SHADOW_IN);
  phaseEScrw->add(m_phase);
  phaseEBox->pack_start(*phaseEScrw);
  phaseEBox->pack_start(*phaseEScrw, false, false);
  HBox* phaseEHBox = manage(new HBox(true));
  phaseEBox->pack_start(*phaseEHBox, false, false);
  voiceTable->attach(*phaseEBox, 0, 1, 0, 1, 
                     AttachOptions(0), AttachOptions(0));
  Table* phaseT1 = manage(new Table(2, 2));
  voiceTable->attach(*phaseT1, 1, 2, 0, 1, AttachOptions(0), AttachOptions(0));
  phaseT1->set_spacings(3);
  phaseT1->attach(*create_knob(lv2, "P.Dist", e_phase_dist), 
                  0, 1, 0, 1, AttachOptions(0));
  phaseT1->attach(*create_knob(lv2, "Env", e_pd_env_sens),
                  0, 1, 1, 2, AttachOptions(0));
  phaseT1->attach(*create_knob(lv2, "Vel", e_pd_vel_sens)
                  , 1, 2, 0, 1, AttachOptions(0));
  VBox* phaseEnvBox = manage(new VBox(false, 0));
  ScrolledWindow* phaseScrw = manage(new ScrolledWindow());
  HScrollbar* phaseBar = 
    manage(new HScrollbar(m_phase_env.get_adjustment()));
  for (int i = 0; i < 4; ++i) {
    ToggleButton* phaseBtn = manage(new ToggleButton);
    phaseBar->signal_size_allocate().connect(bind(&moo, phaseBtn));
    phaseEHBox->pack_start(*phaseBtn);
  }
  
  phaseEnvBox->pack_start(*phaseScrw);
  phaseEnvBox->pack_start(*phaseBar);
  phaseScrw->set_shadow_type(SHADOW_IN);
  phaseScrw->set_policy(POLICY_NEVER, POLICY_NEVER);
  phaseScrw->add(m_phase_env);
  voiceTable->attach(*phaseEnvBox, 2, 3, 0, 1, 
                     FILL|EXPAND, AttachOptions(0));
  Table* phaseT2 = manage(new Table(2, 2));
  voiceTable->attach(*phaseT2, 3, 4, 0, 1, AttachOptions(0), AttachOptions(0));
  phaseT2->set_spacings(3);
  phaseT2->attach(*create_knob(lv2, "Att", e_pd_attack, 1, 0, 0), 
                  0, 1, 0, 1, AttachOptions(0));
  phaseT2->attach(*create_knob(lv2, "Dec", e_pd_decay, 1, 1, 0), 
                  1, 2, 0, 1, AttachOptions(0));
  phaseT2->attach(*create_knob(lv2, "Sus", e_pd_sustain, 0.3, 1, 0.3), 
                  0, 1, 1, 2, AttachOptions(0));
  phaseT2->attach(*create_knob(lv2, "Rel", e_pd_release), 
                  1, 2, 1, 2, AttachOptions(0));
  
  VBox* shapeEBox = manage(new VBox);
  m_shaper.set_size_request(91, 91);
  ScrolledWindow* shapeEScrw = manage(new ScrolledWindow);
  shapeEScrw->set_policy(POLICY_NEVER, POLICY_NEVER);
  shapeEScrw->set_shadow_type(SHADOW_IN);
  shapeEScrw->add(m_shaper);
  shapeEBox->pack_start(*shapeEScrw);
  shapeEBox->pack_start(*shapeEScrw, false, false);
  HBox* shapeEHBox = manage(new HBox(true));
  shapeEBox->pack_start(*shapeEHBox, false, false);
  voiceTable->attach(*shapeEBox, 0, 1, 1, 2, 
                     AttachOptions(0), AttachOptions(0));
  Table* shapeT1 = manage(new Table(2, 2));
  voiceTable->attach(*shapeT1, 1, 2, 1, 2, AttachOptions(0), AttachOptions(0));
  shapeT1->set_spacings(3);
  shapeT1->attach(*create_knob(lv2, "Shape", e_shape), 
                  0, 1, 0, 1, AttachOptions(0));
  shapeT1->attach(*create_knob(lv2, "Env", e_shape_env_sens),
                  0, 1, 1, 2, AttachOptions(0));
  shapeT1->attach(*create_knob(lv2, "Vel", e_shape_vel_sens),
                  1, 2, 0, 1, AttachOptions(0));
  shapeT1->attach(*create_knob(lv2, "Smooth", e_shape_smoothness),
                  1, 2, 1, 2, AttachOptions(0));
  VBox* shapeEnvBox = manage(new VBox(false, 0));
  ScrolledWindow* shapeScrw = manage(new ScrolledWindow());
  HScrollbar* shapeBar = manage(new HScrollbar(m_shape_env.get_adjustment()));
  for (int i = 0; i < 4; ++i) {
    ToggleButton* shapeBtn = manage(new ToggleButton);
    shapeBar->signal_size_allocate().connect(bind(&moo, shapeBtn));
    shapeEHBox->pack_start(*shapeBtn);
  }

  shapeEnvBox->pack_start(*shapeScrw);
  shapeEnvBox->pack_start(*shapeBar);
  shapeScrw->set_shadow_type(SHADOW_IN);
  shapeScrw->set_policy(POLICY_NEVER, POLICY_NEVER);
  shapeScrw->add(m_shape_env);
  voiceTable->attach(*shapeEnvBox, 2, 3, 1, 2, 
                     FILL|EXPAND, AttachOptions(0));
  Table* shapeT2 = manage(new Table(2, 2));
  voiceTable->attach(*shapeT2, 3, 4, 1, 2, AttachOptions(0), AttachOptions(0));
  shapeT2->set_spacings(3);
  shapeT2->attach(*create_knob(lv2, "Att", e_shape_attack, 1, 0, 0), 
                  0, 1, 0, 1, AttachOptions(0));
  shapeT2->attach(*create_knob(lv2, "Dec", e_shape_decay, 1, 1, 0),
                  1, 2, 0, 1, AttachOptions(0));
  shapeT2->attach(*create_knob(lv2, "Sus", e_shape_sustain, 0.3, 1, 0.3),
                  0, 1, 1, 2, AttachOptions(0));
  shapeT2->attach(*create_knob(lv2, "Rel", e_shape_release),
                  1, 2, 1, 2, AttachOptions(0));
  
  voiceVBox->pack_start(*manage(new HSeparator));
  
  HBox* voiceHBox = manage(new HBox(false, 6));
  voiceVBox->pack_start(*voiceHBox);
  Table* voiceKnobTable = manage(new Table(2, 4));
  voiceKnobTable->set_row_spacings(3);
  voiceKnobTable->set_col_spacings(12);
  voiceHBox->pack_start(*voiceKnobTable);
  
  Label* vibratoLabel = manage(new Label("<small>Vibrato</small>"));
  vibratoLabel->set_use_markup(true);
  voiceKnobTable->attach(*vibratoLabel, 0, 1, 0, 1, AttachOptions(0));
  HBox* vibratoHBox = manage(new HBox(false, 3));
  voiceKnobTable->attach(*vibratoHBox, 0, 1, 1, 2, AttachOptions(0));
  vibratoHBox->pack_start(*create_knob(lv2, "Freq", e_vib_freq, 1.0, 0.5, 0));
  vibratoHBox->pack_start(*create_knob(lv2, "Depth", e_vib_depth, 1.0, 0.5, 0));
  
  Label* tremoloLabel = manage(new Label("<small>Tremolo</small>"));
  tremoloLabel->set_use_markup(true);
  voiceKnobTable->attach(*tremoloLabel, 1, 2, 0, 1, AttachOptions(0));
  HBox* tremoloHBox = manage(new HBox(false, 3));
  voiceKnobTable->attach(*tremoloHBox, 1, 2, 1, 2, AttachOptions(0));
  tremoloHBox->pack_start(*create_knob(lv2, "Freq", e_trem_freq, 1, 0, 1));
  tremoloHBox->pack_start(*create_knob(lv2, "Depth", e_trem_depth, 1, 0, 1));

  Label* unisonLabel = manage(new Label("<small>Unison</small>"));
  unisonLabel->set_use_markup(true);
  voiceKnobTable->attach(*unisonLabel, 2, 3, 0, 1, AttachOptions(0));
  HBox* unisonHBox = manage(new HBox(false, 3));
  voiceKnobTable->attach(*unisonHBox, 2, 3, 1, 2, AttachOptions(0));
  unisonHBox->pack_start(*create_knob(lv2, "Layers", e_unison_layers, 
                                      0.4, 0.4, 0.4));
  unisonHBox->pack_start(*create_knob(lv2, "Spread", e_unison_spread, 
                                      0.4, 0.4, 0.4));

  Label* ampLabel = manage(new Label("<small>Amp</small>"));
  ampLabel->set_use_markup(true);
  voiceKnobTable->attach(*ampLabel, 3, 4, 0, 1, AttachOptions(0));
  HBox* ampHBox = manage(new HBox(false, 3));
  voiceKnobTable->attach(*ampHBox, 3, 4, 1, 2, AttachOptions(0));
  ampHBox->pack_start(*create_knob(lv2, "Env", e_amp_env, 0, 1, 1));
  ampHBox->pack_start(*create_knob(lv2, "Vel", e_amp_vel_sens, 0, 1, 1));
  
  Table* noteTable = manage(new Table(2, 3));
  noteTable->set_spacings(3);
  voiceHBox->pack_end(*noteTable, false, false);
  voiceHBox->pack_end(*manage(new VSeparator), false, false);
  noteTable->attach(*manage(new ToggleButton("Poly")), 0, 1, 0, 1, 
                    FILL, AttachOptions(0));
  Label* voicesLabel = manage(new Label("<small>Number of Voices:</small>", 
                                        ALIGN_RIGHT));
  voicesLabel->set_use_markup(true);
  noteTable->attach(*voicesLabel, 1, 2, 0, 1);
  SpinButton* voicesSpin = manage(new SpinButton);
  voicesSpin->set_increments(1, 10);
  voicesSpin->set_range(1, 10);
  voicesSpin->set_digits(0);
  noteTable->attach(*voicesSpin, 2, 3, 0, 1, AttachOptions(0));
  noteTable->attach(*manage(new ToggleButton("Tie")), 0, 1, 1, 2, 
                    FILL, AttachOptions(0));
  Label* slideLabel = manage(new Label("<small>Slide speed:</small>", 
                                       ALIGN_RIGHT));
  slideLabel->set_use_markup(true);
  noteTable->attach(*slideLabel, 1, 2, 1, 2);
  SpinButton* slideSpin = manage(new SpinButton);
  slideSpin->set_increments(0.01, 0.1);
  slideSpin->set_range(0, 1);
  slideSpin->set_digits(2);
  noteTable->attach(*slideSpin, 2, 3, 1, 2, AttachOptions(0));
  
  // effect controls
  HBox* effectHBox = manage(new HBox(true, 12));
  effectHBox->set_border_width(6);
  effectsFrame->add(*effectHBox);
  
  // distortion
  Table* distTable = manage(new Table(3, 2));
  effectHBox->pack_start(*distTable);
  distTable->set_spacings(3);
  distTable->attach(*manage(new ToggleButton("Distortion")), 0, 2, 0, 1);
  distTable->attach(*create_knob(lv2, "Drive", e_dist_drive, 1, 0, 0),
                    0, 1, 1, 2, EXPAND);
  distTable->attach(*create_knob(lv2, "Set", e_dist_set, 1, 0, 0),
                    1, 2, 1, 2, EXPAND);
  distTable->attach(*create_knob(lv2, "Tone", e_dist_tone, 1, 0, 0), 
                    0, 1, 2, 3, EXPAND);
  distTable->attach(*create_knob(lv2, "Mix", e_dist_mix, 1, 0, 0), 
                    1, 2, 2, 3, EXPAND);
  
  // chorus
  Table* chorusTable = manage(new Table(3, 2));
  effectHBox->pack_start(*chorusTable);
  chorusTable->set_spacings(3);
  chorusTable->attach(*manage(new ToggleButton("Chorus")), 0, 2, 0, 1);
  chorusTable->attach(*create_knob(lv2, "Freq", e_chorus_freq, 0.75, 0.1, 0.25),
                      0, 1, 1, 2, EXPAND);
  chorusTable->attach(*create_knob(lv2, "Depth", e_chorus_depth, 0.75, 0.1, 0.25),
                      1, 2, 1, 2, EXPAND);
  chorusTable->attach(*create_knob(lv2, "Delay", e_chorus_delay, 0.75, 0.1, 0.25),
                      0, 1, 2, 3, EXPAND);
  chorusTable->attach(*create_knob(lv2, "Mix", e_chorus_mix, 0.75, 0.1, 0.25),
                      1, 2, 2, 3, EXPAND);
  
  // echo
  Table* echoTable = manage(new Table(3, 2));
  effectHBox->pack_start(*echoTable);
  echoTable->set_spacings(3);
  echoTable->attach(*manage(new ToggleButton("Echo")), 0, 2, 0, 1);
  echoTable->attach(*create_knob(lv2, "Delay", e_echo_delay, 0.25, 0.2, 0.75),
                    0, 1, 1, 2, EXPAND);
  echoTable->attach(*create_knob(lv2, "Fdback", e_echo_feedback, 0.25, 0.2, 0.75),
                    1, 2, 1, 2, EXPAND);
  echoTable->attach(*create_knob(lv2, "Pan", e_echo_pan, 0.25, 0.2, 0.75),
                    0, 1, 2, 3, EXPAND);
  echoTable->attach(*create_knob(lv2, "Mix", e_echo_mix, 0.25, 0.2, 0.75),
                    1, 2, 2, 3, EXPAND);
  
  // reverb
  Table* reverbTable = manage(new Table(3, 2));
  effectHBox->pack_start(*reverbTable);
  reverbTable->set_spacings(3);
  reverbTable->attach(*manage(new ToggleButton("Reverb")), 0, 2, 0, 1);
  reverbTable->attach(*create_knob(lv2, "Time", e_reverb_time), 0, 1, 1, 2, EXPAND);
  reverbTable->attach(*create_knob(lv2, "Room", e_reverb_room), 1, 2, 1, 2, EXPAND);
  reverbTable->attach(*create_knob(lv2, "Damp", e_reverb_damping),
                      0, 1, 2, 3, EXPAND);
  reverbTable->attach(*create_knob(lv2, "Mix", e_reverb_mix), 1, 2, 2, 3, EXPAND);
  
  // global controls
  globalControlsBox->pack_start(*create_knob(lv2, "Gain", e_gain), true, false);
  globalControlsBox->pack_start(*create_knob(lv2, "Octave", e_octave), true, false);
  globalControlsBox->pack_start(*create_knob(lv2, "Tune", e_tune), true, false);
  globalControlsBox->pack_start(*create_knob(lv2, "Pan", e_pan), true, false);
  
  // bottom buttons
  Button* saveProgButton = manage(new Button("Save program"));
  Image* saveImage = manage(new Image);
  saveImage->set(Stock::SAVE, ICON_SIZE_BUTTON);
  saveProgButton->set_image(*saveImage);
  bottomBox->pack_start(*saveProgButton, false, false);
  Button* aboutButton = manage(new Button("About Euphoria"));
  Image* aboutImage = manage(new Image);
  aboutImage->set(Stock::ABOUT, ICON_SIZE_BUTTON);
  aboutButton->set_image(*aboutImage);
  bottomBox->pack_end(*aboutButton, false, false);
  
  // program list
  m_program_view.set_model(m_program_store);
  m_program_view.append_column("Number", m_program_columns.number);
  m_program_view.append_column("Name", m_program_columns.name);
  m_program_view.set_reorderable(false);
  m_program_view.set_headers_visible(false);
  m_program_view.set_rules_hint(true);
  m_program_view.get_selection()->signal_changed().
    connect(mem_fun(*this, &EuphoriaWidget::program_selection_changed));
  
  m_shaper.signal_apply.
    connect(compose(signal_shape_changed, 
                    mem_fun(m_shaper, &ShaperEditor::get_string)));
  m_shape_env.signal_apply.
    connect(compose(signal_shape_envelope_changed,
                    mem_fun(m_shape_env, &EnvelopeEditor::get_string)));
  m_phase.signal_apply.
    connect(compose(signal_phase_changed, 
                    mem_fun(m_phase, &PDEditor::get_string)));
  m_phase_env.signal_apply.
    connect(compose(signal_phase_envelope_changed,
                    mem_fun(m_phase_env, &EnvelopeEditor::get_string)));
  
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


void EuphoriaWidget::configure(const std::string& key, 
                               const std::string& value) {
  if (key == "shape")
    m_shaper.set_string(value);
  if (key == "shape_env")
    m_shape_env.set_string(value);
}


VBox* EuphoriaWidget::create_knob(LV2UIClient& lv2, const string& label, 
                                  int port,
                                  float red, float green, float blue) {
  VBox* box = manage(new VBox(false, 1));
  VGKnob* knob = manage(new VGKnob(e_ports[port].min, e_ports[port].max,
                                   e_ports[port].default_value, 
                                   red, green, blue, e_ports[port].integer,
                                   e_ports[port].logarithmic));
  lv2.connect_adjustment(&knob->get_adjustment(), port);
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
