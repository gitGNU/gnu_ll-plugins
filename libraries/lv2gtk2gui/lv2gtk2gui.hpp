/****************************************************************************
    
    lv2gtk2gui.hpp - Wrapper library to make it easier to write LV2 GUIs
                     in C++
    
    Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
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


class LV2Controller;


/* These functions are C wrappers for the C++ member functions. You should not
   use them directly. That's why they are intentionally documented without the
   Doxygen comment syntax. */
namespace LV2G2GSupportFunctions {
  
  typedef std::map<std::string, LV2UI_UIDescriptor*> DescMap;
  
  /* This function returns the list of LV2 descriptors. It should only be 
     used internally. */
  DescMap& get_lv2g2g_descriptors();

  /* This template function creates an instance of a plugin GUI. It is used 
     as the instantiate() callback in the LV2 descriptor. You should not use
     it directly. */
  template <class T>
  LV2UI_Handle create_ui_instance(LV2UI_ControllerDescriptor* cdesc,
                                  LV2UI_Controller ctrl,
                                  const char* URI,
                                  const char* bundle_path,
                                  GtkWidget** widget) {
    LV2Controller* controller = new LV2Controller(cdesc, ctrl);
    Gtk::Widget* widgetmm;
    T* t = new T(*controller, URI, bundle_path, widgetmm);
    t->m_controller = controller;
    *widget = widgetmm->gobj();
    return reinterpret_cast<LV2UI_Handle>(t);
  }
  
  /* This function destroys an instance of a GUI. It is used as the
     cleanup() callback in the LV2UI descriptor. You should not use it
     directly. */
  void delete_ui_instance(LV2UI_Handle instance);
  
  /* Tell the GUI that a control rate float port has changed. You should
     not use this directly. */
  void set_control(LV2UI_Handle instance, uint32_t port, float value);
  
  void configure(LV2UI_Handle instance, const char* key, const char* value);

  void set_file(LV2UI_Handle instance, const char* key, const char* filename);
  
  /* Try to access extension-specific data. You should not use this directly. */
  void* extension_data(LV2UI_Handle instance, const char* URI);
  
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
  void set_control(uint32_t port, float value);
  
  void configure(const std::string& key, const std::string& value);
  
  void set_file(const std::string& key, const std::string& filename);
  
  /** Return data associated with an extension URI, or 0 if that extension
      is not supported or does not have any data for use in controllers. */
  void* extension_data(const std::string& URI);
  
protected:
  
  template <class T> friend LV2UI_Handle 
  LV2G2GSupportFunctions::create_ui_instance(LV2UI_ControllerDescriptor*,
                                             LV2UI_Controller,
                                             const char*, const char*, 
                                             GtkWidget**);

  LV2Controller(LV2UI_ControllerDescriptor* cdesc, LV2UI_Controller ctrl);
  
  LV2UI_ControllerDescriptor* m_cdesc;
  LV2UI_Controller m_ctrl;
  LV2_InstrumentControllerDescriptor* m_instdesc;
};


/** This is the base class for a plugin GUI. You should inherit it and 
    override any functions you want to provide implementations for. 
    All subclasses must have a constructor with the signature
    
    MyGUI(LV2Controller& controller, const char* plugin_URI, 
          const char* bundle_path, Gtk::Widget*& UI_widget);
    
    Where @c controller is the interface for controlling the plugin instance
    and @c UI_widget should be set to point to a widget that the host will
    display to the user. The plugin is responsible for deallocating the 
    widget when the destructor for the LV2GTK2GUI subclass is called. */
class LV2GTK2GUI {
public:
  
  virtual ~LV2GTK2GUI() { delete m_controller; }
  
  virtual void set_control(uint32_t port, float value) { }
   
  virtual void configure(const char* key, const char* value) { }
  
  virtual void set_file(const char* key, const char* filename) { }
  
  virtual void* extension_data(const std::string& URI) { return 0; }

private:
  
  template <class T> friend LV2UI_Handle 
  LV2G2GSupportFunctions::create_ui_instance(LV2UI_ControllerDescriptor*,
                                             LV2UI_Controller,
                                             const char*, const char*, 
                                             GtkWidget**);
  
  friend void* LV2G2GSupportFunctions::extension_data(LV2UI_Handle instance, 
                                                      const char* URI);

  LV2Controller* m_controller;

  static LV2_InstrumentUIDescriptor m_instrument_ui_desc;
  
};


template <typename T> void register_lv2gtk2gui(const std::string& URI) {
  using namespace LV2G2GSupportFunctions;
  LV2UI_UIDescriptor* desc = new LV2UI_UIDescriptor;
  std::memset(desc, 0, sizeof(LV2UI_UIDescriptor));
  desc->instantiate = &create_ui_instance<T>;
  desc->cleanup = &delete_ui_instance;
  desc->set_control = &set_control;
  desc->extension_data = &extension_data;
  get_lv2g2g_descriptors()[URI] = desc;
}


#endif
