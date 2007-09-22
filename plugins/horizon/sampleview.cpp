/****************************************************************************

    sampleview.cpp

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

#include <cstdio>
#include <iostream>

#include "samplemodel.hpp"
#include "sampleview.hpp"


using namespace std;
using namespace Gtk;
using namespace Gtk::Menu_Helpers;


SampleView::SampleView() 
  : m_model(0),
    m_scroll_adj(0, 0, 0),
    m_scale(4),
    m_active_frame(0),
    m_active_segment(-1),
    m_sel_begin(-1),
    m_sel_end(-1),
    m_envelope(0),
    m_drag_segment(0),
    m_drag_start_x(-1),
    m_drag_x(-1),
    m_preview_on_click(true) {
  
  m_bg.set_rgb(55000, 55000, 60000);
  m_bgl.set_rgb(65000, 65000, 65000);
  m_bgd.set_rgb(20000, 20000, 40000);
  m_bgs.set_rgb(37000, 37000, 50000);
  m_fg.set_rgb(0, 20000, 65000);
  m_red.set_rgb(65000, 0, 0);
  m_zero.set_rgb(30000, 30000, 30000);
  Glib::RefPtr<Gdk::Colormap> cmap = Gdk::Colormap::get_system();
  cmap->alloc_color(m_bg);
  cmap->alloc_color(m_bgl);
  cmap->alloc_color(m_bgd);
  cmap->alloc_color(m_bgs);
  cmap->alloc_color(m_fg);
  cmap->alloc_color(m_red);
  cmap->alloc_color(m_zero);
  
  add_events(Gdk::EXPOSURE_MASK | 
	     Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK |
             Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
	     Gdk::SCROLL_MASK);  
  
  m_scroll_adj.signal_value_changed().
    connect(mem_fun(*this, &SampleView::queue_draw));
  
  m_menu.items().
    push_back(MenuElem("Add splitpoint", 
                       mem_fun(*this, &SampleView::do_add_splitpoint)));
  m_menu.items().
    push_back(MenuElem("Merge", 
                       mem_fun(*this, &SampleView::do_merge)));
  m_menu.items().
    push_back(MenuElem("Split in 2", 
                       mem_fun(*this, &SampleView::do_split_in_2)));
  m_menu.items().
    push_back(MenuElem("Split in 3", 
                       mem_fun(*this, &SampleView::do_split_in_3)));
  m_menu.items().
    push_back(MenuElem("Auto split", 
                       mem_fun(*this, &SampleView::do_auto_split)));
  
  m_menu.items().push_back(SeparatorElem());
  
  CheckMenuItem* cmi = manage(new CheckMenuItem("Preview on click"));
  cmi->set_active(m_preview_on_click);
  cmi->signal_toggled().
    connect(compose(mem_fun(*this, &SampleView::do_preview_on_click),
		    mem_fun(*cmi, &CheckMenuItem::get_active)));
  cmi->show_all();
  m_menu.items().push_back(*cmi);
  
}


void SampleView::set_model(SampleModel* model) {
  m_model = model;
  m_sel_begin = m_sel_end = -1;
  queue_draw();
  delete [] m_envelope;
  m_envelope = 0;
  if (m_model)
    m_scroll_adj.set_upper(model->get_length() / pow(2.0, m_scale) + 1);
  else
    m_scroll_adj.set_upper(0);
  m_scroll_adj.set_value(0);
}


Gtk::Adjustment& SampleView::get_scroll_adjustment() {
  return m_scroll_adj;
}

  
bool SampleView::on_expose_event(GdkEventExpose* event) {
  
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  gc->set_foreground(m_bg);
  int w = get_width();
  int h = get_height();
  
  // draw background (including selection highlight)
  win->draw_rectangle(gc, true, 0, 0, w, h);
  gc->set_foreground(m_bgd);
  win->draw_line(gc, w - 1, 0, w - 1, h - 1);
  win->draw_line(gc, 0, h - 1, w - 1, h - 1);
  gc->set_foreground(m_bgl);
  win->draw_line(gc, 0, 0, w, 0);
  win->draw_line(gc, 0, 0, 0, h);
  if (m_model && m_sel_begin >= 0) {
    const vector<size_t>& seg = m_model->get_splitpoints();
    int begin = int(seg[m_sel_begin] / pow(2.0, m_scale) - 
		    m_scroll_adj.get_value());
    int end = int(seg[m_sel_end + 1] / pow(2.0, m_scale) - 
		  m_scroll_adj.get_value());
    if (end > 0 && begin < w) {
      begin = begin > 0 ? begin : 0;
      end = end < get_width() ? end : w;
      gc->set_foreground(m_bgs);
      win->draw_rectangle(gc, true, begin, 0, end - begin, h);
    }
  }
  
  if (!m_model)
    return true;
  
  // draw audio
  if (m_model->get_channels() == 1) {
    int m = h / 2;
    gc->set_foreground(m_zero);
    win->draw_line(gc, 0, m, w, m);
    gc->set_foreground(m_fg);
    draw_channel(0, win, gc, m, m - 1);
  }
  else if (m_model->get_channels() == 2) {
    int m = h / 4;
    gc->set_foreground(m_zero);
    win->draw_line(gc, 0, m, w, m);
    win->draw_line(gc, 0, m + h / 2, w, m + h / 2);
    gc->set_foreground(m_fg);
    draw_channel(0, win, gc, m, m - 1);
    draw_channel(1, win, gc, m + h / 2, m - 1);
  }
  
  // draw splitpoints
  const vector<size_t>& seg = m_model->get_splitpoints();
  for (size_t i = 0; i < seg.size() - 1; ++i) {
    int x = int(seg[i] / pow(2.0, m_scale) - m_scroll_adj.get_value());
    int x2 = int(seg[i + 1] / pow(2.0, m_scale) - m_scroll_adj.get_value());
    x2 = get_width() > x2 ? x2 : get_width();
    
    // check if the segment intersects the view
    if (x < 0 && x2 < 0)
      continue;
    else if (x > get_width())
      break;
    
    // draw the vertical lines
    if (i == m_active_segment + 1)
      gc->set_foreground(m_bgl);
    else
      gc->set_foreground(m_bgd);
    win->draw_line(gc, x - 1, 0, x - 1, h - 1);
    if (i == m_active_segment)
      gc->set_foreground(m_bgd);
    else
      gc->set_foreground(m_bgl);
    win->draw_line(gc, x, 0, x, h - 1);
    
    // draw the horizontal lines if needed
    if (i == m_active_segment) {
      gc->set_foreground(m_bgd);
      win->draw_line(gc, x, 0, x2, 0);
      gc->set_foreground(m_bgl);
      win->draw_line(gc, x, h - 1, x2, h - 1);
    }
    
  }
  
  // draw the dragged splitpoint
  if (m_drag_segment > 0) {
    gc->set_foreground(m_red);
    int x = int(m_model->get_splitpoints()[m_drag_segment] / pow(2.0, m_scale) -
		m_scroll_adj.get_value()) + m_drag_x - m_drag_start_x;
    int min = int(m_model->get_splitpoints()[m_drag_segment - 1] / 
		  pow(2.0, m_scale) - m_scroll_adj.get_value());
    int max = int(m_model->get_splitpoints()[m_drag_segment + 1] / 
		  pow(2.0, m_scale) - m_scroll_adj.get_value());
    x = x < min ? min : x;
    x = x > max ? max : x;
    win->draw_line(gc, x, 1, x, h - 1);
  }
  
  // draw envelope
  int p = 0;
  if (m_envelope && m_scale >= 0) {
    gc->set_foreground(m_red);
    int scale = int(pow(2.0, m_scale));
    int scroll = int(m_scroll_adj.get_value() * scale);
    for (size_t i = 0; i < w; ++i) {
      size_t index = scroll + i * scale;
      if (index >= m_model->get_length())
	break;
      int y = int(h / 2 + (h / 2 - 1) * m_envelope[index]);
      win->draw_line(gc, i - 1, p, i, y);
      p = y;
    }
  }

  return true;
}


bool SampleView::on_motion_notify_event(GdkEventMotion* event) {
  if (m_drag_segment > 0) {
    m_drag_x = int(event->x);
    queue_draw();
  }
  return true;
}


bool SampleView::on_button_press_event(GdkEventButton* event) {
  
  if (!m_model)
    return true;
  
  // button 1 - activate and select
  if (event->button == 1) {
    size_t frame = size_t((m_scroll_adj.get_value() + event->x) * 
			  pow(2.0, m_scale));
    const vector<size_t>& seg = m_model->get_splitpoints();
    for (unsigned i = 0; i < seg.size() - 1; ++i) {
      if (seg[i + 1] > frame) {
	m_active_segment = i;
	if (m_preview_on_click) {
	  m_signal_play_preview(m_model->get_splitpoints()[i],
				m_model->get_splitpoints()[i + 1]);
	}
	if (event->state & GDK_SHIFT_MASK && m_sel_begin >= 0) {
	  m_sel_begin = m_sel_begin < i ? m_sel_begin : i;
	  m_sel_end = m_sel_end > i ? m_sel_end : i;
	}
	else
	  m_sel_begin = m_sel_end = i;
	queue_draw();
	return true;
      }
    }
	   
  }
  
  // button 2 - move the splitpoints
  else if (event->button == 2 && m_model->get_splitpoints().size() > 2) {
    cerr<<"OK!"<<endl;
    double frame = (event->x + m_scroll_adj.get_value()) * pow(2.0, m_scale);
    if (frame < m_model->get_length()) {
      double limit = 10 * pow(2.0, m_scale);
      const vector<size_t>& seg = m_model->get_splitpoints();
      unsigned point = 1;
      double d = abs(seg[1] - frame);
      for (unsigned i = 2; i < seg.size() - 1; ++i) {
	double new_d = abs(seg[i] - frame);
	if (new_d < limit && new_d < d) {
	  d = new_d;
	  point = i;
	}
      }
      if (d < limit) {
	m_drag_segment = point;
	m_drag_x = m_drag_start_x = int(event->x);
	queue_draw();
	cerr<<"Started dragging segment "<<point<<endl;
      }
    }
  }
  
  // button 3 - pop up the menu
  else if (event->button == 3) {
    m_active_frame = size_t((m_scroll_adj.get_value() + event->x) * 
			    pow(2.0, m_scale));
    m_menu.popup(event->button, event->time);
  }
  
  return true;
}


bool SampleView::on_button_release_event(GdkEventButton* event) {
  
  if (!m_model)
    return true;
  
  // button 1 - inactivate the active segment
  if (event->button == 1 && m_active_segment >= 0) {
    m_active_segment = -1;
    m_signal_stop_preview();
    queue_draw();
  }
  
  // button 2 - stop dragging a splitpoint
  else if (event->button == 2 && m_drag_segment > 0) {
    const vector<size_t>& seg = m_model->get_splitpoints();
    long x = int(seg[m_drag_segment] + 
		(m_drag_x - m_drag_start_x) * pow(2.0, m_scale));
    long min = seg[m_drag_segment - 1] + 1;
    long max = seg[m_drag_segment + 1] - 1;
    x = x < min ? min : x;
    x = x > max ? max : x;
    m_signal_move_splitpoint(m_model->get_splitpoints()[m_drag_segment], x);
    m_drag_segment = 0;
    queue_draw();
  }
  
  return true;
}


bool SampleView::on_scroll_event(GdkEventScroll* event) {
  double frame = (m_scroll_adj.get_value() + event->x + 0.5) * pow(2.0, m_scale);
  if (event->direction == GDK_SCROLL_UP && m_scale > -4)
    --m_scale;
  else if (event->direction == GDK_SCROLL_DOWN && m_scale < 16)
    ++m_scale;
  if (m_model) {
    m_scroll_adj.set_upper(m_model->get_length() / pow(2.0, m_scale) + 1);
    double new_scroll = frame / pow(2.0, m_scale) - event->x - 0.5;
    new_scroll = new_scroll < 0 ? 0 : new_scroll;
    double max = m_scroll_adj.get_upper() - m_scroll_adj.get_page_size();
    new_scroll = new_scroll > max ? max : new_scroll;
    m_scroll_adj.set_value(new_scroll);
    queue_draw();
  }

  return true;
}


void SampleView::on_size_allocate(Gtk::Allocation& a) {
  DrawingArea::on_size_allocate(a);
  m_scroll_adj.set_page_size(a.get_width());
}


void SampleView::do_add_splitpoint() {
  if (m_model) {
    if (m_active_frame < m_model->get_length())
      m_signal_add_splitpoint(m_active_frame);
  }
}


void SampleView::do_merge() {
  if (!m_model || m_sel_begin == -1)
    return;
  vector<size_t> to_remove;
  const vector<size_t>& seg = m_model->get_splitpoints();
  for (size_t i = m_sel_begin + 1; i <= m_sel_end; ++i)
    to_remove.push_back(seg[i]);
  for (size_t i = 0; i < to_remove.size(); ++i)
    m_signal_remove_splitpoint(to_remove[i]);
}


void SampleView::do_split_in_2() {
  if (!m_model || m_sel_begin == -1)
    return;
  vector<size_t> new_points;
  const vector<size_t>& seg = m_model->get_splitpoints();
  for (size_t i = m_sel_begin; i <= m_sel_end; ++i)
    new_points.push_back((seg[i] + seg[i + 1]) / 2);
  for (size_t i = 0; i < new_points.size(); ++i)
    m_signal_add_splitpoint(new_points[i]);
}


void SampleView::do_split_in_3() {
  if (!m_model || m_sel_begin == -1)
    return;
  vector<size_t> new_points;
  const vector<size_t>& seg = m_model->get_splitpoints();
  for (size_t i = m_sel_begin; i <= m_sel_end; ++i) {
    new_points.push_back((2 * seg[i] + seg[i + 1]) / 3);
    new_points.push_back((seg[i] + 2 * seg[i + 1]) / 3);
  }
  for (size_t i = 0; i < new_points.size(); ++i)
    m_signal_add_splitpoint(new_points[i]);  
}


void SampleView::do_auto_split() {
  
  if (!m_model || m_sel_begin == -1)
    return;
  
  /* This is a very crude onset detection algorithm, but it works reasonably
     well for simple beats. The procedure is something like this:
     
     * process the sample data with an asymmetric low pass filter that
       smooths out fast descents but lets fast ascents through, write the
       processed data to a circular buffer (delay line)
       
     * for every sample, check if the difference between the current processed 
       value and the oldest value in the buffer is larger than a given 
       threshold, if it is we have an onset somewhere in the buffer
       
     * search the buffer to find a value that is a weighted average of the
       oldest and the newest buffer values, with given weights, and call that
       frame the onset
       
     * when the onset is found, find the best cut point just before it
       (preferrably a zero crossing)
       
     The parameters below can be used to tweak the performance of the auto
     splitter.
  */
  
  delete [] m_envelope;
  m_envelope = new float[m_model->get_length()];
  memset(m_envelope, 0, m_model->get_length() * sizeof(float));
  
  // algorithm parameters
  // coefficient for the envelope tracker - higher means smoother descents
  double k = pow(0.999, 48000.0 / m_model->get_rate());
  // higher threshold means less detected onsets
  float threshold = 0.3;
  // too high value may cause too early onsets, too low may cause too early
  float sensitivity = 0.7;
  // the size of the circular buffer
  int risetime = int(m_model->get_rate() * 0.05);
  // the minimum time between detected onsets
  int deadtime = int(m_model->get_rate() * 0.2);
  // how far from the onset frame we may look for a perfect cut point
  int slack = int(m_model->get_rate() * 0.003);
  
  float* buffer = new float[risetime];
  memset(buffer, 0, risetime * sizeof(float));
  int offset = 0;
  const vector<size_t>& seg = m_model->get_splitpoints();
  size_t start = seg[m_sel_begin];
  size_t end = seg[m_sel_end + 1];
  int dead = deadtime;
  vector<size_t> new_points;
  const float* data = m_model->get_data(0);
  float y_1 = abs(data[start]);

  for (size_t i = start; i < end; ++i) {
    
    // asymmetric filter (envelope tracker)
    const float& x_0 = abs(data[i]);
    float y_0 = (1 - k) * x_0 + k * y_1;
    y_0 = x_0 > y_0 ? x_0 : y_0;
    
    // write to a circular buffer
    buffer[offset] = y_0;
    
    // check if the difference between the oldest and the newest value in the 
    // buffer is above the threshold, if it is we have found an onset
    if (!dead &&
	buffer[offset] - buffer[(offset + 1) % risetime] > threshold) {
      
      cerr<<"found an onset somewhere before frame "<<i<<"!"<<endl;
      
      // compute the envelope value where we say that the onset actually happens
      float v_1 = buffer[(offset + 1) % risetime];
      float v_2 = buffer[offset];
      float v_t = v_1 + (v_2 - v_1) * (1 - sensitivity);
      
      // find the frame where the envelope goes above that value
      int a = 0;
      int b = risetime - 1;
      while (b - a > 1) {
	int c = (a + b) / 2;
	if (buffer[(offset + 1 + c) % risetime] <= v_t)
	  a = c;
	else
	  b = c;
      }
      a = i - (risetime - 1) + a;
      
      cerr<<"the actual onset is at frame "<<a<<endl;
      
      // find the best split point nearby
      int best = a - slack;
      float v_b = abs(data[best]);
      for (int j = a - slack + 1; j < a; ++j) {
	if (abs(data[j]) <= v_b) {
	  v_b = abs(data[j]);
	  best = j;
	}
      }
      new_points.push_back(best);
      
      cerr<<"the best splitpoint is "<<best<<" with sample value "<<v_b<<endl;
      
      dead = deadtime - (i - best);
    }
    
    // increase the buffer counter
    offset = (offset + 1) % risetime;
    
    // count down the deadtime
    --dead;
    dead = dead < 0 ? 0 : dead;
    
    // write the envelope to an internal buffer for debug display
    m_envelope[i] = y_0;

    y_1 = y_0;
    
  }
  
  delete [] buffer;
  
  for (size_t i = 0; i < new_points.size(); ++i)
    m_signal_add_splitpoint(new_points[i]);
  
  queue_draw();
}


