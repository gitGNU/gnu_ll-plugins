/****************************************************************************
    
    main.cpp - Main source file for the LV2 host Elven
    
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

#include <csignal>
#include <fstream>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sstream>

#include <sys/wait.h>

#include <jack/jack.h>
#include <jack/midiport.h>
#include <lash/lash.h>

#include "lv2host.hpp"
#include "lv2-midiport.h"
#include "lv2-midifunctions.h"
#include "osccontroller.hpp"
#include "eventqueue.hpp"
#include "debug.hpp"
#include "midiutils.hpp"


using namespace std;
using namespace sigc;


vector<jack_port_t*> jack_ports;
jack_client_t* jack_client;
lash_client_t* lash_client;
bool still_running;


string escape_space(const string& str) {
  string str2 = str;
  int pos = 0;
  while ((pos = str2.find("\\", pos)) != string::npos) {
    str2.replace(pos, 1, "\\b");
    pos += 2;
  }
  pos = 0;
  while ((pos = str2.find(" ", pos)) != string::npos) {
    str2.replace(pos, 1, "\\s");
    pos += 2;
  }
  pos = 0;
  while ((pos = str2.find("\t", pos)) != string::npos) {
    str2.replace(pos, 1, "\\t");
    pos += 2;
  }
  pos = 0;
  while ((pos = str2.find("\n", pos)) != string::npos) {
    str2.replace(pos, 1, "\\n");
    pos += 2;
  }
  pos = 0;
  while ((pos = str2.find("\r", pos)) != string::npos) {
    str2.replace(pos, 1, "\\r");
    pos += 2;
  }
  return str2;
}


string unescape_space(const string& str) {
  string str2 = str;
  int pos = 0;
  while ((pos = str2.find("\\b", pos)) != string::npos) {
    str2.replace(pos, 2, "\\");
    pos += 1;
  }
  while ((pos = str2.find("\\r", pos)) != string::npos) {
    str2.replace(pos, 2, "\r");
    pos += 1;
  }
  while ((pos = str2.find("\\n", pos)) != string::npos) {
    str2.replace(pos, 2, "\n");
    pos += 1;
  }
  while ((pos = str2.find("\\t", pos)) != string::npos) {
    str2.replace(pos, 2, "\t");
    pos += 1;
  }
  while ((pos = str2.find("\\s", pos)) != string::npos) {
    str2.replace(pos, 2, " ");
    pos += 1;
  }
  pos = 0;
  return str2;
}


bool init_lash(int argc, char** argv, const char* jackname) {
  DBG2("Initialising LASH client");
  lash_client = lash_init(lash_extract_args(&argc, &argv), "elven", 
                          LASH_Config_File, LASH_PROTOCOL(2, 0));
  
  if (lash_client) {
    lash_event_t* event = lash_event_new_with_type(LASH_Client_Name);
    lash_event_set_string(event, "Elven");
    lash_send_event(lash_client, event);      
    lash_jack_client_name(lash_client, jackname);
  }
  else
    DBG0("Could not initialise LASH!");
  
  return (lash_client != 0);
}


/** Translate from an LV2 MIDI buffer to a JACK MIDI buffer. */
void lv2midi2jackmidi(LV2Port& port, jack_port_t* jack_port, 
                      jack_nframes_t nframes) {
  
  DBG4("Translating MIDI events from LV2 to JACK for port "<<port.symbol);

  
  double timestamp;
  uint32_t data_size;
  unsigned char* data = 0;
  void* output_buf = jack_port_get_buffer(jack_port, nframes);
  LV2_MIDI* input_buf = static_cast<LV2_MIDI*>(port.buffer);
  LV2_MIDIState in = { static_cast<LV2_MIDI*>(port.buffer), nframes, 0 };
  
  jack_midi_clear_buffer(output_buf, nframes);
  
  // iterate over all MIDI events and write them to the JACK port
  for (size_t i = 0; i < input_buf->event_count; ++i) {
    
    // retrieve LV2 MIDI event
    lv2midi_get_event(&in, &timestamp, &data_size, &data);
    lv2midi_step(&in);
    
    DBG3("Received MIDI event from the plugin on port "<<port.symbol
         <<": "<<midi2str(data_size, data));
    
    // write JACK MIDI event
    jack_midi_event_write(output_buf, jack_nframes_t(timestamp), 
                          reinterpret_cast<jack_midi_data_t*>(data),
                          data_size, nframes);
  }
}


