#include <iostream>

#include "switch.hpp"
#include "vonoff.xpm"
#include "onoffgreen.xpm"
#include "minioffon.xpm"


using namespace Gtk;
using namespace Gdk;
using namespace Glib;
using namespace std;


Switch::Switch(Type type) 
  : m_adj(0, 0, 1),
    m_type(type) {

  if (m_type == BigRed) {
    m_width = 15;
    m_height = 28;
  }
  else if (m_type == Green) {
    m_width = 25;
    m_height = 11;
  }
  else if (m_type == Mini) {
    m_width = 11;
    m_height = 12;
  }
  set_size_request(m_width, m_height);
  add_events(EXPOSURE_MASK | BUTTON_PRESS_MASK);
  m_adj.signal_value_changed().connect(mem_fun(*this, &Switch::queue_draw));
}
 
 
void Switch::set_value(float value) {
  m_adj.set_value(value);
}
  

Gtk::Adjustment& Switch::get_adjustment() {
  return m_adj;
}


void Switch::on_realize() {
  DrawingArea::on_realize();
  unsigned pixsize = (m_width * m_height * 2 + 1) / 8;
  char bits[pixsize];
  memset(bits, 0, sizeof(char) * pixsize);
  RefPtr<Bitmap> bitmap = Bitmap::create(bits, m_width, m_height * 2);
  char** xpm = 0;
  if (m_type == BigRed)
    xpm = vonoff;
  else if (m_type == Green)
    xpm = onoffgreen;
  else if (m_type == Mini)
    xpm = minioffon;
  m_pixmap = Pixmap::create_from_xpm(Colormap::get_system(), bitmap, xpm);
}


bool Switch::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  RefPtr<GC> gc = GC::create(win);
  int yoffset = 0;
  if (m_adj.get_value() > 0.5)
    yoffset = m_height;
  win->draw_drawable(gc, m_pixmap, 0, yoffset, 0, 0, m_width, m_height);
  return true;
}


bool Switch::on_button_press_event(GdkEventButton* event) {
  m_adj.set_value(m_adj.get_value() < 0.5 ? 1 : 0);
  return true;
}
