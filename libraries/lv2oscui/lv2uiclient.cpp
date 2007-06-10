/****************************************************************************
    
    lv2uiclient.cpp - a class that makes writing LV2 GUIs easier
    
    Copyright (C) 2005  Lars Luthman <larsl@users.sourceforge.net>
    
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

#include <cassert>
#include <iostream>
#include <sstream>

#include <glibmm.h>

#include "lv2uiclient.hpp"


using namespace Gtk;
using namespace sigc;
using namespace std;
using namespace Glib;



LV2UIClient::LV2UIClient(const string& osc_url, const string& bundle,
                         const string& identifier, bool wait)
  : m_valid(false), 
    m_identifier(identifier),
    m_bundle(bundle), 
    m_blocking(false) {
  
  /*
  cerr<<"Plugin OSC URL: "<<argv[1]<<endl
      <<"Plugin bundle: "<<argv[2]<<endl
      <<"Plugin URI: "<<argv[3]<<endl
      <<"Plugin identifier: "<<argv[4]<<endl;
  */
  
  if(!thread_supported())
    thread_init();
  
  m_control_dispatcher.
    connect(mem_fun(*this, &LV2UIClient::control_receiver));
  m_program_dispatcher.
    connect(mem_fun(*this, &LV2UIClient::program_receiver));
  m_configure_dispatcher.
    connect(mem_fun(*this, &LV2UIClient::configure_receiver));
  m_filename_dispatcher.
    connect(mem_fun(*this, &LV2UIClient::filename_receiver));
  m_add_program_dispatcher.
    connect(mem_fun(*this, &LV2UIClient::add_program_receiver));
  m_remove_program_dispatcher.
    connect(mem_fun(*this, &LV2UIClient::remove_program_receiver));
  
  control_received.connect(mem_fun(*this, &LV2UIClient::update_adjustments));

  m_plugin_address = lo_address_new_from_url(osc_url.c_str());
  m_plugin_path = lo_url_get_path(osc_url.c_str());
  if (m_plugin_path[m_plugin_path.size() - 1] == '/')
    m_plugin_path = m_plugin_path.substr(0, m_plugin_path.size() - 1);
  m_server_thread = lo_server_thread_new(NULL, NULL);

  lo_server_thread_add_method(m_server_thread, "/lv2plugin/control", "if",
                              &LV2UIClient::control_handler, this);
  lo_server_thread_add_method(m_server_thread, "/lv2plugin/program", "i", 
                              &LV2UIClient::program_handler, this);
  lo_server_thread_add_method(m_server_thread, "/lv2plugin/add_program", "is", 
                              &LV2UIClient::add_program_handler, this);
  lo_server_thread_add_method(m_server_thread, "/lv2plugin/remove_program", "i",
                              &LV2UIClient::remove_program_handler, this);
  lo_server_thread_add_method(m_server_thread, "/lv2plugin/clear_programs", "",
                              &LV2UIClient::clear_programs_handler, this);
  lo_server_thread_add_method(m_server_thread, "/lv2plugin/configure", "ss", 
                              &LV2UIClient::configure_handler, this);
  lo_server_thread_add_method(m_server_thread, "/lv2plugin/set_file", "ss", 
                              &LV2UIClient::filename_handler, this);
  lo_server_thread_add_method(m_server_thread, "/lv2plugin/show", "", 
                              &LV2UIClient::show_handler, this);
  lo_server_thread_add_method(m_server_thread, "/lv2plugin/hide", "", 
                              &LV2UIClient::hide_handler, this);
  lo_server_thread_add_method(m_server_thread, "/lv2plugin/quit", "", 
                              &LV2UIClient::quit_handler, this);
  
  lo_server_thread_start(m_server_thread);
  
  m_valid = true;
  
  if (!wait) {
    send_update_request();
  }
}


LV2UIClient::~LV2UIClient() {
  if (m_valid) {
    send_exiting();
    lo_server_thread_stop(m_server_thread);
    lo_server_thread_free(m_server_thread);
  }
}


