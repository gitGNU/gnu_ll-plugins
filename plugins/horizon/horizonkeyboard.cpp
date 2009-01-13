/****************************************************************************

    horizonkeyboard.cpp

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

#include <iostream>

#include "horizonkeyboard.hpp"
#include "actiontriggermodel.hpp"


using namespace Glib;
using namespace Gdk;
using namespace Gtk;
using namespace std;


HorizonKeyboard::HorizonKeyboard(ActionTriggerModel& atm)
  : m_atm(atm) {
  vector<TargetEntry> dnd_targets;
  dnd_targets.push_back(TargetEntry("x-org.nongnu.ll-plugins/horizon-segment"));
  drag_dest_set(dnd_targets, DEST_DEFAULT_ALL, Gdk::ACTION_LINK);
  int w, h;
  get_size_request(w, h);
  set_size_request(w, h + 10);
  sigc::slot<void> redraw = mem_fun(*this, &HorizonKeyboard::queue_draw);
  m_atm.signal_action_added.connect(sigc::hide(redraw));
}


void HorizonKeyboard::on_drag_data_received(const RefPtr<DragContext>& context,
					    int x, int y, 
					    const SelectionData& data,
					    guint info, guint time) {
  unsigned first = -1, last = -1;
  string str = data.get_data_as_string();
  string::size_type i = str.rfind(" ");
  if (i != string::npos)
    last = atol(str.substr(i + 1).c_str());
  string::size_type j = str.rfind(" ", i - 1);
  if (j != string::npos)
    first = atol(str.substr(j + 1, i - j).c_str());
  cerr<<data.get_data_type()<<": '"<<str.substr(0, j)<<"' "
      <<first<<" - "<<last<<endl;
  unsigned char key = pixel_to_key(x, y);
  if (first != -1 && last != -1 && key < 128)
    signal_segments_dropped(str.substr(0, j), first, last, key);
}


bool HorizonKeyboard::on_expose_event(GdkEventExpose* event) {
  Keyboard::on_expose_event(event);
  std::map<string, ActionTriggerModel::ActionModel*>::const_iterator iter;
  const std::map<string, ActionTriggerModel::ActionModel*>& actions = 
    m_atm.get_actions();
  for (iter = actions.begin(); iter != actions.end(); ++iter) {
    const unsigned char& k = iter->second->first;
    if (k != 255) {
      if (k >= m_octave_offset * 12 && 
	  k <= (m_octave_offset + m_octaves) * 12) {
	int x, y, w, h;
	key_to_rect(k, x, y, w, h);
	m_gc->set_foreground(m_green1);
	m_win->draw_rectangle(m_gc, true, x, m_keyheight + 2, 
			      w, get_height() - m_keyheight - 3);
	m_gc->set_foreground(m_green2);
	m_win->draw_rectangle(m_gc, false, x, m_keyheight + 2, 
			      w, get_height() - m_keyheight - 3);
      }
    }
  }
}
