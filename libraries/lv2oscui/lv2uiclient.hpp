/****************************************************************************
    
    lv2uiclient.hpp - a class that makes writing LV2 GUIs easier
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
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

#ifndef LV2UICLIENT_HPP
#define LV2UICLIENT_HPP

#include <queue>
#include <string>
#include <utility>
#include <vector>

#include <glibmm.h>
#include <gtkmm.h>
#include <lo/lo.h>
#include <sigc++/sigc++.h>


/** This class can be used by a LV2 plugin UI to communicate with the plugin
    host. It has public functions for all the messages that the UI can send
    to the host, and public signals for all the messages that the host can send
    to the UI, which will be emitted when a message is received. It also handles
    all the required communication with the host, such as sending the first
    /update request and sending an /exiting message when the LV2UIClient
    object is destroyed. 
*/
class LV2UIClient {
public:
  
  /** This constructor starts an OSC receiver thread and sends an update
      request to the plugin host. The parameters should be the @c argc and
      @c argv from the main() function. */
  LV2UIClient(const std::string& osc_url, const std::string& bundle,
              const std::string& identifier, bool wait = false);

  /** This destructor will mark any allocated shared memory for destruction,
      send an /exiting message to the plugin host, and stop and deallocate
      the OSC receiver thread. */
  ~LV2UIClient();
  
  /** Returns @c true if the initialisation went OK and we haven't received
      a /quit message yet. */
  bool is_valid() const;
  
  /** Returns the identifier string given by the plugin host. */
  const std::string& get_identifier() const;
  
  /** Returns the bundle path. */
  const std::string& get_bundle_path() const;
  
  /** Connects a Gtk::Adjustment to a LV2 port. */
  void connect_adjustment(Gtk::Adjustment* adj, int port);
  
  Gtk::Adjustment* get_adjustment(int port);
  
  /** Returns the value of a connected adjustment. */
  float get_adjustment_value(int port);
  
  /** Get active program. */
  bool get_active_program(unsigned long& program);
  
  
  // C++ wrappers for OSC methods specified in the LV2 RFC:
  
  // UI to host
  
  /** Sends the control value @c value to input port number @c port in the 
      plugin. */
  void send_control(int port, float value);
  
  /** Change the plugin's program. */
  void send_program(int program);
  
  /** Tell the host that we want an update of all the controls, program and
      configuration values for the plugin. This is called automatically
      when this LV2UIClient object is created. */
  void send_update_request();
  
  /** Send a configuration value to the plugin. */
  void send_configure(const std::string& key, const std::string& value);
  
  /** Send a configuration value to the plugin. */
  void send_filename(const std::string& key, const std::string& filename);
  
  /** Send a command to the plugin. */
  void send_tell_plugin(uint32_t argc, const char* const* argv);
  
  /** Send a MIDI event to the plugin. The effect will be exactly the same
      as if it had been sent by the plugin host. */
  void send_midi(int port, int size, const unsigned char* event);
  
  /** Tell the plugin host that the GUI is about to quit (you shouldn't have
      to call this explicitly, it is called when this LV2UIClient object
      is destroyed. */
  void send_exiting();
  
  /** Tell the plugin host to save the plugin's current setting as a program
      with the given name and number. */
  void send_save_program(int number, const std::string& name);
  
  // Host to UI
  
  /** This signal is emitted when the host sends a new control value.
      The parameters are the control port number and the new control value. */
  sigc::signal<void, int, float> control_received;
  
  /** Emitted when the host sends a program change. The parameter is the 
      program number. */
  sigc::signal<void, int> program_received;
  
  /** Emitted when the host sends a configuration value. The parameters are
      the configuration key and the configuration value. */
  sigc::signal<void, std::string, std::string> configure_received;
  
  /** Emitted when the host sends a filename. The parameters are
      the filename key and the filename value. */
  sigc::signal<void, std::string, std::string> filename_received;
  
  /** Emitted when the host sends a tell_gui. The parameters are the argument
      count and the argument array. */
  sigc::signal<void, uint32_t, const char* const*> tell_gui_received;
  
  /** Emitted when the host wants the UI to be visible. A LV2 GUI should not
      show any windows until this signal is emitted. */
  Glib::Dispatcher show_received;
  
  /** Emitted when the host wants to hide the UI. */
  Glib::Dispatcher hide_received;
  
  /** Emitted when the host wants the UI to exit. This LV2UIClient object
      will not send or receive any OSC messages after it has received this
      message, but you still have to quit the program yourself. */
  Glib::Dispatcher quit_received;
  
  /** Emitted when the host tells the GUI that a program has been added. */
  sigc::signal<void, int, std::string> add_program_received;
  
  /** Emitted when the host tells the GUI that a program has been removed. */
  sigc::signal<void, int> remove_program_received;
  
