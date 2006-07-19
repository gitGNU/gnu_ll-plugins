#include <map>
#include <iostream>

#include "lv2host.hpp"
#include "osccontroller.hpp"


using namespace std;


OSCController::OSCController(LV2Host& host, bool& still_running)
  : m_host(host),
    m_cbdata(host, *this),
    m_still_running(still_running) {
  
  m_server = lo_server_thread_new(0, 0);
  
  char* url_c = lo_server_thread_get_url(m_server);
  m_url = url_c;
  free(url_c);
  m_url += "lv2/";
  
  lo_server_thread_add_method(m_server, "/lv2/update", "s", 
                              &OSCController::update_handler, &m_cbdata);
  lo_server_thread_add_method(m_server, "/lv2/exiting", "", 
                              &OSCController::exiting_handler, &m_cbdata);
  lo_server_thread_add_method(m_server, "/lv2/control", "if", 
                              &OSCController::control_handler, &m_cbdata);
  
  cout<<"OSC server listening on "<<m_url<<endl;
}


void OSCController::start() {
  lo_server_thread_start(m_server);
}


void OSCController::stop() {
  lo_server_thread_stop(m_server);
}


const string& OSCController::get_url() const {
  return m_url;
}

 
int OSCController::update_handler(const char*, const char*, lo_arg** argv, 
                                  int argc, lo_message, void* cbdata) {
  
  cout<<__PRETTY_FUNCTION__<<endl;
  
  CallbackData* data = static_cast<CallbackData*>(cbdata);
  lo_address client = lo_address_new_from_url(&(argv[0]->s));
  char* path_c = lo_url_get_path(&(argv[0]->s));
  string path = path_c;
  free(path_c);
  if (path[path.size() - 1] != '/')
    path += '/';
  
  // send config
  const map<string, string>& config = data->host.get_configuration();
  map<string, string>::const_iterator iter;
  for (iter = config.begin(); iter != config.end(); ++iter) {
    lo_send(client, (path + "configure").c_str(), "ss", 
            iter->first.c_str(), iter->second.c_str());
  }
  
  // send program
  if (data->host.program_is_valid())
    lo_send(client, (path + "program").c_str(), "i", data->host.get_program());
  
  // send controls
  for (unsigned long p = 0; p < data->host.get_ports().size(); ++p) {
    const LV2Port& port = data->host.get_ports()[p];
    if (port.rate == ControlRate && !port.midi && port.direction == InputPort)
      lo_send(client, (path + "control").c_str(), "if", p, port.locked_value);
  }
  
  lo_send(client, (path + "show").c_str(), "");
}


int OSCController::exiting_handler(const char*, const char*, lo_arg** argv, 
                                   int argc, lo_message, void* cbdata) {
  cerr<<__PRETTY_FUNCTION__<<endl;
  static_cast<CallbackData*>(cbdata)->me.m_still_running = false;
}


int OSCController::control_handler(const char*, const char*, lo_arg** argv, 
                                   int argc, lo_message, void* cbdata) {
  unsigned long port = argv[0]->i;
  float value = argv[1]->f;
  static_cast<CallbackData*>(cbdata)->host.queue_control(port, value);
}
