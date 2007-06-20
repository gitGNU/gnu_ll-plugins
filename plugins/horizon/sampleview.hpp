#ifndef SAMPLEVIEW_HPP
#define SAMPLEVIEW_HPP

#include <gtkmm.h>


class SampleModel;


class SampleView : public Gtk::DrawingArea {
public:
  
  SampleView();
  
  void set_model(SampleModel* model);
  
  Gtk::Adjustment& get_scroll_adjustment();
  
  sigc::signal<void, size_t>& signal_add_splitpoint();
  sigc::signal<void, size_t>& signal_remove_splitpoint();
  
protected:
  
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_scroll_event(GdkEventScroll* event);
  virtual void on_size_allocate(Gtk::Allocation& allocation);
  
  void do_add_splitpoint();
  void do_merge();
  void do_split_in_2();
  void do_split_in_3();
  
  void draw_channel(size_t channel, Glib::RefPtr<Gdk::Window> win,
		    Glib::RefPtr<Gdk::GC> gc, int c, int h);
  
  Gdk::Color m_bg, m_fg, m_bgl, m_bgd, m_fgl, m_fgd, m_bgs;
  
  SampleModel* m_model;
  
  Gtk::Adjustment m_scroll_adj;
  
  Gtk::Menu m_menu;
  
  int m_scale;

  size_t m_active_frame;
  int m_active_segment;
  
  int m_sel_begin;
  int m_sel_end;
  
  
  sigc::signal<void, size_t> m_signal_add_splitpoint;
  sigc::signal<void, size_t> m_signal_remove_splitpoint;
  
};


#endif