const string& LV2UIClient::get_identifier() const {
  return m_identifier;
}


const string& LV2UIClient::get_bundle_path() const {
  return m_bundle;
}


void LV2UIClient::connect_adjustment(Adjustment* adj, int port) {
  adj->signal_value_changed().
    connect(compose(bind<0>(mem_fun(*this, &LV2UIClient::send_control), port),
		    mem_fun(*adj, &Adjustment::get_value)));
  if (m_adjustments.size() <= unsigned(port))
    m_adjustments.resize(port + 1, NULL);
  m_adjustments[port] = adj;
}


Adjustment* LV2UIClient::get_adjustment(int port) {
  if (port >= 0 && port < (int)m_adjustments.size())
    return m_adjustments[port];
  return 0;
}


float LV2UIClient::get_adjustment_value(int port) {
  if (port >= 0 && port < (int)m_adjustments.size() && m_adjustments[port]) {
    return m_adjustments[port]->get_value();
  }
  return 0;
}


bool LV2UIClient::get_active_program(unsigned long& program) {
  program = m_active_program;
  return m_program_is_set;
}


void LV2UIClient::send_control(int port, float value) {
  if (m_valid && !m_blocking) {
    lo_send(m_plugin_address, string(m_plugin_path + "/control").c_str(),
            "if", port, value);
  }
}


void LV2UIClient::send_program(int program) {
  if (m_valid && !m_blocking) {
    lo_send(m_plugin_address, string(m_plugin_path + "/program").c_str(),
            "i", program);
    /* The host does not send a /program back when we told it to change
       program, so we fire off the signal directly instead */
    m_blocking = true;
    m_program_is_set = true;
    m_active_program = program;
    program_received(program);
    m_blocking = false;
  }
}


void LV2UIClient::send_update_request() {
  if (m_valid) {
    char* my_url = lo_server_thread_get_url(m_server_thread);
    string url(my_url);
    free(my_url);
    lo_send(m_plugin_address, (m_plugin_path + "/update").c_str(), 
            "s", (url + "lv2plugin").c_str());
  }
}


void LV2UIClient::send_configure(const string& key, const string& value) {
  if (m_valid) {
    lo_send(m_plugin_address, (m_plugin_path + "/configure").c_str(),
            "ss", key.c_str(), value.c_str());
  }
}


void LV2UIClient::send_filename(const string& key, const string& filename) {
  if (m_valid) {
    lo_send(m_plugin_address, (m_plugin_path + "/set_file").c_str(),
            "ss", key.c_str(), filename.c_str());
  }
}


void LV2UIClient::send_tell_plugin(uint32_t argc, const char* const* argv) {
  cerr<<__PRETTY_FUNCTION__<<endl;
  cerr<<"argc = "<<argc<<endl;
  if (m_valid) {
    string types = "";
    for (unsigned i = 0; i < argc; ++i)
      types += "s";
    if (argc == 0)
      lo_send(m_plugin_address, (m_plugin_path + "/tell_plugin").c_str(),
	      types.c_str());
    else if (argc == 1)
      lo_send(m_plugin_address, (m_plugin_path + "/tell_plugin").c_str(),
	      types.c_str(), argv[0]);
    else if (argc == 2)
      lo_send(m_plugin_address, (m_plugin_path + "/tell_plugin").c_str(),
	      types.c_str(), argv[0], argv[1]);
    else if (argc == 3)
      lo_send(m_plugin_address, (m_plugin_path + "/tell_plugin").c_str(),
	      types.c_str(), argv[0], argv[1], argv[2]);
    else if (argc == 4)
      lo_send(m_plugin_address, (m_plugin_path + "/tell_plugin").c_str(),
	      types.c_str(), argv[0], argv[1], argv[2], argv[3]);
    else
      cerr<<"Can not send more than 4 arguments"<<endl;
    // XXX fix this
  }
}


