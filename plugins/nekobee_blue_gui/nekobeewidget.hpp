#ifndef NEKOBEEWIDGET_HPP
#define NEKOBEEWIDGET_HPP

#include <string>

#include <cairomm/cairomm.h>
#include <gtkmm.h>


class NekobeeWidget : public Gtk::DrawingArea {
public:
  
  NekobeeWidget(const std::string& bundle);
  ~NekobeeWidget();
  
  void set_waveform(float waveform);
  void set_tuning(float tuning);
  void set_cutoff(float cutoff);
  void set_resonance(float resonance);
  void set_envmod(float envmod);
  void set_decay(float decay);
  void set_accent(float accent);
  void set_volume(float volume);

  sigc::signal<void, float> signal_waveform_changed;
  sigc::signal<void, float> signal_tuning_changed;
  sigc::signal<void, float> signal_cutoff_changed;
  sigc::signal<void, float> signal_resonance_changed;
  sigc::signal<void, float> signal_envmod_changed;
  sigc::signal<void, float> signal_decay_changed;
  sigc::signal<void, float> signal_accent_changed;
  sigc::signal<void, float> signal_volume_changed;

protected:
  
  enum Control {
    Tuning,
    Cutoff,
    Resonance,
    EnvMod,
    Decay,
    Accent,
    Volume,
    Waveform,
    NumControls,
  };
  
  void on_realize();
  bool on_expose_event(GdkEventExpose* event);
  bool on_button_press_event(GdkEventButton* event);
  bool on_scroll_event(GdkEventScroll* event);
  
  void draw_bar(Cairo::RefPtr<Cairo::Context>& cc, 
		float x1, float y1, float x2, float y2, 
		float x3, float y3, float x4, float y4, Control c);
  void set_text(Control c);
  void hide_text();
  
  std::string m_bundle;
  Gdk::Color m_bar_colour, m_text_colour;
  Control m_active_control;
  sigc::connection m_text_connection;
  std::string m_control_names[NumControls];
  float m_text_size;
  
  float m_controls[NumControls];
  float m_min_values[NumControls];
  float m_max_values[NumControls];
  sigc::signal<void, float>* m_signals[NumControls];
  
};


#endif
