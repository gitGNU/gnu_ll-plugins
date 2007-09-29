/****************************************************************************

    horizonkeyboard.hpp

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

#ifndef HORIZONKEYBOARD_HPP
#define HORIZONKEYBOARD_HPP

#include "keyboard.hpp"


class HorizonKeyboard : public Keyboard {
public:

  HorizonKeyboard();
  
  sigc::signal<void, const std::string&, unsigned, unsigned, unsigned char> 
  signal_segments_dropped;
  
protected:

  void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context,
			     int x, int y, const Gtk::SelectionData& data,
			     guint info, guint time);

};


#endif
