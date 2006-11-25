#include "lv2gtk2gui.hpp"


LV2Controller::LV2Controller()
  : m_cdesc(0),
    m_ctrl(0) {

}


void LV2Controller::set_control(uint32_t port, float value) {
  if (m_cdesc)
    m_cdesc->set_control(m_ctrl, port, value);
}
  

void* LV2Controller::extension_data(const std::string& URI) {
  if (m_cdesc)
    return m_cdesc->extension_data(m_ctrl, URI.c_str());
  return 0;
}


LV2Controller::LV2Controller(LV2UI_ControllerDescriptor* cdesc, 
                             LV2UI_Controller ctrl)
  : m_cdesc(cdesc),
    m_ctrl(ctrl) {

}


namespace LV2G2GSupportFunctions {
  

  DescMap& get_lv2g2g_descriptors() {
    static DescMap list;
    return list;
  }


  void delete_ui_instance(LV2UI_Handle instance) {
    delete static_cast<LV2GTK2GUI*>(instance);
  }
  
  
  void set_control(LV2UI_Handle instance, uint32_t port, float value) {
    static_cast<LV2GTK2GUI*>(instance)->set_control(port, value);
  }
  
  
  void* extension_data(LV2UI_Handle instance, const char* URI) {
    return static_cast<LV2GTK2GUI*>(instance)->extension_data(URI);
  }
  
  
}  


extern "C" {

  const LV2UI_UIDescriptor* lv2ui_descriptor(const char* URI) {
    using namespace LV2G2GSupportFunctions;
    DescMap descs = get_lv2g2g_descriptors();
    DescMap::const_iterator iter = descs.find(URI);
    if (iter == descs.end())
      return 0;
    return &iter->second;
  }

}

