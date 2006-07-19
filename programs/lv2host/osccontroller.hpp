#ifndef OSCCONTROLLER_HPP
#define OSCCONTROLLER_HPP

#include <string>

#include <lo/lo.h>

class LV2Host;


class OSCController {
public:
  
  OSCController(LV2Host& host, bool& still_running);
  
  void start();
  void stop();
  
  const std::string& get_url() const;
  
protected:
  
  static int update_handler(const char*, const char*, lo_arg** argv, int argc, 
                            lo_message, void* cbdata);
  static int control_handler(const char*, const char*, lo_arg** argv, 
                             int argc, lo_message, void* cbdata);
  static int exiting_handler(const char*, const char*, lo_arg** argv, 
                             int argc, lo_message, void* cbdata);
  
  
  LV2Host& m_host;
  
  lo_server_thread m_server;

  std::string m_url;
  
  struct CallbackData {
    CallbackData(LV2Host& h, OSCController& o) : host(h), me(o) { }
    LV2Host& host;
    OSCController& me;
  } m_cbdata;
  
  bool& m_still_running;
};


#endif
