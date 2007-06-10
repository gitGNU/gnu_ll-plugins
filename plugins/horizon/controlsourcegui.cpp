#include "controlsourcegui.hpp"


namespace {
  
  class MLabel : public Gtk::Label {
  public:
    MLabel(const std::string& str) 
      : Gtk::Label(str) {
      set_use_markup(true);
    }
  };

}




ControlSourceGUI::ControlSourceGUI() {
  LFO_GUI* lfo = manage(new LFO_GUI);
  lfo->set_border_width(3);
  MOD_GUI* mod = manage(new MOD_GUI);
  mod->set_border_width(3);
  append_page(*lfo, *manage(new MLabel("<b>LFOs</b>")));
  append_page(*mod, *manage(new MLabel("<b>MODs</b>")));
}