/** Translate from a JACK MIDI buffer to an LV2 MIDI buffer. */
void jackmidi2lv2midi(jack_port_t* jack_port, LV2Port& port,
                      LV2Host& host, jack_nframes_t nframes) {
  
  DBG4("Translating MIDI events from JACK to LV2 for port "<<port.symbol);
  
  static unsigned bank = 0;
  
  void* input_buf = jack_port_get_buffer(jack_port, nframes);
  jack_midi_event_t input_event;
  jack_nframes_t input_event_index = 0;
  jack_nframes_t input_event_count = 
    jack_midi_get_event_count(input_buf, nframes);
  jack_nframes_t timestamp;
  LV2_MIDI* output_buf = static_cast<LV2_MIDI*>(port.buffer);
  output_buf->event_count = 0;
  
  // iterate over all incoming JACK MIDI events
  unsigned char* data = output_buf->data;
  for (unsigned int i = 0; i < input_event_count; ++i) {
    
    // retrieve JACK MIDI event
    jack_midi_event_get(&input_event, input_buf, i, nframes);
    
    DBG3("Received MIDI event from JACK on port "<<port.symbol
         <<": "<<midi2str(input_event.size, input_event.buffer));
        

    
    if ((data - output_buf->data) + sizeof(double) + 
        sizeof(size_t) + input_event.size >= output_buf->capacity)
      break;
    
    // check if it's a bank select MSB
    if ((input_event.size == 3) && ((input_event.buffer[0] & 0xF0) == 0xB0) &&
        (input_event.buffer[1] == 0)) {
      bank = (bank & 0x7F) + (input_event.buffer[2] & 0x7F) << 7;
    }
    
    // LSB
    else if ((input_event.size == 3) && 
             ((input_event.buffer[0] & 0xF0) == 0xB0) &&
             (input_event.buffer[1] == 32)) {
      bank = (bank & (0x7F << 7)) + (input_event.buffer[2] & 0x7F);
    }
    
    // or a mapped CC
    else if ((input_event.size == 3) && 
             ((input_event.buffer[0] & 0xF0) == 0xB0) &&
             (host.get_midi_map()[input_event.buffer[1]] != -1)) {
      int port = host.get_midi_map()[input_event.buffer[1]];
      float* pbuf = static_cast<float*>(host.get_ports()[port].buffer);
      float& min = host.get_ports()[port].min_value;
      float& max = host.get_ports()[port].max_value;
      *pbuf = min + (max - min) * input_event.buffer[2] / 127.0;
      host.queue_control(port, *pbuf, false);
    }
    
    // or a program change
    /*
      else if ((input_event.size == 2) && 
      ((input_event.buffer[0] & 0xF0) == 0xC0)) {
      host.select_program(128 * bank + input_event.buffer[1]);
      }
    */
    
    else {
      // write LV2 MIDI event
      *reinterpret_cast<double*>(data) = input_event.time;
      data += sizeof(double);
      *reinterpret_cast<size_t*>(data) = input_event.size;
      data += sizeof(size_t);
      memcpy(data, input_event.buffer, input_event.size);
      
      // normalise note events if needed
      if ((input_event.size == 3) && ((data[0] & 0xF0) == 0x90) && 
          (data[2] == 0))
        data[0] = 0x80 | (data[0] & 0x0F);
      
      data += input_event.size;
      ++output_buf->event_count;
    }
  }

  output_buf->size = data - output_buf->data;
  
}


/** The JACK process callback */
int process(jack_nframes_t nframes, void* arg) {
  
  LV2Host* host = static_cast<LV2Host*>(arg);
  
  // iterate over all ports and copy data from JACK ports to audio and MIDI
  // ports in the plugin
  for (size_t i = 0; i < host->get_ports().size(); ++i) {
    
    // does this plugin port have an associated JACK port?
    if (jack_ports[i]) {

      LV2Port& port = host->get_ports()[i];
      
      // audio port, just copy the buffer pointer.
      if (port.type == AudioType)
        port.buffer = jack_port_get_buffer(jack_ports[i], nframes);
      
      // MIDI input port, copy the events one by one
      else if (port.type == MidiType && port.direction == InputPort)
        jackmidi2lv2midi(jack_ports[i], port, *host, nframes);
      
    }
  }
  
  // run the plugin!
  host->run(nframes);
  
  // Copy events from MIDI output ports to JACK ports
  for (size_t i = 0; i < host->get_ports().size(); ++i) {
    if (jack_ports[i]) {
      LV2Port& port = host->get_ports()[i];
      if (port.type == MidiType && port.direction == OutputPort)
        lv2midi2jackmidi(port, jack_ports[i], nframes);
    }
  }
  
  return 0;
}


