#include <iostream>

#include <gtkmm.h>

#include "lv2gtk2gui.hpp"
#include "euphoriawidget.hpp"


using namespace std;
using namespace Gtk;
using namespace sigc;


class EuphoriaGUI : public LV2GTK2GUI {
public:
  
  EuphoriaGUI(LV2Controller& ctrl, const std::string& URI, 
              const std::string& bundle_path, Widget*& widget) {
    widget = &m_euph;
    m_euph.signal_control_changed.
      connect(mem_fun(ctrl, &LV2Controller::set_control));
  }
  
protected:

  EuphoriaWidget m_euph;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2gtk2gui<EuphoriaGUI>("http://ll-plugins.nongnu.org/lv2/dev/euphoria/0.0.0");
}
