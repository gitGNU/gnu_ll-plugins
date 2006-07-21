#ifndef OSCCONTROLLER_HPP
#define OSCCONTROLLER_HPP

#include <string>
#include <vector>

#include <pthread.h>
#include <lo/lo.h>

#include "lv2host.hpp"
#include "eventqueue.hpp"


class OSCController {
public:
  
  OSCController(LV2Host& host, bool& still_running);
  ~OSCController();
  
  void start();
  void stop();
  
  const std::string& get_url() const;
  
protected:
  
  static int update_handler(const char*, const char*, lo_arg** argv, int argc, 
                            lo_message, void* cbdata);
  static int control_handler(const char*, const char*, lo_arg** argv, 
                             int argc, lo_message, void* cbdata);
  static int program_handler(const char*, const char*, lo_arg** argv, 
                             int argc, lo_message, void* cbdata);
  static int exiting_handler(const char*, const char*, lo_arg** argv, 
                             int argc, lo_message, void* cbdata);
  
  static void* sender_thread(void* arg);
  
  
  LV2Host& m_host;
  
  lo_server_thread m_server;

  std::string m_url;
  
  struct CallbackData {
    CallbackData(LV2Host& h, OSCController& o) : host(h), me(o) { }
    LV2Host& host;
    OSCController& me;
  } m_cbdata;
  
  struct ClientInfo {
    ClientInfo(lo_address a, const std::string& p) : address(a), path(p) { }
    lo_address address;
    std::string path;
  };
  
  bool& m_still_running;
  
  EventQueue m_queue;
  pthread_t m_sender;

  std::vector<ClientInfo*> m_clients;
  pthread_mutex_t m_clients_mutex;
};


#endif
