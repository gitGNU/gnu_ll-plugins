#include <cstdlib>
#include <map>
#include <iostream>

#include "lv2host.hpp"
#include "osccontroller.hpp"


using namespace std;


OSCController::OSCController(LV2Host& host, bool& still_running)
  : m_host(host),
    m_cbdata(host, *this),
    m_still_running(still_running) {
  
  pthread_mutex_init(&m_clients_mutex, 0);
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
  lo_server_thread_add_method(m_server, "/lv2/program", "i", 
                              &OSCController::program_handler, &m_cbdata);
  
  cout<<"OSC server listening on "<<m_url<<endl;
  
  if (pthread_create(&m_sender, 0, &OSCController::sender_thread, this))
    exit(-1);
  
  m_host.set_event_queue(&m_queue);
  
}


OSCController::~OSCController() {
  stop();
  lo_server_thread_free(m_server);
  pthread_join(m_sender, 0);
  for (unsigned i = 0; i < m_clients.size(); ++i)
    lo_address_free(m_clients[i].address);
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
  
  CallbackData* data = static_cast<CallbackData*>(cbdata);
  cerr<<"Got /update from "<<(&argv[0]->s)<<endl;
  lo_address client = lo_address_new_from_url(&(argv[0]->s));
  char* path_c = lo_url_get_path(&(argv[0]->s));
  string path = path_c;
  free(path_c);
  if (path[path.size() - 1] != '/')
    path += '/';
  
  pthread_mutex_lock(&data->me.m_clients_mutex);
  data->me.m_clients.push_back(ClientInfo(client, path));
  pthread_mutex_unlock(&data->me.m_clients_mutex);

  data->host.queue_config_request();
  lo_send(client, (path + "show").c_str(), "");
}


int OSCController::exiting_handler(const char*, const char*, lo_arg** argv, 
                                   int argc, lo_message, void* cbdata) {
  static_cast<CallbackData*>(cbdata)->me.m_still_running = false;
}


int OSCController::control_handler(const char*, const char*, lo_arg** argv, 
                                   int argc, lo_message, void* cbdata) {
  unsigned long port = argv[0]->i;
  float value = argv[1]->f;
  static_cast<CallbackData*>(cbdata)->host.queue_control(port, value);
}


int OSCController::program_handler(const char*, const char*, lo_arg** argv, 
                                   int argc, lo_message, void* cbdata) {
  unsigned long program = argv[0]->i;
  static_cast<CallbackData*>(cbdata)->host.queue_program(program);
}


void* OSCController::sender_thread(void* arg) {

  OSCController* me = static_cast<OSCController*>(arg);
  EventQueue::Type t;
  const EventQueue::Event& event = me->m_queue.get_event();
  
  do {
    while ((t = me->m_queue.read_event()) != EventQueue::None) {
      
      switch (t) {
      
      case EventQueue::Control:
        pthread_mutex_lock(&me->m_clients_mutex);
        for (size_t i = 0; i < me->m_clients.size(); ++i) {
          lo_send(me->m_clients[i].address, 
                  (me->m_clients[i].path + "control").c_str(), "if",
                  event.control.port, event.control.value);
        }
        pthread_mutex_unlock(&me->m_clients_mutex);
        break;
        
      case EventQueue::Program:
        pthread_mutex_lock(&me->m_clients_mutex);
        for (size_t i = 0; i < me->m_clients.size(); ++i) {
          lo_send(me->m_clients[i].address, 
                  (me->m_clients[i].path + "program").c_str(), "i",
                  event.program.program);
        }
        pthread_mutex_unlock(&me->m_clients_mutex);
        break;
      }
      
    }
  
    usleep(10000);
  } while (me->m_still_running);
  return 0;
}
