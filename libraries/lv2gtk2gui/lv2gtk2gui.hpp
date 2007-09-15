/****************************************************************************
    
    lv2gtk2gui.hpp - Wrapper library to make it easier to write LV2 GUIs
                     in C++
    
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

#ifndef LV2GTK2GUI_HPP
#define LV2GTK2GUI_HPP

#include <map>
#include <string>

#include <gtkmm.h>

#include "lv2-gtk2gui.h"
#include "lv2-instrument-gtk2gui.h"
#include "lv2-guicomm-gtk2gui.h"
#include "lv2-program-gtk2gui.h"
#include "lv2-miditype-gtk2gui.h"


namespace LV2G2GSupportFunctions {
  
  template <class T>
  LV2UI_Handle create_ui_instance(const struct _LV2UI_Descriptor* descriptor,
				  const char*                     plugin_uri,
				  const char*                     bundle_path,
				  LV2UI_Write_Function            write_func,
				  LV2UI_Command_Function          command_func,
				  LV2UI_Controller                ctrl,
				  GtkWidget**                     widget,
				  const LV2_Host_Feature**        features);
}


/** This class is an interface for controlling a plugin instance. An object
    of this class will be provided by the host when a plugin GUI is 
    instantiated. */
class LV2Controller {
public:
  
  /** This constructor creates an invalid controller that doesn't actually
      control anything. */
  LV2Controller();
  
  /** Set the value of a control rate float port in the plugin instance. */
  void write(uint32_t port, uint32_t buffer_size, const void* buffer);
  
  void command(uint32_t argc, const char* const* argv);
  
  /** Tell the plugin host to switch to program @c number for the plugin
      instance. */
  void set_program(unsigned char number);
  
  /** Return data associated with an extension URI, or 0 if that extension
      is not supported or does not have any data for use in controllers. */
  void* extension_data(const std::string& URI);
  
protected:
  
  template <class T> friend
  LV2UI_Handle LV2G2GSupportFunctions::
  create_ui_instance(const struct _LV2UI_Descriptor* descriptor,
		     const char*                     plugin_uri,
		     const char*                     bundle_path,
		     LV2UI_Write_Function            write_function,
		     LV2UI_Command_Function          command_function,
		     LV2UI_Controller                ctrl,
		     GtkWidget**                     widget,
		     const LV2_Host_Feature**        features);

  LV2Controller(LV2UI_Write_Function wfcn, LV2UI_Command_Function cfcn,
		LV2UI_Controller ctrl, const LV2_Host_Feature** features);
  
  LV2UI_Write_Function m_wfunc;
  LV2UI_Command_Function m_cfunc;
  LV2UI_Controller m_ctrl;
  LV2_InstrumentControllerDescriptor* m_instdesc;
  LV2_GUICommControllerDescriptor* m_commdesc;
  LV2_ProgramControllerDescriptor* m_progdesc;
  LV2_MIDIControllerDescriptor* m_mididesc;
};


//class LV2Controller;


/* These functions are C wrappers for the C++ member functions. You should not
   use them directly. That's why they are intentionally documented without the
   Doxygen comment syntax. */
namespace LV2G2GSupportFunctions {
  
  typedef std::vector<LV2UI_Descriptor*> DescList;
  
  /* This function returns the list of LV2 descriptors. It should only be 
     used internally. */
  DescList& get_lv2g2g_descriptors();

  /* This template function creates an instance of a plugin GUI. It is used 
     as the instantiate() callback in the LV2 descriptor. You should not use
     it directly. */
  template <class T>
  LV2UI_Handle create_ui_instance(const struct _LV2UI_Descriptor* descriptor,
				  const char*                     plugin_uri,
				  const char*                     bundle_path,
				  LV2UI_Write_Function            write_func,
				  LV2UI_Command_Function          command_func,
				  LV2UI_Controller                ctrl,
				  GtkWidget**                     widget,
				  const LV2_Host_Feature**        features) {
    
    // this is needed to initialise gtkmm stuff in case we're running in
    // a Gtk+ or PyGtk host or some other language
    Gtk::Main::init_gtkmm_internals();
    
    LV2Controller* controller = new LV2Controller(write_func, command_func,
						  ctrl, features);
    T* t = new T(*controller, plugin_uri, bundle_path);
    t->m_controller = controller;
    *widget = static_cast<Gtk::Widget*>(t)->gobj();
    return reinterpret_cast<LV2UI_Handle>(t);
  }
  
