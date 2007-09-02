/****************************************************************************

    chunkeditor.cpp

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

#include "chunkeditor.hpp"


using namespace Gtk;


namespace {
  
  class MLabel : public Gtk::Label {
  public:
    MLabel(const std::string& str) 
      : Gtk::Label(str) {
      set_use_markup(true);
    }
  };

}


ChunkEditor::ChunkEditor() {
  
  set_shadow_type(SHADOW_IN);
  set_label_widget(*manage(new MLabel("<b>Chunks</b>")));
  
  VBox* vbox = manage(new VBox(false, 6));
  vbox->set_border_width(6);
  
  HBox* top_row = manage(new HBox(false, 6));
  top_row->pack_start(*manage(new MLabel("<small><b>Sample:</b></small>")),
		      false, false);
  ComboBoxText* smpcbt = manage(new ComboBoxText);
  smpcbt->append_text("No samples");
  top_row->pack_start(*smpcbt, false, false);
  top_row->pack_end(*manage(new SpinButton), false, false);
  top_row->pack_end(*manage(new MLabel("<small>to</small>")), false, false);
  top_row->pack_end(*manage(new SpinButton), false, false);
  top_row->pack_end(*manage(new MLabel("<small><b>Segments:</b></small>")),
		    false, false);
  
  HBox* bottom_row = manage(new HBox(false, 6));
  ComboBoxText* cbt2 = manage(new ComboBoxText);
  cbt2->append_text("No chunks");
  bottom_row->pack_start(*cbt2, false, false);
  bottom_row->pack_end(*manage(new Button("New")), false, false);
  bottom_row->pack_end(*manage(new Button("Delete")), false, false);
  bottom_row->pack_end(*manage(new Button("Rename")), false, false);
  

  vbox->pack_start(*top_row, false, false);
  vbox->pack_end(*bottom_row, false, false);
  
  add(*vbox);
}
