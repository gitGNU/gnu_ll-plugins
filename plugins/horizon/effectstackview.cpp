#include <cstdio>
#include <iostream>

#include "samplemodel.hpp"
#include "effectstackview.hpp"


using namespace std;
using namespace Gtk::Menu_Helpers;


EffectStackView::EffectStackView() 
  : m_model(0),
    m_stepheight(20),
    m_selected(-1),
    m_offset(0) {
  
  m_bg.set_rgb(30000, 45000, 30000);
  m_bgl.set_rgb(50000, 65000, 50000);
  m_bgd.set_rgb(15000, 25000, 15000);
  m_bgs.set_rgb(0, 60000, 0);
  m_fg.set_rgb(0, 0, 0);
  m_fgbp.set_rgb(22500, 31250, 22500);
  m_red.set_rgb(65000, 0, 0);
  m_zero.set_rgb(30000, 30000, 30000);
  Glib::RefPtr<Gdk::Colormap> cmap = Gdk::Colormap::get_system();
  cmap->alloc_color(m_bg);
  cmap->alloc_color(m_bgl);
  cmap->alloc_color(m_bgd);
  cmap->alloc_color(m_bgs);
  cmap->alloc_color(m_fg);
  cmap->alloc_color(m_fgbp);
  cmap->alloc_color(m_red);
  cmap->alloc_color(m_zero);
  
  add_events(Gdk::EXPOSURE_MASK | 
	     Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK |
             Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
	     Gdk::SCROLL_MASK);
  
  m_menu.items().
    push_back(MenuElem("Insert new effect", 
		       mem_fun(*this, &EffectStackView::do_add_effect)));
  m_menu.items().
    push_back(MenuElem("Bypass",
		       mem_fun(*this, &EffectStackView::do_bypass_effect)));
  m_menu.items().
    push_back(MenuElem("Remove",
		       mem_fun(*this, &EffectStackView::do_remove_effect)));
  
  set_size_request(100, 2 * m_stepheight);
}


void EffectStackView::set_model(SampleModel* model) {
  m_model = model;
  queue_draw();
}


bool EffectStackView::on_expose_event(GdkEventExpose* event) {
  
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  int w = get_width();
  int h = get_height();
  
  // background and borders
  gc->set_foreground(m_bg);
  win->draw_rectangle(gc, true, 0, 0, w, h);
  if (m_selected >= 0) {
    gc->set_foreground(m_bgs);
    win->draw_rectangle(gc, true, 0, (m_offset + m_selected) * m_stepheight,
			w, m_stepheight);
  }
  gc->set_foreground(m_bgl);
  win->draw_line(gc, 0, 0, w, 0);
  win->draw_line(gc, 0, 0, 0, h);
  gc->set_foreground(m_bgd);
  win->draw_line(gc, 1, h - 1, w, h - 1);
  win->draw_line(gc, w - 1, 1, w - 1, h);
  
  // internal edges
  for (int i = 1; i * m_stepheight + 1 <= h; ++i) {
    gc->set_foreground(m_bgd);
    win->draw_line(gc, 1, i * m_stepheight, w - 1, i * m_stepheight);
    gc->set_foreground(m_bgl);
    win->draw_line(gc, 1, i * m_stepheight + 1, w - 1, i * m_stepheight + 1);
  }
  
  if (!m_model)
    return true;
  
  // text
  gc->set_foreground(m_fg);
  const EffectStackModel& est = m_model->get_effect_stack_model();
  Glib::RefPtr<Pango::Layout> l;
  for (unsigned i = m_offset; i < est.get_effects().size(); ++i) {
    l = Pango::Layout::create(get_pango_context());
    l->set_text(est.get_effects()[i].get_name());
    Pango::FontDescription f("sans,monospace bold");
    f.set_absolute_size(Pango::SCALE * 10);
    l->set_font_description(f);
    if (est.get_effects()[i].get_bypassed())
      gc->set_foreground(m_fgbp);
    win->draw_layout(gc, 4, (i - m_offset) * m_stepheight + 4, l);
    if (est.get_effects()[i].get_bypassed())
      gc->set_foreground(m_fg);
  }
  
  return true;
}


bool EffectStackView::on_motion_notify_event(GdkEventMotion* event) {
  return true;
}


bool EffectStackView::on_button_press_event(GdkEventButton* event) {
  
  if (!m_model)
    return true;
  
  int step = int(event->y / m_stepheight);
  if (step >= 0 && 
      step < m_model->get_effect_stack_model().get_effects().size())
    m_selected = step;
  else
    m_selected = -1;
  queue_draw();
  
  // button 3 - pop up the menu
  if (event->button == 3) {
    m_menu.popup(event->button, event->time);
  }
  
  return true;
}


bool EffectStackView::on_button_release_event(GdkEventButton* event) {
  return true;
}


bool EffectStackView::on_scroll_event(GdkEventScroll* event) {
  return true;
}


sigc::signal<void, size_t, const std::string&>& 
EffectStackView::signal_add_effect() {
  return m_signal_add_effect;
}


sigc::signal<void, size_t>& EffectStackView::signal_remove_effect() {
  return m_signal_remove_effect;
}


sigc::signal<void, size_t, bool>& EffectStackView::signal_bypass_effect() {
  return m_signal_bypass_effect;
}


void EffectStackView::do_add_effect() {
  if (!m_model)
    return;
  m_signal_add_effect(m_model->get_effect_stack_model().get_effects().size(),
		      "foo");
}


void EffectStackView::do_remove_effect() {
  if (!m_model || m_selected < 0 || 
      m_selected >= m_model->get_effect_stack_model().get_effects().size())
    return;
  m_signal_remove_effect(m_selected);
}


void EffectStackView::do_bypass_effect() {
  std::cerr<<__PRETTY_FUNCTION__<<std::endl;
  if (!m_model || m_selected < 0 || 
      m_selected >= m_model->get_effect_stack_model().get_effects().size())
    return;
  m_signal_bypass_effect(m_selected, 
			 !m_model->get_effect_stack_model().
			 get_effects()[m_selected].get_bypassed());
}