void LV2UIClient::send_midi(int port, int size, const unsigned char* event) {
  assert(size <= 4);
  if (m_valid) {
    uint8_t ievent[4];
    memcpy(ievent, event, size);
    lo_send(m_plugin_address, (m_plugin_path + "/midi").c_str(), "iim", 
            port, size, ievent);
  }
}


void LV2UIClient::send_exiting() {
  if (m_valid) {
    lo_send(m_plugin_address, (m_plugin_path + "/exiting").c_str(), NULL);
  }
}


void LV2UIClient::update_adjustments(int port, float value) {
  
}


int LV2UIClient::control_handler(const char *path, const char *types,
                                 lo_arg **argv, int argc, 
                                 void *data, void *user_data) {
  LV2UIClient* me = static_cast<LV2UIClient*>(user_data);
  me->m_control_queue.push(make_pair(argv[0]->i, argv[1]->f));
  me->m_control_dispatcher();
  return 0;
}


int LV2UIClient::program_handler(const char *path, const char *types,
                                 lo_arg **argv, int argc, 
                                 void *data, void *user_data) {
  LV2UIClient* me = static_cast<LV2UIClient*>(user_data);
  me->m_program_queue.push(argv[0]->i);
  me->m_program_dispatcher();
  return 0;
}


int LV2UIClient::add_program_handler(const char *path, const char *types,
                                     lo_arg **argv, int argc, 
                                     void *data, void *user_data) {
  LV2UIClient* me = static_cast<LV2UIClient*>(user_data);
  me->m_add_program_queue.push(make_pair(argv[0]->i, string(&argv[1]->s)));
  me->m_add_program_dispatcher();
  return 0;
}


int LV2UIClient::remove_program_handler(const char *path, const char *types,
                                        lo_arg **argv, int argc, 
                                        void *data, void *user_data) {
  LV2UIClient* me = static_cast<LV2UIClient*>(user_data);
  me->m_remove_program_queue.push(argv[0]->i);
  me->m_remove_program_dispatcher();
  return 0;
}


int LV2UIClient::clear_programs_handler(const char *path, const char *types,
                                        lo_arg **argv, int argc, 
                                        void *data, void *user_data) {
  LV2UIClient* me = static_cast<LV2UIClient*>(user_data);
  me->clear_programs_received();
  return 0;
}


int LV2UIClient::configure_handler(const char *path, const char *types,
                                   lo_arg **argv, int argc, 
                                   void *data, void *user_data) {
  LV2UIClient* me = static_cast<LV2UIClient*>(user_data);
  string key(&argv[0]->s);
  string value(&argv[1]->s);
  me->m_configure_queue.push(make_pair(key, value));
  me->m_configure_dispatcher();
  return 0;
}


int LV2UIClient::filename_handler(const char *path, const char *types,
                                  lo_arg **argv, int argc, 
                                  void *data, void *user_data) {
  LV2UIClient* me = static_cast<LV2UIClient*>(user_data);
  string key(&argv[0]->s);
  string value(&argv[1]->s);
  me->m_filename_queue.push(make_pair(key, value));
  me->m_filename_dispatcher();
  return 0;
}


int LV2UIClient::show_handler(const char *path, const char *types,
                              lo_arg **argv, int argc, 
                              void *data, void *user_data) {
  LV2UIClient* me = static_cast<LV2UIClient*>(user_data);
  me->show_received();
  return 0;
}


int LV2UIClient::hide_handler(const char *path, const char *types,
                              lo_arg **argv, int argc, 
                              void *data, void *user_data) {
  LV2UIClient* me = static_cast<LV2UIClient*>(user_data);
  me->hide_received();
  return 0;
}


int LV2UIClient::quit_handler(const char *path, const char *types,
                              lo_arg **argv, int argc, 
                              void *data, void *user_data) {
  LV2UIClient* me = static_cast<LV2UIClient*>(user_data);
  me->m_valid = false;
  me->quit_received();
  lo_server_thread_stop(me->m_server_thread);
  return 0;
}


bool LV2UIClient::is_valid() const {
  return m_valid;
}
