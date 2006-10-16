#include "euphoriawidget.hpp"
#include "vgknob.hpp"


using namespace Gtk;
using namespace std;


EuphoriaWidget::EuphoriaWidget()
  : VBox(false, 6) {
  
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
  
  Button* phaseEditor = manage(new Button("Phase"));
  phaseEditor->set_size_request(100, 100);
  voiceTable->attach(*phaseEditor, 0, 1, 0, 1, 
                     AttachOptions(0), AttachOptions(0));
  Table* phaseT1 = manage(new Table(2, 2));
  voiceTable->attach(*phaseT1, 1, 2, 0, 1, AttachOptions(0), AttachOptions(0));
  phaseT1->set_spacings(3);
  phaseT1->attach(*create_knob("P.Dist"), 0, 1, 0, 1, AttachOptions(0));
  phaseT1->attach(*create_knob("Env"), 0, 1, 1, 2, AttachOptions(0));
  phaseT1->attach(*create_knob("Vel"), 1, 2, 0, 1, AttachOptions(0));
  Button* phaseEnvelope = manage(new Button("Phase envelope"));
  phaseEnvelope->set_size_request(200, 100);
  voiceTable->attach(*phaseEnvelope, 2, 3, 0, 1, 
                     FILL|EXPAND, AttachOptions(0));
  Table* phaseT2 = manage(new Table(2, 2));
  voiceTable->attach(*phaseT2, 3, 4, 0, 1, AttachOptions(0), AttachOptions(0));
  phaseT2->set_spacings(3);
  phaseT2->attach(*create_knob("Att", 1, 0, 0), 0, 1, 0, 1, AttachOptions(0));
  phaseT2->attach(*create_knob("Dec", 1, 1, 0), 1, 2, 0, 1, AttachOptions(0));
  phaseT2->attach(*create_knob("Sus", 0.3, 1, 0.3), 
                  0, 1, 1, 2, AttachOptions(0));
  phaseT2->attach(*create_knob("Rel"), 1, 2, 1, 2, AttachOptions(0));
  
  Button* shapeEditor = manage(new Button("Shape"));
  shapeEditor->set_size_request(100, 100);
  voiceTable->attach(*shapeEditor, 0, 1, 1, 2, 
                     AttachOptions(0),AttachOptions(0));
  Table* shapeT1 = manage(new Table(2, 2));
  voiceTable->attach(*shapeT1, 1, 2, 1, 2, AttachOptions(0), AttachOptions(0));
  shapeT1->set_spacings(3);
  shapeT1->attach(*create_knob("Shape"), 0, 1, 0, 1, AttachOptions(0));
  shapeT1->attach(*create_knob("Env"), 0, 1, 1, 2, AttachOptions(0));
  shapeT1->attach(*create_knob("Vel"), 1, 2, 0, 1, AttachOptions(0));
  Button* shapeEnvelope = manage(new Button("Shape envelope"));
  shapeEnvelope->set_size_request(200, 100);
  voiceTable->attach(*shapeEnvelope, 2, 3, 1, 2, 
                     FILL|EXPAND, AttachOptions(0));
  Table* shapeT2 = manage(new Table(2, 2));
  voiceTable->attach(*shapeT2, 3, 4, 1, 2, AttachOptions(0), AttachOptions(0));
  shapeT2->set_spacings(3);
  shapeT2->attach(*create_knob("Att", 1, 0, 0), 0, 1, 0, 1, AttachOptions(0));
  shapeT2->attach(*create_knob("Dec", 1, 1, 0), 1, 2, 0, 1, AttachOptions(0));
  shapeT2->attach(*create_knob("Sus", 0.3, 1, 0.3),
                  0, 1, 1, 2, AttachOptions(0));
  shapeT2->attach(*create_knob("Rel"), 1, 2, 1, 2, AttachOptions(0));
  
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
  vibratoHBox->pack_start(*create_knob("Freq", 1.0, 0.5, 0));
  vibratoHBox->pack_start(*create_knob("Depth", 1.0, 0.5, 0));
  
  Label* tremoloLabel = manage(new Label("<small>Tremolo</small>"));
  tremoloLabel->set_use_markup(true);
  voiceKnobTable->attach(*tremoloLabel, 1, 2, 0, 1, AttachOptions(0));
  HBox* tremoloHBox = manage(new HBox(false, 3));
  voiceKnobTable->attach(*tremoloHBox, 1, 2, 1, 2, AttachOptions(0));
  tremoloHBox->pack_start(*create_knob("Freq", 1, 0, 1));
  tremoloHBox->pack_start(*create_knob("Depth", 1, 0, 1));

  Label* unisonLabel = manage(new Label("<small>Unison</small>"));
  unisonLabel->set_use_markup(true);
  voiceKnobTable->attach(*unisonLabel, 2, 3, 0, 1, AttachOptions(0));
  HBox* unisonHBox = manage(new HBox(false, 3));
  voiceKnobTable->attach(*unisonHBox, 2, 3, 1, 2, AttachOptions(0));
  unisonHBox->pack_start(*create_knob("Layers", 0.4, 0.4, 0.4));
  unisonHBox->pack_start(*create_knob("Spread", 0.4, 0.4, 0.4));

  Label* ampLabel = manage(new Label("<small>Amp</small>"));
  ampLabel->set_use_markup(true);
  voiceKnobTable->attach(*ampLabel, 3, 4, 0, 1, AttachOptions(0));
  HBox* ampHBox = manage(new HBox(false, 3));
  voiceKnobTable->attach(*ampHBox, 3, 4, 1, 2, AttachOptions(0));
  ampHBox->pack_start(*create_knob("Env", 0, 1, 1));
  ampHBox->pack_start(*create_knob("Vel", 0, 1, 1));
  
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
  distTable->attach(*create_knob("Drive", 1, 0, 0), 0, 1, 1, 2, EXPAND);
  distTable->attach(*create_knob("Set", 1, 0, 0), 1, 2, 1, 2, EXPAND);
  distTable->attach(*create_knob("Tone", 1, 0, 0), 0, 1, 2, 3, EXPAND);
  distTable->attach(*create_knob("Mix", 1, 0, 0), 1, 2, 2, 3, EXPAND);
  
  // chorus
  Table* chorusTable = manage(new Table(3, 2));
  effectHBox->pack_start(*chorusTable);
  chorusTable->set_spacings(3);
  chorusTable->attach(*manage(new ToggleButton("Chorus")), 0, 2, 0, 1);
  chorusTable->attach(*create_knob("Freq", 0.75, 0.1, 0.25),
                      0, 1, 1, 2, EXPAND);
  chorusTable->attach(*create_knob("Depth", 0.75, 0.1, 0.25),
                      1, 2, 1, 2, EXPAND);
  chorusTable->attach(*create_knob("Delay", 0.75, 0.1, 0.25),
                      0, 1, 2, 3, EXPAND);
  chorusTable->attach(*create_knob("Mix", 0.75, 0.1, 0.25),
                      1, 2, 2, 3, EXPAND);
  
  // echo
  Table* echoTable = manage(new Table(3, 2));
  effectHBox->pack_start(*echoTable);
  echoTable->set_spacings(3);
  echoTable->attach(*manage(new ToggleButton("Echo")), 0, 2, 0, 1);
  echoTable->attach(*create_knob("Delay", 0.25, 0.2, 0.75),
                    0, 1, 1, 2, EXPAND);
  echoTable->attach(*create_knob("Fdback", 0.25, 0.2, 0.75),
                    1, 2, 1, 2, EXPAND);
  echoTable->attach(*create_knob("Pan", 0.25, 0.2, 0.75), 0, 1, 2, 3, EXPAND);
  echoTable->attach(*create_knob("Mix", 0.25, 0.2, 0.75), 1, 2, 2, 3, EXPAND);
  
  // reverb
  Table* reverbTable = manage(new Table(3, 2));
  effectHBox->pack_start(*reverbTable);
  reverbTable->set_spacings(3);
  reverbTable->attach(*manage(new ToggleButton("Reverb")), 0, 2, 0, 1);
  reverbTable->attach(*create_knob("Time"), 0, 1, 1, 2, EXPAND);
  reverbTable->attach(*create_knob("Room"), 1, 2, 1, 2, EXPAND);
  reverbTable->attach(*create_knob("Damp"), 0, 1, 2, 3, EXPAND);
  reverbTable->attach(*create_knob("Mix"), 1, 2, 2, 3, EXPAND);
  
  // global controls
  globalControlsBox->pack_start(*create_knob("Gain"), true, false);
  globalControlsBox->pack_start(*create_knob("Octave"), true, false);
  globalControlsBox->pack_start(*create_knob("Tune"), true, false);
  globalControlsBox->pack_start(*create_knob("Pan"), true, false);
  
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
  
  show_all();
}


VBox* EuphoriaWidget::create_knob(const string& label, 
                                  float red, float green, float blue) {
  VBox* box = manage(new VBox(false, 1));
  VGKnob* button = manage(new VGKnob(0, 1, 0.3, red, green, blue));
  //button->set_size_request(37, 37);
  box->pack_start(*button);
  Label* text = manage(new Label(string("<small>") + label + "</small>"));
  text->set_use_markup(true);
  box->pack_start(*text);
  return box;
}
