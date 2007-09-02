/****************************************************************************
    
    elven_guiloader.cpp - A program that loads LV2 GUI plugins and can be
                          controlled via OSC
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA

****************************************************************************/

#include <iostream>
#include <dlfcn.h>

#include <gtkmm.h>
#include <gtkmm/plug.h>

#include "lv2-gtk2gui.h"
#include "lv2-instrument-gtk2gui.h"
#include "lv2-guicomm-gtk2gui.h"
#include "lv2-miditype-gtk2gui.h"
#include "lv2-program-gtk2gui.h"
#include "lv2uiclient.hpp"
#include "debug.hpp"


using namespace std;
using namespace Gtk;
using namespace sigc;


/** This is needed to cast void* (returned by dlsym()) to a function
    pointer. */
template <typename A, typename B> A nasty_cast(B b) {
  union {
    A a;
    B b;
  } u;
  u.b = b;
  return u.a;
}


/** Change a control port value in the plugin. This function is exposed to
    the plugin GUI. */
void set_control(LV2UI_Controller c, uint32_t port, float value) {
  static_cast<LV2UIClient*>(c)->send_control(port, value);
}


/** Change the configuration of the plugin. This function is exposed to
    the plugin GUI. */
void configure(LV2UI_Controller c, const char* key, const char* value) {
  static_cast<LV2UIClient*>(c)->send_configure(key, value);
}


/** Change a data file in the plugin. This function is exposed to the 
    plugin GUI. */
void set_file(LV2UI_Controller c, const char* key, const char* filename) {
  static_cast<LV2UIClient*>(c)->send_filename(key, filename);
}


/** Send a command to the plugin. This function is exposed to the plugin GUI. 
 */
void tell_plugin(LV2UI_Controller c, uint32_t argc, const char* const* argv) {
  cerr<<__PRETTY_FUNCTION__<<endl;
  static_cast<LV2UIClient*>(c)->send_tell_plugin(argc, argv);
}


/** Send a MIDI event to the plugin. This function is exposed to the plugin
    GUI. */
void send_midi(LV2UI_Controller c, uint32_t port, uint32_t size,
               const unsigned char* data) {
  static_cast<LV2UIClient*>(c)->send_midi(port, size, data);
}


/** Change the current program of the plugin. This function is exposed to
    the plugin GUI. */
void set_program(LV2UI_Controller c, unsigned char number) {
  static_cast<LV2UIClient*>(c)->send_program(number);
}


/** Return structs with additional function pointers to control the plugin.
    This function is exposed to the plugin GUI. */
void* extension_data(LV2UI_Controller c, const char* URI) {
  static LV2_InstrumentControllerDescriptor instcdesc = {
    &configure,
    &set_file
  };
  static LV2_ProgramControllerDescriptor progcdesc = {
    &set_program
  };
  static LV2_MIDIControllerDescriptor midicdesc = {
    &send_midi
  };
  static LV2_GUICommControllerDescriptor commcdesc = {
    &tell_plugin
  };
  
  if (!strcmp(URI, "http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext"))
    return &instcdesc;
  if (!strcmp(URI, "http://ll-plugins.nongnu.org/lv2/namespace#program"))
    return &progcdesc;
  if (!strcmp(URI, "http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension"))
    return &commcdesc;
  if (!strcmp(URI, "http://ll-plugins.nongnu.org/lv2/ext/miditype"))
    return &midicdesc;
  return 0;
}


void ui_configure(const std::string& key, const std::string& value,
                  LV2UI_Handle handle, 
                  const LV2_InstrumentUIDescriptor* instdesc) {
  instdesc->configure(handle, key.c_str(), value.c_str());
}


void ui_set_file(const std::string& key, const std::string& filename,
                 LV2UI_Handle handle, 
                 const LV2_InstrumentUIDescriptor* instdesc) {
  instdesc->set_file(handle, key.c_str(), filename.c_str());
}


void ui_tell_gui(uint32_t argc, const char* const* argv,
		 LV2UI_Handle handle, 
                 const LV2_GUICommUIDescriptor* commdesc) {
  commdesc->tell_gui(handle, argc, argv);
}


void ui_add_program(int number, const std::string& name, LV2UI_Handle handle,
                    const LV2_ProgramUIDescriptor* progdesc) {
  progdesc->add_program(handle, number, name.c_str());
}

// XXX tell_gui


