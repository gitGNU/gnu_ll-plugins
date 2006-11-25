#ifndef LV2GTK2GUI_HPP
#define LV2GTK2GUI_HPP

#include <map>
#include <string>

#include <gtkmm.h>

#include "lv2-gtk2gui.h"


namespace LV2G2GSupportFunctions {

  template <class T> 
  LV2UI_Handle create_ui_instance(LV2UI_ControllerDescriptor*,
                                  LV2UI_Controller,
                                  const char*, const char*, GtkWidget**);
}


class LV2Controller {
public:
  
  LV2Controller();
  
  void set_control(uint32_t port, float value);
  
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

};


class LV2GTK2GUI {
public:
  
  virtual ~LV2GTK2GUI() { }
  
  virtual void set_control(uint32_t port, float value) { }
  
  virtual void* extension_data(const std::string& URI) { return 0; }

};


/* These functions are C wrappers for the C++ member functions. You should not
   use them directly. That's why they are intentionally documented without the
   Doxygen comment syntax. */
namespace LV2G2GSupportFunctions {
  
  typedef std::map<std::string, LV2UI_UIDescriptor> DescMap;
  
  /* This function returns the list of LV2 descriptors. It should only be 
     used internally. */
  DescMap& get_lv2g2g_descriptors();

  /* This template function creates an instance of a plugin. It is used as
     the instantiate() callback in the LV2 descriptor. You should not use
     it directly. */
  template <class T>
  LV2UI_Handle create_ui_instance(LV2UI_ControllerDescriptor* cdesc,
                                  LV2UI_Controller ctrl,
                                  const char* URI,
                                  const char* bundle_path,
                                  GtkWidget** widget) {
    LV2Controller controller(cdesc, ctrl);
    Gtk::Widget* widgetmm;
    T* t = new T(controller, URI, bundle_path, widgetmm);
    *widget = widgetmm->gobj();
    return reinterpret_cast<LV2UI_Handle>(t);
  }
  
  /* This function destroys an instance of a GUI. It is used as the
     cleanup() callback in the LV2UI descriptor. You should not use it
     directly. */
  void delete_ui_instance(LV2UI_Handle instance);
  
  void set_control(LV2UI_Handle instance, uint32_t port, float value);
  
  void* extension_data(LV2UI_Handle instance, const char* URI);
  
}  


template <typename T> void register_lv2gtk2gui(const std::string& URI) {
  using namespace LV2G2GSupportFunctions;
  LV2UI_UIDescriptor desc;
  std::memset(&desc, 0, sizeof(LV2UI_UIDescriptor));
  desc.instantiate = &create_ui_instance<T>;
  desc.cleanup = &delete_ui_instance;
  desc.set_control = &set_control;
  desc.extension_data = &extension_data;
  get_lv2g2g_descriptors()[URI] = desc;
}


#endif
