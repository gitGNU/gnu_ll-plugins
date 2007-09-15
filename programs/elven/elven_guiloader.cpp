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


/** Write to an input port in the plugin. This function is exposed to 
    the plugin GUI. */
void write_function(LV2UI_Controller c, uint32_t port,
		    uint32_t buffer_size, const void* buffer) {
  static_cast<LV2UIClient*>(c)->
    send_control(port, *static_cast<const float*>(buffer));
}


/** Send a command to the plugin. This function is exposed to the 
    plugin GUI. */
void command_function(LV2UI_Controller c, 
		      uint32_t argc, const char* const* argv) {
  static_cast<LV2UIClient*>(c)->send_command(argc, argv);
}


/** Change the current program of the plugin. This function is exposed to
    the plugin GUI. */
void set_program(LV2UI_Controller c, unsigned char number) {
  static_cast<LV2UIClient*>(c)->send_program(number);
}


/** Wrapper for the port_event() GUI function. */
void control_port_event(uint32_t port, float value, 
			LV2UI_Handle handle, const LV2UI_Descriptor* desc) {
  desc->port_event(handle, port, sizeof(float), &value);
}


/** Return structs with additional function pointers to control the plugin.
    This function is exposed to the plugin GUI. */
void* extension_data(LV2UI_Controller c, const char* URI) {
  static LV2_ProgramControllerDescriptor progcdesc = {
    &set_program
  };
  if (!strcmp(URI, "http://ll-plugins.nongnu.org/lv2/namespace#program"))
    return &progcdesc;
  return 0;
}


void ui_feedback(uint32_t argc, const char* const* argv,
		 LV2UI_Handle handle, const LV2UI_Descriptor* desc) {
  desc->feedback(handle, argc, argv);
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
  
  LV2UI_Write_Function wfunc = &write_function;
  LV2UI_Command_Function cfunc = &command_function;
  
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
  Glib::signal_idle().
    connect(bind_return(mem_fun(osc, &LV2UIClient::run), true));
  
  // build the feature list
  LV2_Host_Feature** features = new LV2_Host_Feature*[2];
  features[0] = new LV2_Host_Feature;
  features[0]->URI = strdup("http://ll-plugins.nongnu.org/lv2/namespace#program");
  static LV2_ProgramControllerDescriptor progcdesc = {
    &set_program
  };
  features[0]->data = &progcdesc;
  features[1] = 0;

  
  // create a GUI instance
  GtkWidget* cwidget;
  LV2UI_Controller ctrl = static_cast<LV2UI_Controller>(&osc);
  LV2UI_Handle ui = desc->instantiate(desc, plugin_uri, bundle_path, wfunc, 
				      cfunc, ctrl, &cwidget, 
				      const_cast<const LV2_Host_Feature**>(features));
  if (!ui || !cwidget) {
    DBG0("Could not create an UI instance");
    return 1;
  }
  
  // connect OSC server to UI
  if (desc->port_event) {
    osc.control_received.connect(bind(bind(ptr_fun(&control_port_event), desc), ui));
    if (desc->feedback)
      osc.feedback_received.connect(bind(bind(&ui_feedback, desc), ui));
  }
  
  // connect any eventual program handling functions to the OSC signals
  if (desc->extension_data) {
    const LV2_ProgramUIDescriptor* progdesc = 
      static_cast<LV2_ProgramUIDescriptor*>
      (desc->extension_data(ui, "http://ll-plugins.nongnu.org/lv2/namespace#program"));
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
