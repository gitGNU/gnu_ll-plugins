/****************************************************************************
    
    trilobyte_gtk.cpp - GUI for the Trilobyte sequencer
    
    Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
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
#include <string>

#include <gtkmm.h>

#include "lv2gtk2gui.hpp"
#include "trilobyte.peg"
#include "patternwidget.hpp"


using namespace sigc;
using namespace Gtk;
using namespace Gdk;
using namespace Glib;


class TrilobyteGUI : public LV2GTK2GUI {
public:
  
  TrilobyteGUI(LV2Controller& ctrl, const std::string& URI, 
               const std::string& bundle_path, Widget*& widget)
    : m_pat(32, 15, 15),
      m_ctrl(ctrl) {
    
    m_pat.signal_sequence_changed.
      connect(compose(bind<0>(mem_fun(ctrl, &LV2Controller::configure), "seq"),
                      mem_fun(m_pat, &PatternWidget::get_string)));
    
    RefPtr<Pixbuf> pixbuf = Pixbuf::create_from_file(bundle_path + 
                                                     "/patternbg.png");
    RefPtr<Pixmap> pixmap = Pixmap::create(m_pat.get_window(), 
                                           pixbuf->get_width(), 
                                           pixbuf->get_height(),
                                           Visual::get_best_depth());
    RefPtr<Bitmap> bitmap;
    pixbuf->render_pixmap_and_mask(pixmap, bitmap, 10);
    RefPtr<Style> s = m_pat.get_style()->copy();
    s->set_bg_pixmap(STATE_NORMAL, pixmap);
    s->set_bg_pixmap(STATE_ACTIVE, pixmap);
    s->set_bg_pixmap(STATE_PRELIGHT, pixmap);
    s->set_bg_pixmap(STATE_SELECTED, pixmap);
    s->set_bg_pixmap(STATE_INSENSITIVE, pixmap);
    m_pat.set_style(s);
    
    widget = &m_pat;
  }
  
protected:

  PatternWidget m_pat;
  LV2Controller& m_ctrl;
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2gtk2gui<TrilobyteGUI>(k_uri);
}
