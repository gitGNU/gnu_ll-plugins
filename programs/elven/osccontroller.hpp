/****************************************************************************
    
    osccontroller.hpp - Simple OSC server interface for Elven
    
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
  
  void send_configure(const std::string& key, const std::string& value);
  void send_filename(const std::string& key, const std::string& filename);
  void send_quit();
  
  void start();
  void stop();
  
  const std::string& get_url() const;
  
protected:
  
  static int update_handler(const char*, const char*, lo_arg** argv, int argc, 
                            lo_message, void* cbdata);
  static int control_handler(const char*, const char*, lo_arg** argv, 
                             int argc, lo_message, void* cbdata);
  static int configure_handler(const char*, const char*, lo_arg** argv, 
                             int argc, lo_message, void* cbdata);
  static int set_file_handler(const char*, const char*, lo_arg** argv, 
                              int argc, lo_message, void* cbdata);
  static int program_handler(const char*, const char*, lo_arg** argv, 
                             int argc, lo_message, void* cbdata);
  static int midi_handler(const char*, const char*, lo_arg** argv,
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

  void send_configure(const std::string& key, const std::string& value, ClientInfo* ci);
  
  bool& m_still_running;
  
  EventQueue m_queue;
  pthread_t m_sender;

  std::vector<ClientInfo*> m_clients;
  pthread_mutex_t m_clients_mutex;
};


#endif