void thread_init(void*) {
  DebugInfo::thread_prefix()[pthread_self()] = "J ";
}


void sigchild(int signal) {
  DBG2("Child process terminated");
  if (signal == SIGCHLD)
    still_running = false;
}


void print_usage(const char* argv0) {
  cerr<<"usage:   "<<argv0<<" --help"<<endl
      <<"         "<<argv0<<" --list"<<endl
      <<"         "<<argv0<<" [--debug DEBUGLEVEL] PLUGIN_URI"<<endl<<endl
      <<"example: "<<argv0
      <<" 'http://ll-plugins.nongnu.org/lv2/dev/klaviatur/0.0.0'"<<endl;
}


int main(int argc, char** argv) {
  
  DebugInfo::prefix() = "H:";
  DebugInfo::thread_prefix()[pthread_self()] = "M ";
  
  if (argc < 2) {
    print_usage(argv[0]);
    return 1;
  }
  
  int i;
  for (i = 1; i < argc; ++i) {
    
    // print help
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
      cerr<<"Elven is an (E)xperimental (LV)2 (E)xecution e(N)vironment.\n\n"
          <<"(C) 2006 Lars Luthman <lars.luthman@gmail.com>\n"
          <<"Released under the GNU General Public License, version 2 or later."
          <<endl<<endl;
      print_usage(argv[0]);
      return 0;
    }
    
    // list all available plugins
    else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--list")) {
      LV2Host::list_plugins();
      return 0;
    }
    
    // set debugging level (higher level -> more messages)
    else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug")) {
      if (i == argc - 1) {
        cerr<<"No debug level given!"<<endl;
        return 1;
      }
      DebugInfo::level() = atoi(argv[i + 1]);
      ++i;
    }
    
    else
      break;
  }
  
  if (i >= argc) {
    print_usage(argv[0]);
    return 1;
  }
    
  // load plugin
  LV2Host lv2h(argv[i], 48000);
  
  if (lv2h.is_valid()) {
    
    DBG2("Plugin host is OK");
    
    bool has_map = false;
    for (unsigned j = 0; j < 127; ++j) {
      long port = lv2h.get_midi_map()[j];
      if (port == -1)
        continue;
      if (!has_map)
        DBG2("MIDI map:");
      has_map = true;
      DBG2("  "<<j<<" -> "<<port<<" ("<<lv2h.get_ports()[port].symbol<<")");
    }
    if (has_map)
      DBG2("");
    
    DBG2("Default MIDI port: "<<lv2h.get_default_midi_port());
    
    // initialise JACK client and plugin port buffers
    if (!(jack_client = jack_client_open(lv2h.get_name().c_str(), 
                                         jack_options_t(0), 0))) {
      cerr<<"jackd isn't running!"<<endl;
      return -1;
    }
      
    if (!init_lash(argc, argv, jack_get_client_name(jack_client))) {
      cerr<<"Could not initialise LASH"<<endl;
      return -1;
    }
    
    for (size_t p = 0; p < lv2h.get_ports().size(); ++p) {
      jack_port_t* port = 0;
      LV2Port& lv2port = lv2h.get_ports()[p];
      
      // add JACK MIDI port and allocate internal MIDI buffer
      if (lv2port.type == MidiType) {
        port = jack_port_register(jack_client, lv2port.symbol.c_str(),
                                  JACK_DEFAULT_MIDI_TYPE,
                                  (lv2port.direction == InputPort ?
                                   JackPortIsInput : JackPortIsOutput), 0);
        LV2_MIDI* mbuf = new LV2_MIDI;
        mbuf->capacity = 8192;
        mbuf->data = new unsigned char[8192];
        lv2port.buffer = mbuf;
      }
      
      // add JACK audio port
      else if (lv2port.type == AudioType) {
        port = jack_port_register(jack_client, lv2port.symbol.c_str(),
                                  JACK_DEFAULT_AUDIO_TYPE,
                                  (lv2port.direction == InputPort ?
                                   JackPortIsInput : JackPortIsOutput), 0);
      }
      
      // for control ports, just create buffers consisting of a single float
      else if (lv2port.type == ControlType) {
        lv2port.buffer = new float;
        *static_cast<float*>(lv2port.buffer) = lv2port.default_value;
      }
      
      jack_ports.push_back(port);
    }
    jack_set_process_callback(jack_client, &process, &lv2h);
    jack_set_thread_init_callback(jack_client, &thread_init, 0);
    lv2h.activate();
    jack_activate(jack_client);
    
    still_running = true;
    OSCController osc(lv2h, still_running);
    //osc.start();
    
    // start the GUI
    string gui_uri = lv2h.get_gui_uri();
    string gui_path = lv2h.get_gui_path();
    pid_t gui_pid = 0;
    if (gui_path.size()) {
      if (!(gui_pid = fork())) {
        ostringstream oss;
        oss<<DebugInfo::level();
        execlp("elven_guiloader", "elven_guiloader", oss.str().c_str(), 
               gui_path.c_str(), osc.get_url().c_str(), gui_uri.c_str(), 
	       argv[i], lv2h.get_bundle_dir().c_str(), lv2h.get_name().c_str(),
	       0);
        DBG0("Could not execute elven_guiloader");
        exit(-1);
      }
      else if (gui_pid > 0)
        ::signal(SIGCHLD, &sigchild);
    }
    
    lv2h.signal_configure.connect(mem_fun(osc, &OSCController::send_configure));
    lv2h.signal_filename.connect(mem_fun(osc, &OSCController::send_filename));
    
    // queue that the host can pass configurations to
    EventQueue conf_q;
    
    // wait until we are killed
    while (still_running) {
      
      while (osc.run());
      
      lash_event_t* event;
      while ((event = lash_get_event(lash_client))) {
        
        // save
        if (lash_event_get_type(event) == LASH_Save_File) {

          ofstream of((string(lash_event_get_string(event)) + 
                       "/lv2host").c_str());
          
          // configuration
          const map<string, string>& config = lv2h.get_config();
          map<string, string>::const_iterator iter;
          for (iter = config.begin(); iter != config.end(); ++iter) {
            of<<"configure "<<escape_space(iter->first)<<" "
              <<escape_space(iter->second)<<endl;
          }
          
          // data files
          const map<string, string>& filenames = lv2h.get_filenames();
          for (iter = filenames.begin(); iter != filenames.end(); ++iter) {
            of<<"set_file "<<escape_space(iter->first)<<" "
              <<escape_space(iter->second)<<endl;
          }
          
          lv2h.queue_config_request(&conf_q);
          EventQueue::Type t;
          do {
            while (!conf_q.wait() && still_running);
            t = conf_q.read_event();
            switch (t) {
            case EventQueue::Program:
              of<<"program "<<conf_q.get_event().program.program<<endl;
              break;
            case EventQueue::Control:
              of<<"control "<<conf_q.get_event().control.port<<" "
                <<conf_q.get_event().control.value<<endl;
            }
          } while (t != EventQueue::Passthrough && still_running);

          lash_send_event(lash_client, 
                          lash_event_new_with_type(LASH_Save_File));
        }
        
        // restore
        else if (lash_event_get_type(event) == LASH_Restore_File) {

          ifstream ifs((string(lash_event_get_string(event)) + 
                        "/lv2host").c_str());
          while (ifs) {
            string word;
            ifs>>word;

            if (word == "configure") {
              string key, value;
              ifs>>key>>value;
              key = unescape_space(key);
              value = unescape_space(value);
              lv2h.configure(key.c_str(), value.c_str());
              osc.send_configure(key, value);
            }

            if (word == "set_file") {
              string key, filename;
              ifs>>key>>filename;
              key = unescape_space(key);
              filename = unescape_space(filename);
              lv2h.set_file(key.c_str(), filename.c_str());
              osc.send_filename(key, filename);
            }

            if (word == "control") {
              unsigned long port;
              float value;
              ifs>>port>>value;
              lv2h.queue_control(port, value);
            }
              
          }
          lash_send_event(lash_client,
                          lash_event_new_with_type(LASH_Restore_File));
        }
        
        // quit
        else if (lash_event_get_type(event) == LASH_Quit)
          still_running = false;
        
        lash_event_destroy(event);
      }
      
      usleep(100000);
    }
    
    // kill the GUI
    if (gui_pid) {
      kill(gui_pid, SIGKILL);
      waitpid(gui_pid, 0, 0);
    }
    
    //osc.stop();
    jack_client_close(jack_client);
    lv2h.deactivate();
  }
  
  else {
    return 1;
  }
  
  DBG2("Exiting");
  
  return 0;
}
