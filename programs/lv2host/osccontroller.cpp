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
  lo_server_thread_add_method(m_server, "/lv2/configure", "ss", 
                              &OSCController::configure_handler, &m_cbdata);
  lo_server_thread_add_method(m_server, "/lv2/program", "i", 
                              &OSCController::program_handler, &m_cbdata);
  lo_server_thread_add_method(m_server, "/lv2/midi", "iis", 
                              &OSCController::midi_handler, &m_cbdata);
  
  cout<<"OSC server listening on "<<m_url<<endl;
  
  if (pthread_create(&m_sender, 0, &OSCController::sender_thread, this))
    exit(-1);
  
  m_host.set_event_queue(&m_queue);
  
}


OSCController::~OSCController() {
  stop();
  lo_server_thread_free(m_server);
  pthread_cancel(m_sender);
  pthread_join(m_sender, 0);
  pthread_mutex_destroy(&m_clients_mutex);
  for (unsigned i = 0; i < m_clients.size(); ++i) {
    lo_address_free(m_clients[i]->address);
    delete m_clients[i];
  }
}


void OSCController::start() {
  lo_server_thread_start(m_server);
}


void OSCController::stop() {
  lo_server_thread_stop(m_server);
}


void OSCController::send_configure(const std::string& key, const std::string& value) {
  pthread_mutex_lock(&m_clients_mutex);
  for (size_t i = 0; i < m_clients.size(); ++i) {
    lo_send(m_clients[i]->address, (m_clients[i]->path + "configure").c_str(), "ss",
            key.c_str(), value.c_str());
  }
  pthread_mutex_unlock(&m_clients_mutex);
}


const string& OSCController::get_url() const {
  return m_url;
}

 
int OSCController::update_handler(const char*, const char*, lo_arg** argv, 
                                  int argc, lo_message, void* cbdata) {
  
  CallbackData* data = static_cast<CallbackData*>(cbdata);
  //cerr<<"Got /update from "<<(&argv[0]->s)<<endl;
  lo_address client = lo_address_new_from_url(&(argv[0]->s));
  char* path_c = lo_url_get_path(&(argv[0]->s));
  string path = path_c;
  free(path_c);
  if (path[path.size() - 1] != '/')
    path += '/';
  
  pthread_mutex_lock(&data->me.m_clients_mutex);
  ClientInfo* ci = new ClientInfo(client, path);
  data->me.m_clients.push_back(ci);
  pthread_mutex_unlock(&data->me.m_clients_mutex);
  
  const map<string, string>& config = data->host.get_config();
  map<string, string>::const_iterator iter;
  
  pthread_mutex_lock(&data->me.m_clients_mutex);
  for (iter = config.begin(); iter != config.end(); ++iter)
    data->me.send_configure(iter->first, iter->second, ci);
  pthread_mutex_unlock(&data->me.m_clients_mutex);

  data->host.queue_config_request(&data->me.m_queue);
  data->host.queue_passthrough("show", ci);
  
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


int OSCController::configure_handler(const char*, const char*, lo_arg** argv, 
                                     int argc, lo_message, void* cbdata) {
  static_cast<CallbackData*>(cbdata)->host.configure(&argv[0]->s, &argv[1]->s);
}


int OSCController::program_handler(const char*, const char*, lo_arg** argv, 
                                   int argc, lo_message, void* cbdata) {
  cerr<<__PRETTY_FUNCTION__<<endl;
  unsigned long program = argv[0]->i;
  static_cast<CallbackData*>(cbdata)->host.queue_program(program);
}


int OSCController::midi_handler(const char*, const char*, lo_arg** argv, 
                                   int argc, lo_message, void* cbdata) {
  static_cast<CallbackData*>(cbdata)->
    host.queue_midi(argv[0]->i, argv[1]->i, 
                    (unsigned char*)(&argv[2]->s));
}


void OSCController::send_configure(const std::string& key, const std::string& value, 
                                   ClientInfo* ci) {
  lo_send(ci->address, (ci->path + "configure").c_str(), "ss",
          key.c_str(), value.c_str());
}


void OSCController::send_quit() {
  pthread_mutex_lock(&m_clients_mutex);
  for (size_t i = 0; i < m_clients.size(); ++i)
    lo_send(m_clients[i]->address, (m_clients[i]->path + "quit").c_str(), "");
  pthread_mutex_unlock(&m_clients_mutex);
}


void* OSCController::sender_thread(void* arg) {

  OSCController* me = static_cast<OSCController*>(arg);
  EventQueue::Type t;
  const EventQueue::Event& event = me->m_queue.get_event();
  
  do {
    while ((t = me->m_queue.read_event()) != EventQueue::None) {
      
      switch (t) {
      
      case EventQueue::Control:
        cerr<<__PRETTY_FUNCTION__<<" "<<event.control.port<<" "<<event.control.value<<endl;
        pthread_mutex_lock(&me->m_clients_mutex);
        for (size_t i = 0; i < me->m_clients.size(); ++i) {
          lo_send(me->m_clients[i]->address, 
                  (me->m_clients[i]->path + "control").c_str(), "if",
                  event.control.port, event.control.value);
        }
        pthread_mutex_unlock(&me->m_clients_mutex);
        break;
        
      case EventQueue::Program:
        pthread_mutex_lock(&me->m_clients_mutex);
        for (size_t i = 0; i < me->m_clients.size(); ++i) {
          lo_send(me->m_clients[i]->address, 
                  (me->m_clients[i]->path + "program").c_str(), "i",
                  event.program.program);
        }
        pthread_mutex_unlock(&me->m_clients_mutex);
        break;
      
      case EventQueue::Passthrough:
        if (!std::strcmp(event.passthrough.msg, "show")) {
          ClientInfo* ci = static_cast<ClientInfo*>(event.passthrough.ptr);
          map<uint32_t, LV2Preset>::const_iterator iter;
          for (iter = me->m_host.get_presets().begin(); 
               iter != me->m_host.get_presets().end(); ++iter) {
            lo_send(ci->address, (ci->path + "add_program").c_str(), "is",
                    iter->first, iter->second.name.c_str());
          }
          lo_send(ci->address, (ci->path + "show").c_str(), "");
        }
        break;
      
      }
    }
  
    me->m_queue.wait();
  } while (me->m_still_running);
  return 0;
}
