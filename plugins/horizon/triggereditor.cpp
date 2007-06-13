#include "triggereditor.hpp"


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


TriggerEditor::TriggerEditor() {
  
  set_shadow_type(SHADOW_IN);
  set_label_widget(*manage(new MLabel("<b>Triggers</b>")));
  
  VBox* vbox = manage(new VBox(false, 6));
  vbox->set_border_width(6);
  
  HBox* top_row = manage(new HBox(false, 6));
  top_row->pack_start(*manage(new MLabel("<small><b>Keys:</b></small>")),
		      false, false);
  top_row->pack_start(*manage(new SpinButton), false, false);
  top_row->pack_start(*manage(new MLabel("<small>to</small>")),
		      false, false);
  top_row->pack_start(*manage(new SpinButton), false, false);
  ComboBoxText* cbt = manage(new ComboBoxText);
  cbt->append_text("Play chunk");
  cbt->append_text("Reset LFO");
  cbt->append_text("Modify parameter");
  top_row->pack_end(*cbt, false, false);
  top_row->pack_end(*manage(new MLabel("<small><b>Action:</b></small>")),
		    false, false);
  
  // "Play chunk"
  HBox* pchk_box = manage(new HBox(false, 6));
  ComboBoxText* chkcbt = manage(new ComboBoxText);
  chkcbt->append_text("No chunks");
  pchk_box->pack_start(*manage(new MLabel("<small><b>Chunk:</b></small>")),
		       false, false);
  pchk_box->pack_start(*chkcbt, false, false);
  ComboBoxText* modcbt = manage(new ComboBoxText);
  modcbt->append_text("One shot");
  modcbt->append_text("Normal");
  modcbt->append_text("Ping-pong");
  pchk_box->pack_end(*modcbt, false, false);
  pchk_box->pack_end(*manage(new MLabel("<small><b>Loop:</b></small>")),
		     false, false);
  
  // "Reset LFO"
  HBox* rlfo_box = manage(new HBox(false, 6));
  ComboBoxText* lfocbt = manage(new ComboBoxText);
  char tmp[] = "LFO0";
  for (int i = 0; i < 9; ++i) {
    ++tmp[3];
    lfocbt->append_text(tmp);
  }
  rlfo_box->pack_start(*manage(new MLabel("<small><b>LFO:</b></small>")), 
					  false, false);
  rlfo_box->pack_start(*lfocbt, false, false);
  SpinButton* phssbn = manage(new SpinButton);
  phssbn->set_range(0, 2*3.14159);
  phssbn->set_digits(4);
  phssbn->set_increments(0.01, 3.14159/2);
  phssbn->set_numeric(true);
  rlfo_box->pack_end(*phssbn, false, false);
  rlfo_box->pack_end(*manage(new MLabel("<small><b>Phase:</b></small>")),
		     false, false);
  
  // "Modify parameter"
  HBox* mpar_box = manage(new HBox(false, 6));
  ComboBoxText* parcbt = manage(new ComboBoxText);
  parcbt->append_text("No parameters");
  mpar_box->pack_start(*manage(new MLabel("<small><b>Parameter:</b></small>")),
		       false, false);
  mpar_box->pack_start(*parcbt, false, false);
  SpinButton* parsbn = manage(new SpinButton);
  parsbn->set_range(-100, 100);
  parsbn->set_increments(0.1, 1);
  parsbn->set_digits(4);
  parsbn->set_numeric(true);
  ComboBoxText* opercbt = manage(new ComboBoxText);
  opercbt->append_text("*");
  opercbt->append_text("+");
  opercbt->append_text("=");
  mpar_box->pack_end(*parsbn, false, false);
  mpar_box->pack_end(*opercbt, false, false);
  
  HBox* bottom_row = manage(new HBox(false, 6));
  ComboBoxText* cbt2 = manage(new ComboBoxText);
  cbt2->append_text("No triggers");
  bottom_row->pack_start(*cbt2, false, false);
  bottom_row->pack_end(*manage(new Button("New")), false, false);
  bottom_row->pack_end(*manage(new Button("Delete")), false, false);
  bottom_row->pack_end(*manage(new Button("Rename")), false, false);
  
  
  vbox->pack_start(*top_row, false, false);
  Notebook* nbk = manage(new Notebook);
  nbk->set_show_tabs(false);
  nbk->set_show_border(false);
  nbk->append_page(*pchk_box);
  nbk->append_page(*rlfo_box);
  nbk->append_page(*mpar_box);
  vbox->pack_start(*nbk, false, false);
  vbox->pack_end(*bottom_row, false, false);
  
  using namespace sigc;
  
  cbt->signal_changed().
    connect(compose(mem_fun(*nbk, &Notebook::set_current_page),
		    mem_fun(*cbt, &ComboBox::get_active_row_number)));
  
  add(*vbox);
}

