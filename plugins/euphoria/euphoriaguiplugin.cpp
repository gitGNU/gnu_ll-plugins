#include <iostream>

#include <gtkmm.h>

#include "lv2-gtk2gui.h"
#include "euphoriawidget.hpp"


using namespace std;
using namespace Gtk;


namespace {
  
  struct GUI {
    EuphoriaWidget* euph;
  };
  
  LV2UI_Handle instantiate(LV2UI_ControllerDescriptor*   descriptor,
                           LV2UI_Controller              controller,
                           const char*                   URI,
                           const char*                   bundle_path,
                           GtkWidget**                   widget) {
    cerr<<"instantiate: "<<URI<<", "<<bundle_path<<endl;
    GUI* gui = new GUI;
    gui->euph = new EuphoriaWidget;
    gui->euph->signal_control_changed.
      connect(sigc::bind<0>(descriptor->set_control, controller));
    *widget = GTK_WIDGET(gui->euph->gobj());
    return gui;
  }
  
  void cleanup(LV2UI_Handle ui) {
    cerr<<"cleanup"<<endl;
    GUI* gui = static_cast<GUI*>(ui);
    delete gui->euph;
    delete gui;
  }
  
  LV2UI_UIDescriptor desc = { &instantiate, &cleanup };
  
}


extern "C" {
  const LV2UI_UIDescriptor* lv2ui_descriptor(const char* URI) {
    if (!strcmp(URI, "http://ll-plugins.nongnu.org/lv2/dev/euphoria/0.0.0"))
      return &desc;
    return 0;
  }
}
