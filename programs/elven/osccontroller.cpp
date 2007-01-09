/****************************************************************************
    
    osccontroller.cpp - Simple OSC server interface for Elven
    
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

#include <cstdlib>
#include <map>
#include <iostream>

#include "lv2host.hpp"
#include "osccontroller.hpp"
#include "debug.hpp"
#include "midiutils.hpp"

using namespace std;


OSCController::OSCController(LV2Host& host, bool& still_running)
  : m_host(host),
    m_cbdata(host, *this),
    m_still_running(still_running) {
  
  pthread_mutex_init(&m_clients_mutex, 0);
  m_server = lo_server_new(0, 0);
  
  char* url_c = lo_server_get_url(m_server);
  m_url = url_c;
  free(url_c);
  m_url += "lv2/";
  
  lo_server_add_method(m_server, "/lv2/update", "s", 
                       &OSCController::update_handler, &m_cbdata);
  lo_server_add_method(m_server, "/lv2/exiting", "", 
                       &OSCController::exiting_handler, &m_cbdata);
  lo_server_add_method(m_server, "/lv2/control", "if", 
                       &OSCController::control_handler, &m_cbdata);
  lo_server_add_method(m_server, "/lv2/configure", "ss", 
                       &OSCController::configure_handler, &m_cbdata);
  lo_server_add_method(m_server, "/lv2/set_file", "ss", 
                       &OSCController::set_file_handler, &m_cbdata);
  lo_server_add_method(m_server, "/lv2/program", "i", 
                       &OSCController::program_handler, &m_cbdata);
  lo_server_add_method(m_server, "/lv2/midi", "iim", 
                       &OSCController::midi_handler, &m_cbdata);
  
  DBG2("OSC server created, listening on "<<m_url);
  
  if (pthread_create(&m_sender, 0, &OSCController::sender_thread, this))
    exit(-1);
  
  m_host.set_event_queue(&m_queue);
  
}


OSCController::~OSCController() {
  lo_server_free(m_server);
  pthread_cancel(m_sender);
  pthread_join(m_sender, 0);
  pthread_mutex_destroy(&m_clients_mutex);
  for (unsigned i = 0; i < m_clients.size(); ++i) {
    lo_address_free(m_clients[i]->address);
    delete m_clients[i];
  }
  DBG2("OSC server destroyed");
}


/*
void OSCController::start() {
  DBG2("OSC server listening on "<<m_url);
  lo_server_thread_start(m_server);
}


void OSCController::stop() {
  DBG2("OSC server shutting down");
  lo_server_thread_stop(m_server);
}
*/


bool OSCController::run() {
  return lo_server_recv_noblock(m_server, 1) != 0;
}


void OSCController::send_configure(const std::string& key, 
                                   const std::string& value) {
  DBG2("Sending /configure "<<key<<" "<<value);
  pthread_mutex_lock(&m_clients_mutex);
  for (size_t i = 0; i < m_clients.size(); ++i) {
    lo_send(m_clients[i]->address, (m_clients[i]->path + "configure").c_str(),
            "ss", key.c_str(), value.c_str());
  }
  pthread_mutex_unlock(&m_clients_mutex);
}


void OSCController::send_filename(const std::string& key, 
                                  const std::string& filename) {
  DBG2("Sending /set_file "<<key<<" "<<filename);
  pthread_mutex_lock(&m_clients_mutex);
  for (size_t i = 0; i < m_clients.size(); ++i) {
    lo_send(m_clients[i]->address, (m_clients[i]->path + "set_file").c_str(),
            "ss", key.c_str(), filename.c_str());
  }
  pthread_mutex_unlock(&m_clients_mutex);
}


const string& OSCController::get_url() const {
  return m_url;
}

 
int OSCController::update_handler(const char*, const char*, lo_arg** argv, 
                                  int argc, lo_message, void* cbdata) {
  
  DBG2("Received /update from "<<(&argv[0]->s));

  CallbackData* data = static_cast<CallbackData*>(cbdata);
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
    data->me.send_one_configure(iter->first, iter->second, ci);
  pthread_mutex_unlock(&data->me.m_clients_mutex);

  const map<string, string>& filenames = data->host.get_filenames();
  pthread_mutex_lock(&data->me.m_clients_mutex);
  for (iter = filenames.begin(); iter != filenames.end(); ++iter)
    data->me.send_one_filename(iter->first, iter->second, ci);
  pthread_mutex_unlock(&data->me.m_clients_mutex);

  data->host.queue_config_request(&data->me.m_queue);
  data->host.queue_passthrough("show", ci);
  
}


