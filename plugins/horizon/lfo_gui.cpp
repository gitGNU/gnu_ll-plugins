#include "lfo_gui.hpp"
#include "optionslider.hpp"


namespace {
  
  class MLabel : public Gtk::Label {
  public:
    MLabel(const std::string& str) 
      : Gtk::Label(str) {
      set_use_markup(true);
    }
  };

}


LFO_GUI::LFO_GUI()
  : Table(10, 4) {

  set_spacings(3);
  std::vector<std::string> options;
  
  attach(*manage(new MLabel("<small><b>Low</b></small>")), 
	 1, 2, 0, 1, Gtk::FILL, Gtk::FILL);
  attach(*manage(new MLabel("<small><b>High</b></small>")), 
	 2, 3, 0, 1, Gtk::FILL, Gtk::FILL);
  attach(*manage(new MLabel("<small><b>Freq</b></small>")), 
	 3, 4, 0, 1, Gtk::FILL, Gtk::FILL);
  
  options.push_back("Use value");
  options.push_back("MOD1");
  options.push_back("MOD2");
  options.push_back("MOD3");
  options.push_back("MOD4");
  options.push_back("MOD5");
  options.push_back("MOD6");
  options.push_back("MOD7");
  options.push_back("MOD8");
  options.push_back("MOD9");
  
  char lfo_txt[] = "LFO0";
  char lbl_txt[] = "<small><b>0</b></small>";
  for (int i = 0; i < 9; ++i) {
    attach(*manage(new OptionSlider(0, 1, 0, 3, options)),
	   1, 2, i + 1, i + 2);
    attach(*manage(new OptionSlider(0, 1, 1, 3, options)),
	   2, 3, i + 1, i + 2);
    attach(*manage(new OptionSlider(0, 20, i, 2, options)),
	   3, 4, i + 1, i + 2);
    ++lfo_txt[3];
    options.push_back(lfo_txt);
    ++lbl_txt[10];
    attach(*manage(new MLabel(lbl_txt)), 
	   0, 1, i + 1, i + 2, Gtk::FILL, Gtk::FILL);
  }

}