  /** Emitted when the host tells the GUI to clear the program list. */
  Glib::Dispatcher clear_programs_received;
  
private:
  
  // Adjustment connection handling (for host->UI messages)
  void update_adjustments(int port, float value);
  
  // Static callbacks for OSC method calls (liblo is not C++)
  static int control_handler(const char *path, const char *types,
                             lo_arg **argv, int argc, 
                             void *data, void *user_data);
  static int program_handler(const char *path, const char *types,
                             lo_arg **argv, int argc, 
                             void *data, void *user_data);
  static int configure_handler(const char *path, const char *types,
                               lo_arg **argv, int argc, 
                               void *data, void *user_data);
  static int filename_handler(const char *path, const char *types,
                              lo_arg **argv, int argc, 
                              void *data, void *user_data);
  static int tell_gui_handler(const char *path, const char *types,
                              lo_arg **argv, int argc, 
                              void *data, void *user_data);
  static int show_handler(const char *path, const char *types,
                          lo_arg **argv, int argc, 
                          void *data, void *user_data);
  static int hide_handler(const char *path, const char *types,
                          lo_arg **argv, int argc, 
                          void *data, void *user_data);
  static int add_program_handler(const char *path, const char *types,
                                 lo_arg **argv, int argc, 
                                 void *data, void *user_data);
  static int remove_program_handler(const char *path, const char *types,
                                    lo_arg **argv, int argc, 
                                    void *data, void *user_data);
  static int clear_programs_handler(const char *path, const char *types,
                                    lo_arg **argv, int argc, 
                                    void *data, void *user_data);
  static int quit_handler(const char *path, const char *types,
                          lo_arg **argv, int argc, 
                          void *data, void *user_data);
  
  // Dispatchers that get the signals from the OSC thread to the GUI thread
  // (the queues and receiver functions are needed for passing data since 
  // Dispatchers don't take parameters)
  Glib::Dispatcher m_control_dispatcher;
  std::queue<std::pair<int, float> > m_control_queue;
  void control_receiver() {
    int port = m_control_queue.front().first;
    float value = m_control_queue.front().second;
    m_control_queue.pop();
    if (unsigned(port) < m_adjustments.size() && m_adjustments[port] != NULL)
      m_adjustments[port]->set_value(value);
    control_received(port, value);
  }

  Glib::Dispatcher m_program_dispatcher;
  std::queue<int> m_program_queue;
  void program_receiver() {
    int program = m_program_queue.front();
    m_program_queue.pop();
    m_program_is_set = true;
    m_active_program = program;
    program_received(program);
  }

  Glib::Dispatcher m_configure_dispatcher;
  std::queue<std::pair<std::string, std::string> > m_configure_queue;
  void configure_receiver() {
    std::string key = m_configure_queue.front().first;
    std::string value = m_configure_queue.front().second;
    m_configure_queue.pop();
    configure_received(key, value);
  }
  
  Glib::Dispatcher m_filename_dispatcher;
  std::queue<std::pair<std::string, std::string> > m_filename_queue;
  void filename_receiver() {
    std::string key = m_filename_queue.front().first;
    std::string value = m_filename_queue.front().second;
    m_filename_queue.pop();
    filename_received(key, value);
  }

  Glib::Dispatcher m_tell_gui_dispatcher;
  std::queue<std::pair<uint32_t, char**> > m_tell_gui_queue;
  void tell_gui_receiver() {
    std::cerr<<__PRETTY_FUNCTION__<<std::endl;
    uint32_t argc = m_tell_gui_queue.front().first;
    char** argv = m_tell_gui_queue.front().second;
    m_tell_gui_queue.pop();
    tell_gui_received(argc, argv);
    // XXX shouldn't I deallocate the buffers here? it crashes
  }

  Glib::Dispatcher m_add_program_dispatcher;
  std::queue<std::pair<int, std::string> > m_add_program_queue;
  void add_program_receiver() {
    int number = m_add_program_queue.front().first;
    std::string name = m_add_program_queue.front().second;
    m_add_program_queue.pop();
    add_program_received(number, name);
  }
  
  Glib::Dispatcher m_remove_program_dispatcher;
  std::queue<int> m_remove_program_queue;
  void remove_program_receiver() {
    int number = m_remove_program_queue.front();
    m_remove_program_queue.pop();
    remove_program_received(number);
  }
  
  lo_address m_plugin_address;
  std::string m_plugin_path;
  lo_server_thread m_server_thread;
  
  bool m_valid;
  std::string m_identifier;
  std::string m_bundle;
  
  std::vector<Gtk::Adjustment*> m_adjustments;
  bool m_blocking;
  
  bool m_program_is_set;
  unsigned long m_active_program;
};


#endif