int OSCController::exiting_handler(const char*, const char*, lo_arg** argv, 
                                   int argc, lo_message, void* cbdata) {
  DBG2("Received /exiting");
  static_cast<CallbackData*>(cbdata)->me.m_still_running = false;
}


int OSCController::control_handler(const char*, const char*, lo_arg** argv, 
                                   int argc, lo_message, void* cbdata) {
  DBG2("Received /control "<<argv[0]->i<<" "<<argv[1]->f);
  unsigned long port = argv[0]->i;
  float value = argv[1]->f;
  static_cast<CallbackData*>(cbdata)->host.queue_control(port, value);
}


int OSCController::configure_handler(const char*, const char*, lo_arg** argv, 
                                     int argc, lo_message, void* cbdata) {
  DBG2("Received /configure "<<&argv[0]->s<<" "<<&argv[1]->s);
  static_cast<CallbackData*>(cbdata)->host.configure(&argv[0]->s, &argv[1]->s);
}


int OSCController::set_file_handler(const char*, const char*, lo_arg** argv, 
                                    int argc, lo_message, void* cbdata) {
  DBG2("Received /set_file "<<argv[0]->s<<" "<<argv[1]->s);
  static_cast<CallbackData*>(cbdata)->host.set_file(&argv[0]->s, &argv[1]->s);
}


int OSCController::program_handler(const char*, const char*, lo_arg** argv, 
                                   int argc, lo_message, void* cbdata) {
  DBG2("Received /program "<<argv[0]->i);
  unsigned long program = argv[0]->i;
  static_cast<CallbackData*>(cbdata)->host.queue_program(program);
}


int OSCController::midi_handler(const char*, const char*, lo_arg** argv, 
                                int argc, lo_message, void* cbdata) {
  DBG2("Received /midi "<<argv[0]->i<<" "<<argv[1]->i<<" <"
       <<midi2str(argv[1]->i, (unsigned char*)&argv[2]->s)<<">");
  static_cast<CallbackData*>(cbdata)->
    host.queue_midi(argv[0]->i, argv[1]->i, 
                    (unsigned char*)(argv[2]->m));
}


void OSCController::send_one_configure(const std::string& key, const std::string& value, 
                                       ClientInfo* ci) {
  lo_send(ci->address, (ci->path + "configure").c_str(), "ss",
          key.c_str(), value.c_str());
}


void OSCController::send_one_filename(const std::string& key, const std::string& value, 
                                      ClientInfo* ci) {
  lo_send(ci->address, (ci->path + "set_file").c_str(), "ss",
          key.c_str(), value.c_str());
}


void OSCController::send_quit() {
  DBG2("Sending /quit");
  pthread_mutex_lock(&m_clients_mutex);
  for (size_t i = 0; i < m_clients.size(); ++i)
    lo_send(m_clients[i]->address, (m_clients[i]->path + "quit").c_str(), "");
  pthread_mutex_unlock(&m_clients_mutex);
}


void* OSCController::sender_thread(void* arg) {
  
  DebugInfo::thread_prefix()[pthread_self()] = "S ";
  
  OSCController* me = static_cast<OSCController*>(arg);
  EventQueue::Type t;
  const EventQueue::Event& event = me->m_queue.get_event();
  
  do {
    while ((t = me->m_queue.read_event()) != EventQueue::None) {
      
      switch (t) {
      
      case EventQueue::Control:
        //cerr<<__PRETTY_FUNCTION__<<" "<<event.control.port<<" "<<event.control.value<<endl;
        DBG2("Sending /control "<<event.control.port<<" "<<event.control.value);
        pthread_mutex_lock(&me->m_clients_mutex);
        for (size_t i = 0; i < me->m_clients.size(); ++i) {
          lo_send(me->m_clients[i]->address, 
                  (me->m_clients[i]->path + "control").c_str(), "if",
                  event.control.port, event.control.value);
        }
        pthread_mutex_unlock(&me->m_clients_mutex);
        break;
        
      case EventQueue::Program:
        DBG2("Sending /program "<<event.program.program);
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
            DBG2("Sending /add_program "<<iter->first<<" "<<iter->second.name);
            lo_send(ci->address, (ci->path + "add_program").c_str(), "is",
                    iter->first, iter->second.name.c_str());
          }
          DBG2("Sending /show");
          lo_send(ci->address, (ci->path + "show").c_str(), "");
        }
        break;
      
      }
    }
  
    me->m_queue.wait();
  } while (me->m_still_running);
  return 0;
}
