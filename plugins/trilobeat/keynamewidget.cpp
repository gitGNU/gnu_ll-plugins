/****************************************************************************
    
    keynamewidget.cpp - Simple drum sequencer widget
    
    Copyright (C) 2008 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
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

#include <iostream>
#include <sstream>
#include <string>

#include "keynamewidget.hpp"


using namespace Gtk;
using namespace Gdk;
using namespace Glib;
using namespace std;


KeyNameWidget::KeyNameWidget(unsigned int sheight, 
			     string const& background)
  : m_sheight(sheight),
    m_background(background) {
  
  char const* keys[] = { "C ", "C#", "D ", "D#", "E ", "F ", 
			 "F#", "G ", "G#", "A ", "A#", "B " };
  for (unsigned i = 0; i < 128; ++i) {
    ostringstream oss;
    oss<<keys[i % 12]<<(i / 12);
    m_keyinfo.push_back(KeyInfo(false, oss.str()));
  }
  
  m_white.set_rgb(65535, 65535, 65535);
  RefPtr<Colormap> cmap = Colormap::get_system();
  cmap->alloc_color(m_white);
  
  set_size_request(89, sheight - 1);
  add_events(EXPOSURE_MASK | BUTTON_PRESS_MASK);
  vector<TargetEntry> dnd_targets;
  dnd_targets.push_back(TargetEntry("x-org.nongnu.ll-plugins/keynames"));
  drag_dest_set(dnd_targets);
}


void KeyNameWidget::set_keyname(unsigned char key, std::string const& name) {
  m_keyinfo[key].name = name;
  if (!m_keyinfo[key].active) {
    m_keyinfo[key].active = true;
    int n = 0;
    for (unsigned i = 0; i < 128; ++i)
      n += m_keyinfo[i].active ? 1 : 0;
    set_size_request(89, n * m_sheight - 1);
  }
  queue_draw();
}


bool KeyNameWidget::on_button_press_event(GdkEventButton* event) {
  return true;
}


void KeyNameWidget::on_realize() {
  DrawingArea::on_realize();
  m_win = get_window();
  m_gc = GC::create(m_win);
  if (m_background != "") {
    RefPtr<Pixbuf> pixbuf = Pixbuf::create_from_file(m_background);
    int w = pixbuf->get_width();
    int h = pixbuf->get_height();
    RefPtr<Pixmap> pixmap = Pixmap::create(get_window(), w, h);
    RefPtr<GC> gc = GC::create(pixmap);
    pixmap->draw_pixbuf(gc, pixbuf, 0, 0, 0, 0, w, h, RGB_DITHER_NONE, 0, 0);
    RefPtr<Style> s = get_style()->copy();
    s->set_bg_pixmap(STATE_NORMAL, pixmap);
    s->set_bg_pixmap(STATE_ACTIVE, pixmap);
    s->set_bg_pixmap(STATE_PRELIGHT, pixmap);
    s->set_bg_pixmap(STATE_SELECTED, pixmap);
    s->set_bg_pixmap(STATE_INSENSITIVE, pixmap);
    set_style(s);
  }
  Pango::FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  m_layout = Pango::Layout::create(get_pango_context());
  m_layout->set_text("No active key");
}


bool KeyNameWidget::on_expose_event(GdkEventExpose* event) {
  int row = 0;
  m_gc->set_foreground(m_white);
  for (int key = 127; key >= 0; --key) {
    if (m_keyinfo[key].active) {
      m_layout->set_text(m_keyinfo[key].name);
      m_win->draw_layout(m_gc, 1, row * m_sheight + 1, m_layout);
      ++row;
    }
  }
  if (!row) {
    m_layout->set_text("No active key");
    m_win->draw_layout(m_gc, 1, 1, m_layout);
  }
  return true;
}


void KeyNameWidget::on_drag_data_received(const RefPtr<DragContext>& context,
					  int x, int y, 
					  const SelectionData& selection_data,
					  guint info, guint time) {
  cout<<__PRETTY_FUNCTION__<<endl;
  istringstream iss(selection_data.get_data_as_string());
  iss>>ws;
  while (!iss.eof()) {
    int key;
    iss>>key;
    string name;
    getline(iss, name);
    iss>>ws;
    cout<<"key: "<<key<<" name: "<<name<<endl;
  }
  signal_names_changed(selection_data.get_data_as_string());
}


unsigned char KeyNameWidget::row2key(int row) {
  if (row < 0)
    return -1;
  int c, i;
  for (i = 127, c = -1; i >= 0 && c < row; --i)
    c += m_keyinfo[i].active ? 1 : 0;
  if (i >= 0)
    return (unsigned char)(i + 1);
  return 128;
}

