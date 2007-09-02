/****************************************************************************

    optionslider.cpp

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

#include "optionslider.hpp"


OptionSlider::OptionSlider(double lower, double upper, double value, 
			   int digits, const std::vector<std::string>& options)
  : LabelSlider(lower, upper, value, digits),
    m_option(0),
    m_labels(options) {
  
  m_bg.set_rgb(40000, 40000, 50000);
  Glib::RefPtr<Gdk::Colormap> cmap = Gdk::Colormap::get_system();
  cmap->alloc_color(m_bg);
  
  sigc::slot<void, int> so = mem_fun(*this, &OptionSlider::set_option);
  using namespace Gtk::Menu_Helpers;
  for (int i = 0; i < m_labels.size(); ++i)
    m_menu.items().push_back(MenuElem(m_labels[i], sigc::bind(so, i)));
  
  int w, h;
  get_size_request(w, h);
  for (int i = 1; i < m_labels.size(); ++i) {
    char buf[50];
    snprintf(buf, 50, "<b><small>%s</small></b>", m_labels[i].c_str());
    m_layout->set_markup(buf);
    int lw, lh;
    m_layout->get_pixel_size(lw, lh);
    lw = int(lw * 1.1);
    lh = int(lh * 1.1);
    w = lw > w ? lw : w;
    h = lh > h ? lh : h;
  }
  set_size_request(w, h);
}
  

void OptionSlider::set_option(int option) {
  m_option = option;
  queue_draw();
}


bool OptionSlider::on_expose_event(GdkEventExpose* event) {
  
  if (m_option == 0)
    return LabelSlider::on_expose_event(event);

  char buf[50];
  snprintf(buf, 50, "<b><small>%s</small></b>", m_labels[m_option].c_str());
  m_layout->set_markup(buf);
  int lw, lh;
  m_layout->get_pixel_size(lw, lh);

  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  gc->set_foreground(m_black);
  int w = get_width();
  int h = get_height();
  win->draw_rectangle(gc, false, 0, 0, w - 1, h - 1);
  int x = int((w - 2) * (m_adj.get_value() - m_adj.get_lower()) / 
	      (m_adj.get_upper() - m_adj.get_lower()));
  gc->set_foreground(m_bg);
  win->draw_rectangle(gc, true, 1, 1, w - 2, h - 2);
  gc->set_foreground(m_black);
  win->draw_layout(gc, (w - lw) / 2, (h - lh) / 2, m_layout);
  return true;
}


bool OptionSlider::on_motion_notify_event(GdkEventMotion* event) {
  if (m_option == 0)
    return LabelSlider::on_motion_notify_event(event);
  return true;
}


bool OptionSlider::on_button_press_event(GdkEventButton* event) {
  if (event->button == 3)
    m_menu.popup(event->button, event->time);
  
  else if (m_option == 0)
    return LabelSlider::on_button_press_event(event);

  return true;
}


bool OptionSlider::on_scroll_event(GdkEventScroll* event) {
  if (m_option == 0)
    return LabelSlider::on_scroll_event(event);
  return true;
}
