#ifndef STACKVIEW_HPP
#define STACKVIEW_HPP

#include <gtkmm.h>


class SampleModel;


class EffectStackView : public Gtk::DrawingArea {
public:
  
  EffectStackView();
  
  void set_model(SampleModel* model);
  
  sigc::signal<void, size_t, const std::string&>& signal_add_effect();
  sigc::signal<void, size_t>& signal_remove_effect();
  sigc::signal<void, size_t, bool>& signal_bypass_effect();
  
protected:
  
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_scroll_event(GdkEventScroll* event);
  
  void do_add_effect();
  void do_remove_effect();
  void do_bypass_effect();
  
  
  sigc::signal<void, size_t, const std::string&> m_signal_add_effect;
  sigc::signal<void, size_t> m_signal_remove_effect;
  sigc::signal<void, size_t, bool> m_signal_bypass_effect;
  
  Gdk::Color m_bg, m_fg, m_bgl, m_bgd, m_fgbp, m_bgs, m_red, m_zero;
  
  SampleModel* m_model;
  
  Gtk::Menu m_menu;
  
  int m_stepheight;
  int m_selected;
  int m_offset;
  
};


#endif