  /* This function destroys an instance of a GUI. It is used as the
     cleanup() callback in the LV2UI descriptor. You should not use it
     directly. */
  void delete_ui_instance(LV2UI_Handle instance);
  
  /* Tell the GUI that a control rate float port has changed. You should
     not use this directly. */
  void port_event(LV2UI_Handle instance, uint32_t port, uint32_t buffer_size,
		  const void* buffer);
  
  void feedback(LV2UI_Handle instance, uint32_t argc, const char* const* argv);
  
  void add_program(LV2UI_Handle instance, unsigned char number, 
                   const char* name);
  
  void remove_program(LV2UI_Handle instance, unsigned char number);
  
  void clear_programs(LV2UI_Handle instance);
  
  void set_program(LV2UI_Handle instance, unsigned char number);
  
  /* Try to access extension-specific data. You should not use this directly. */
  void* extension_data(LV2UI_Handle instance, const char* URI);
  
}  


/** This is the base class for a plugin GUI. You should inherit it and 
    override any functions you want to provide implementations for. 
    All subclasses must have a constructor with the signature
    
    MyGUI(LV2Controller& controller, const char* plugin_URI, 
          const char* bundle_path, Gtk::Widget*& UI_widget);
    
    Where @c controller is the interface for controlling the plugin instance
    and @c UI_widget should be set to point to a widget that the host will
    display to the user. The plugin is responsible for deallocating the 
    widget when the destructor for the LV2GTK2GUI subclass is called. */
class LV2GTK2GUI : public Gtk::HBox {
public:
  
  virtual ~LV2GTK2GUI() { delete m_controller; }
  
  /** Override this if you want your GUI to do something when a control port
      value changes in the plugin instance. */
  virtual void port_event(uint32_t port, uint32_t buffer_size, 
			  const void* buffer) { }
  
  /** Override this if you want your GUI to do something when a piece of
      configuration data changes in the plugin instance. */
  virtual void configure(const char* key, const char* value) { }
  
  /** Override this is you want your GUI to do something when a filename
      changes in the plugin instance. */
  virtual void set_file(const char* key, const char* filename) { }
  
  virtual void feedback(uint32_t argc, const char* const* argv) { }
  
  /** Override this if you want your GUI to do something when a program has
      been added for the plugin instance. */
  virtual void add_program(unsigned char number, const char* name) { }
  
  /** Override this if you want your GUI to do something when a program has
      been removed for the plugin instance. */
  virtual void remove_program(unsigned char number) { }
  
  /** Override this if you want your GUI to do something when the host
      removes all programs for the plugin instance. */
  virtual void clear_programs() { }
  
  /** Override this if you want your GUI to do something when the host
      changes the program for the plugin instance. */
  virtual void set_program(unsigned char number) { }
  
  /** Return data associated with an extension URI, or 0 if that extension
      is not supported or does not have any data for use in plugin GUIs. */
  virtual void* extension_data(const std::string& URI) { return 0; }

private:
  
  template <class T> friend
  LV2UI_Handle LV2G2GSupportFunctions::
  create_ui_instance(const struct _LV2UI_Descriptor* descriptor,
		     const char*                     plugin_uri,
		     const char*                     bundle_path,
		     LV2UI_Write_Function            write_function,
		     LV2UI_Command_Function          command_function,
		     LV2UI_Controller                ctrl,
		     GtkWidget**                     widget,
		     const LV2_Host_Feature**        features);

  friend void* LV2G2GSupportFunctions::extension_data(LV2UI_Handle instance, 
                                                      const char* URI);

  LV2Controller* m_controller;

  static LV2_InstrumentUIDescriptor m_instrument_ui_desc;
  static LV2_ProgramUIDescriptor m_program_ui_desc;
  static LV2_GUICommUIDescriptor m_guicomm_ui_desc;
  
};


template <typename T> void register_lv2gtk2gui(const std::string& URI) {
  using namespace LV2G2GSupportFunctions;
  LV2UI_Descriptor* desc = new LV2UI_Descriptor;
  std::memset(desc, 0, sizeof(LV2UI_Descriptor));
  desc->URI = strdup(URI.c_str());
  desc->instantiate = &create_ui_instance<T>;
  desc->cleanup = &delete_ui_instance;
  desc->port_event = &port_event;
  desc->feedback = &feedback;
  desc->extension_data = &extension_data;
  get_lv2g2g_descriptors().push_back(desc);
}


#endif
