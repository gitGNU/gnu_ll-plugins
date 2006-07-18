#include <csignal>
#include <iostream>
#include <vector>
#include <unistd.h>

#include <jack/jack.h>
#include <jack/midiport.h>

#include "lv2host.hpp"
#include "lv2-miditype.h"

#include "osccontroller.hpp"


using namespace std;


vector<jack_port_t*> jack_ports;
jack_client_t* jack_client;


/** Translate from an LV2 MIDI buffer to a JACK MIDI buffer. */
void lv2midi2jackmidi(LV2Port& port, jack_port_t* jack_port, 
                      jack_nframes_t nframes) {
  jack_nframes_t timestamp;
  size_t data_size;
  void* output_buf = jack_port_get_buffer(jack_port, nframes);
  LV2_MIDI* input_buf = static_cast<LV2_MIDI*>(port.buffer);
  
  jack_midi_clear_buffer(output_buf, nframes);
  
  // iterate over all MIDI events and write them to the JACK port
  unsigned char* data = input_buf->data;
  
  for (size_t i = 0; i < input_buf->event_count; ++i) {
    
    // retrieve LV2 MIDI event
    timestamp = static_cast<jack_nframes_t>(*reinterpret_cast<double*>(data));
    data += sizeof(double);
    data_size = *reinterpret_cast<size_t*>(data);
    data += sizeof(size_t);
    
    // write JACK MIDI event
    jack_midi_event_write(output_buf, timestamp, 
                          reinterpret_cast<jack_midi_data_t*>(data),
                          data_size, nframes);
    data += data_size;
  }
}


/** Translate from a JACK MIDI buffer to an LV2 MIDI buffer. */
void jackmidi2lv2midi(jack_port_t* jack_port, LV2Port& port,
                      LV2Host& host, jack_nframes_t nframes) {
  void* input_buf = jack_port_get_buffer(jack_port, nframes);
  jack_midi_event_t input_event;
  jack_nframes_t input_event_index = 0;
  jack_nframes_t input_event_count = 
    jack_midi_port_get_info(input_buf, nframes)->event_count;
  jack_nframes_t timestamp;
  LV2_MIDI* output_buf = static_cast<LV2_MIDI*>(port.buffer);
  output_buf->event_count = 0;
  
  // iterate over all incoming JACK MIDI events
  unsigned char* data = output_buf->data;
  for (unsigned int i = 0; i < input_event_count; ++i) {
    
    // retrieve JACK MIDI event
    jack_midi_event_get(&input_event, input_buf, i, nframes);
    if ((data - output_buf->data) + sizeof(double) + 
        sizeof(size_t) + input_event.size >= output_buf->capacity)
      break;
    
    // check if it's a mapped CC
    if ((input_event.size == 3) && ((input_event.buffer[0] & 0xF0) == 0xB0) &&
        (host.get_midi_map()[input_event.buffer[1]] != -1)) {
      int port = host.get_midi_map()[input_event.buffer[1]];
      float* pbuf = static_cast<float*>(host.get_ports()[port].buffer);
      float& min = host.get_ports()[port].min_value;
      float& max = host.get_ports()[port].max_value;
      *pbuf = min + (max - min) * input_event.buffer[2] / 127.0;
    }
    
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

  output_buf->used_capacity = data - output_buf->data;
  
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
      if (!port.midi)
        port.buffer = jack_port_get_buffer(jack_ports[i], nframes);
      
      // MIDI input port, copy the events one by one
      else if (port.rate == ControlRate && port.direction == InputPort)
        jackmidi2lv2midi(jack_ports[i], port, *host, nframes);
      
    }
  }
  
  // run the plugin!
  host->run(nframes);
  
  // Copy events from MIDI output ports to JACK ports
  for (size_t i = 0; i < host->get_ports().size(); ++i) {
    if (jack_ports[i]) {
      LV2Port& port = host->get_ports()[i];
      if (port.midi && port.direction == OutputPort && port.rate== ControlRate)
        lv2midi2jackmidi(port, jack_ports[i], nframes);
    }
  }
  
  return 0;
}


int main(int argc, char** argv) {
  
  if (argc < 2) {
    cerr<<"usage: "<<argv[0]<<" PLUGIN_URI"<<endl
        <<"example: "<<argv[0]
        <<" 'http://ll-plugins.nongnu.org/lv2/sineshaper/0.0.0'"<<endl;
    return 1;
  }
  
  // load plugin
  LV2Host lv2h(argv[1], 48000);
  
  if (lv2h.is_valid()) {
    
    
    cerr<<"MIDI map:"<<endl;
    for (unsigned i = 0; i < 127; ++i) {
      long port = lv2h.get_midi_map()[i];
      if (port == -1)
        continue;
      cerr<<"  "<<i<<" -> "<<port<<" ("
          <<lv2h.get_ports()[port].symbol<<")"<<endl;
    }
    cerr<<endl;
    
    cerr<<"Default MIDI port: "<<lv2h.get_default_midi_port()<<endl;
    
    if (lv2h.get_gui_path().size())
      cerr<<"Standalone GUI: "<<lv2h.get_gui_path()<<endl;
    else
      cerr<<"No standalone GUI"<<endl;
    
    // initialise JACK client and plugin port buffers
    jack_client = jack_client_new("LV2Host");
    for (size_t i = 0; i < lv2h.get_ports().size(); ++i) {
      jack_port_t* port = 0;
      LV2Port& lv2port = lv2h.get_ports()[i];
      
      // add JACK MIDI port and allocate internal MIDI buffer
      if (lv2port.midi) {
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
      else if (lv2port.rate == AudioRate) {
        port = jack_port_register(jack_client, lv2port.symbol.c_str(),
                                  JACK_DEFAULT_AUDIO_TYPE,
                                  (lv2port.direction == InputPort ?
                                   JackPortIsInput : JackPortIsOutput), 0);
      }
      
      // for control ports, just create buffers consisting of a single float
      else if (lv2port.rate == ControlRate)
        lv2port.buffer = new float;
      
      jack_ports.push_back(port);
    }
    jack_set_process_callback(jack_client, &process, &lv2h);
    lv2h.activate();
    jack_activate(jack_client);
    
    OSCController osc(lv2h);
    osc.start();
    cerr<<"Listening on URL "<<osc.get_url()<<endl;
    
    // start the GUI
    string gui = lv2h.get_gui_path();
    pid_t gui_pid = 0;
    if (gui.size()) {
      if (!(gui_pid = fork())) {
        execl(gui.c_str(), gui.c_str(), osc.get_url().c_str(),
              lv2h.get_bundle_dir().c_str(), argv[1], 
              "User friendly identifier!", 0);
        cerr<<"Could not execute "<<gui<<endl;
        exit(-1);
      }
    }
    
    // wait until we are killed
    while (true) {
      string word;
      cin>>word;
      if (word == "configure") {
        string key, value;
        cin>>key>>value;
        lv2h.configure(key.c_str(), value.c_str());
      }
      else {
        cout<<"Unknown command \""<<word<<"\""<<endl;
      }
    }
    
    // kill the GUI
    if (gui_pid) {
      kill(gui_pid, SIGQUIT);
      wait(gui_pid);
    }
    
    osc.stop();
    jack_client_close(jack_client);
    lv2h.deactivate();
  }
  
  else {
    cerr<<"Could not find the plugin with URI <"<<argv[1]<<">"<<endl;
    return 1;
  }
  
  return 0;
}
