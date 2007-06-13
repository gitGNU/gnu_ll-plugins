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
