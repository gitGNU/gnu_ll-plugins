/****************************************************************************

    sampleview.hpp

    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

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
  sigc::signal<void, size_t, size_t>& signal_move_splitpoint();
  sigc::signal<void, size_t, size_t>& signal_play_preview();
  sigc::signal<void>& signal_stop_preview();
  
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
  void do_auto_split();
  void do_preview_on_click(bool enable);
  
  void draw_channel(size_t channel, Glib::RefPtr<Gdk::Window> win,
		    Glib::RefPtr<Gdk::GC> gc, int c, int h);
  
  sigc::signal<void, size_t> m_signal_add_splitpoint;
  sigc::signal<void, size_t> m_signal_remove_splitpoint;
  sigc::signal<void, size_t, size_t> m_signal_move_splitpoint;
  sigc::signal<void, size_t, size_t> m_signal_play_preview;
  sigc::signal<void> m_signal_stop_preview;
  
  Gdk::Color m_bg, m_fg, m_bgl, m_bgd, m_fgl, m_fgd, m_bgs, m_red, m_zero;
  
  SampleModel* m_model;
  
  Gtk::Adjustment m_scroll_adj;
  
  Gtk::Menu m_menu;
  
  int m_scale;

  size_t m_active_frame;
  int m_active_segment;
  
  int m_sel_begin;
  int m_sel_end;
  
  float* m_envelope;
  
  size_t m_drag_segment;
  int m_drag_start_x;
  int m_drag_x;
  
  bool m_preview_on_click;
  
};


#endif