void SampleView::do_preview_on_click(bool enable) {
  m_preview_on_click = enable;
}


sigc::signal<void, size_t>& SampleView::signal_add_splitpoint() {
  return m_signal_add_splitpoint;
}


sigc::signal<void, size_t>& SampleView::signal_remove_splitpoint() {
  return m_signal_remove_splitpoint;
}


sigc::signal<void, size_t, size_t>& SampleView::signal_move_splitpoint() {
  return m_signal_move_splitpoint;
}


sigc::signal<void, size_t, size_t>& SampleView::signal_play_preview() {
  return m_signal_play_preview;
}


sigc::signal<void>& SampleView::signal_stop_preview() {
  return m_signal_stop_preview;
}


void SampleView::draw_channel(size_t channel, Glib::RefPtr<Gdk::Window> win,
			      Glib::RefPtr<Gdk::GC> gc, int c, int h) {
  
  const SampleModel::PeakData* peak = 0;
  float scale = 0;
  size_t scroll = size_t(m_scroll_adj.get_value());
  size_t lines = get_width();
  if (scroll + lines > m_model->get_length() / pow(2.0, m_scale))
    lines = size_t(m_model->get_length() / pow(2.0, m_scale) - scroll);
  
  // if we are zoomed in closer than 1:1, draw each sample as a tiny box
  if (m_scale < 0) {
    scale = pow(2.0, m_scale);
    const float* data = m_model->get_data(channel);
    const int offset = int(scroll * scale);
    for (size_t i = 0; i < lines * scale; ++i) {
      win->draw_rectangle(gc, true, int(i / scale), 
			  c + int(h * data[i + offset]) - 1, 3, 3); 
    }
  }
  
  // if the scale is 1:1 < 1:16, use the actual data
  else if (m_scale < 4) {
    scale = pow(2.0, m_scale);
    const float* data = m_model->get_data(channel);
    for (size_t i = scroll; i < scroll + lines; ++i) {
      float min = data[int(i*scale)];
      float max = data[int(i*scale)];
      for (size_t j = 0; j < scale; ++j) {
	min = min < data[int(i*scale + j)] ? min : data[int(i*scale + j)];
	max = max > data[int(i*scale + j)] ? max : data[int(i*scale + j)];
      }
      
      win->draw_line(gc, i - scroll, c + int(h * min),
		     i - scroll, c + int(h * max));
    }
  }
  
  // else, select a peak level depending on the scale
  else {
    if (m_scale < 8) {
      scale = pow(2.0, m_scale - 4);
      peak = m_model->get_peak_data(channel)[0];
    }
    
    else if (m_scale < 12) {
      scale = pow(2.0, m_scale - 8);
      peak = m_model->get_peak_data(channel)[1];
    }
    
    else {
      scale = pow(2.0, m_scale - 12);
      peak = m_model->get_peak_data(channel)[2];
    }
    
    if (peak) {
      size_t scroll = size_t(m_scroll_adj.get_value());
      for (size_t i = scroll; i < scroll + lines; ++i) {
	float min = peak[int(i*scale)].min;
	float max = peak[int(i*scale)].max;
	for (size_t j = 0; j < scale; ++j) {
	  min = min < peak[int(i*scale + j)].min ? 
	    min : peak[int(i*scale + j)].min;
	  max = max > peak[int(i*scale + j)].max ? 
	    max : peak[int(i*scale + j)].max;
	}
	
	win->draw_line(gc, i - scroll, c + int(h * min),
		       i - scroll, c + int(h * max));
      }
    }
  }
  
}