int main(int argc, char** argv) {
  
  DebugInfo::prefix() = "GUI ";
  
  // initialise GTK
  Gtk::Main kit(argc, argv);
  
  if (argc < 8) {
    DBG0("Not enough parameters.");
    return 1;
  }

  DebugInfo::level() = atoi(argv[1]);
  
  const char* filename = argv[2];
  const char* osc_url = argv[3];
  const char* gui_uri = argv[4];
  const char* plugin_uri = argv[5];
  const char* bundle_path = argv[6];
  const char* name = argv[7];
  const char* icon_path = 0;
  // XXX handle socket ID somehow
  int socket_id = -1;
  if (argc > 8)
    icon_path = argv[8];
  
  LV2UI_Set_Control_Function cfunc = &set_control;
  
  // open the module
  DBG2("Loading "<<filename);
  void* module = dlopen(filename, RTLD_LAZY);
  if (!module) {
    DBG0("Could not load "<<filename<<": "<<dlerror());
    return 1;
  }
  
  // get the GUI descriptor
  LV2UI_DescriptorFunction func = 
    nasty_cast<LV2UI_DescriptorFunction>(dlsym(module, "lv2ui_descriptor"));
  if (!func) {
    DBG0("Could not find symbol lv2ui_descriptor in "<<filename);
    return 1;
  }
  const LV2UI_Descriptor* desc = 0;
  for (unsigned i = 0; (desc = func(i)); ++i) {
    DBG2("Found GUI "<<desc->URI);
    if (!strcmp(gui_uri, desc->URI))
      break;
    DBG2(desc->URI<<" does not match "<<gui_uri);
  }
  if (!desc) {
    DBG0(filename<<" did not contain the plugin GUI "<<gui_uri);
    return 1;
  }
  
  // create an OSC server
  DBG2("Creating OSC server...");
  LV2UIClient osc(osc_url, bundle_path, name, true);
  if (!osc.is_valid()) {
    DBG0("Could not start an OSC server");
    return 1;
  }
  
  // build the feature list
  LV2_Host_Feature** features = new LV2_Host_Feature*[5];
  features[0] = new LV2_Host_Feature;
  features[0]->URI = strdup("http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext");
  static LV2_InstrumentControllerDescriptor instcdesc = {
    &configure,
    &set_file
  };
  features[0]->data = &instcdesc;
  features[1] = new LV2_Host_Feature;
  features[1]->URI = strdup("http://ll-plugins.nongnu.org/lv2/namespace#program");
  static LV2_ProgramControllerDescriptor progcdesc = {
    &set_program
  };
  features[1]->data = &progcdesc;
  features[2] = new LV2_Host_Feature;
  features[2]->URI = strdup("http://ll-plugins.nongnu.org/lv2/ext/miditype");
  static LV2_MIDIControllerDescriptor midicdesc = {
    &send_midi
  };
  features[2]->data = &midicdesc;
  features[3] = new LV2_Host_Feature;
  features[3]->URI = strdup("http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension");
  static LV2_GUICommControllerDescriptor commcdesc = {
    &tell_plugin
  };
  features[3]->data = &commcdesc;
  
  features[4] = 0;

  
  // create a GUI instance
  GtkWidget* cwidget;
  LV2UI_Controller ctrl = static_cast<LV2UI_Controller>(&osc);
  LV2UI_Handle ui = desc->instantiate(desc, plugin_uri, bundle_path, cfunc, 
				      ctrl, &cwidget, 
				      const_cast<const LV2_Host_Feature**>(features));
  if (!ui || !cwidget) {
    DBG0("Could not create an UI instance");
    return 1;
  }
  
  // connect OSC server to UI
  if (desc->set_control) {
    osc.control_received.connect(bind<0>(desc->set_control, ui));
  }
  if (desc->extension_data) {
    const LV2_InstrumentUIDescriptor* instdesc = static_cast<LV2_InstrumentUIDescriptor*>(desc->extension_data(ui, "http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext"));
    if (instdesc) {
      DBG2("The GUI uses the instrument extension");
      if (instdesc->configure) {
        osc.configure_received.connect(bind(bind(&ui_configure, instdesc), ui));
      }
      if (instdesc->set_file)
        osc.filename_received.connect(bind(bind(&ui_set_file, instdesc), ui));
    }
    const LV2_GUICommUIDescriptor* commdesc = static_cast<LV2_GUICommUIDescriptor*>(desc->extension_data(ui, "http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension"));
    if (commdesc) {
      DBG2("The GUI uses the command extension");
      if (commdesc->tell_gui)
        osc.tell_gui_received.connect(bind(bind(&ui_tell_gui, commdesc), ui));
    }
    const LV2_ProgramUIDescriptor* progdesc = static_cast<LV2_ProgramUIDescriptor*>(desc->extension_data(ui, "http://ll-plugins.nongnu.org/lv2/namespace#program"));
    if (progdesc) {
      DBG2("The GUI uses the program extension");
      if (progdesc->add_program) {
        osc.add_program_received.
          connect(bind(bind(&ui_add_program, progdesc), ui));
      }
      if (progdesc->remove_program) {
        osc.remove_program_received.
          connect(bind<0>(progdesc->remove_program, ui));
      }
      if (progdesc->clear_programs) {
        osc.clear_programs_received.connect(bind(progdesc->clear_programs, ui));
      }
      if (progdesc->set_program) {
        osc.program_received.
          connect(bind<0>(progdesc->set_program, ui));
      }
    }
  }
  
  // show the GUI instance and start the main loop
  if (socket_id == -1) {
    Window win;
    win.set_title(name);
    win.add(*Glib::wrap(cwidget));
    if (icon_path) {
      DBG2("Loading icon from "<<icon_path);
      win.set_icon(Gdk::Pixbuf::create_from_file(icon_path));
    }
    win.show_all();
    osc.send_update_request();
    DBG2("Starting GUI in its own window");
    kit.run(win);
  }
  else {
    Plug plug(socket_id);
    plug.add(*Glib::wrap(cwidget));
    osc.send_update_request();
    DBG2("Starting GUI in an embeddable plug");
    kit.run(plug);
  }
  
  // clean up
  desc->cleanup(ui);
  dlclose(module);
  
  DBG2("elven_guiloader is exiting");
  
  return 0;
}
