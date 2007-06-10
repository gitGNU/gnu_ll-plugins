#include "mod_gui.hpp"
#include "labelslider.hpp"


namespace {
  
  class MLabel : public Gtk::Label {
  public:
    MLabel(const std::string& str) 
      : Gtk::Label(str) {
      set_use_markup(true);
    }
  };

}


MOD_GUI::MOD_GUI()
  : Table(10, 2) {

  set_spacings(3);
  std::vector<std::string> options;
  
  attach(*manage(new MLabel("<small><b>Value</b></small>")), 
	 1, 2, 0, 1, Gtk::FILL, Gtk::FILL);
  
  char lbl_txt[] = "<small><b>0</b></small>";
  for (int i = 0; i < 9; ++i) {
    attach(*manage(new LabelSlider(0, 1, 0.34, 3)),
	   1, 2, i + 1, i + 2);
    ++lbl_txt[10];
    attach(*manage(new MLabel(lbl_txt)), 
	   0, 1, i + 1, i + 2, Gtk::FILL, Gtk::FILL);
  }

}

