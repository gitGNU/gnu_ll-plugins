#include <iostream>
#include <dlfcn.h>

#include <gtkmm.h>

#include "lv2-gtk2gui.h"


using namespace std;
using namespace Gtk;


void set_control(LV2UI_Controller c, uint32_t port, float value) {
  cerr<<"set_control: "<<port<<", "<<value<<endl;
}


void* extension_data(LV2UI_Controller c, const char* URI) {
  return 0;
}


int main(int argc, char** argv) {
  
  // initialise GTK
  Gtk::Main kit(argc, argv);
  
  if (argc < 5) {
    cerr<<"Not enough parameters."<<endl;
    return 1;
  }
  
  const char* filename = argv[1];
  const char* uri = argv[2];
  const char* bundle_path = argv[3];
  const char* name = argv[4];
  
  LV2UI_ControllerDescriptor cdesc = {
    &set_control,
    &extension_data
  };
  
  // open the plugin
  void* module = dlopen(filename, RTLD_LAZY);
  if (!module) {
    cerr<<"Could not load "<<filename<<endl;
    return 1;
  }
  
  // get the GUI descriptor
  LV2UI_UIDescriptorFunction func = 
    (LV2UI_UIDescriptorFunction)dlsym(module, "lv2ui_descriptor");
  if (!func) {
    cerr<<"Could not find symbol lv2ui_descriptor in "<<filename<<endl;
    return 1;
  }
  const LV2UI_UIDescriptor* desc = func(uri);
  if (!desc) {
    cerr<<"lv2ui_descriptor() returned NULL"<<endl;
    return 1;
  }
  
  // create a GUI instance
  GtkWidget* cwidget;
  LV2UI_Handle ui = desc->instantiate(&cdesc, 0, uri, bundle_path, &cwidget);
  if (!ui || !cwidget) {
    cerr<<"Could not create an UI"<<endl;
    return 1;
  }
  
  // show the GUI instance and start the main loop
  Window win;
  win.set_title(name);
  win.add(*Glib::wrap(cwidget));
  win.show_all();
  kit.run(win);
  
  // clean up
  win.remove();
  desc->cleanup(ui);
  dlclose(module);
  
  return 0;
}
