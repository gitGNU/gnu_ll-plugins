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


using namespace Glib;
using namespace Gdk;
using namespace Gtk;
using namespace std;


HorizonKeyboard::HorizonKeyboard() {
  vector<TargetEntry> dnd_targets;
  dnd_targets.push_back(TargetEntry("x-org.nongnu.ll-plugins/horizon-segment"));
  drag_dest_set(dnd_targets, DEST_DEFAULT_ALL, Gdk::ACTION_LINK);
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
