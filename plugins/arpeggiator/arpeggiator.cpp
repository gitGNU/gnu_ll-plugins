/****************************************************************************
    
    arpeggiator.cpp - simple MIDI arpeggiator plugin
    
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

#include <cstring>
#include <iostream>

#include "lv2plugin.hpp"
#include "lv2-miditype.h"


using namespace std;


/** This is the class that contains all the code and data for the Sineshaper
    synth plugin. */
class Arpeggiator : public LV2Plugin {
public:
  
  Arpeggiator(uint32_t, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(5),
      m_num_keys(0),
      m_frame_counter(0),
      m_next_key(0) {

  }
  
  
  void activate() {
    m_num_keys = 0;
    m_frame_counter = 0;
    m_next_key = 0;
  }

  
  void run(uint32_t nframes) {
    
    float& npb = *static_cast<float*>(m_ports[0]);
    float& plength = *static_cast<float*>(m_ports[1]);
    float& direction = *static_cast<float*>(m_ports[2]);
    LV2_MIDI* midi_in = static_cast<LV2_MIDI*>(m_ports[3]);
    LV2_MIDI* midi_out = static_cast<LV2_MIDI*>(m_ports[4]);
    
    const unsigned char* in_data = midi_in->data;
    unsigned long last_timestamp = 0;
    unsigned long timestamp;
    size_t in_data_size;
    unsigned char* out_data = midi_out->data;
    
    midi_out->event_count = 0;
    midi_out->size = 0;
    
    // iterate over incoming MIDI events
    for (size_t in_event = 0; in_event < midi_in->event_count; ++in_event) {
      
      timestamp = *reinterpret_cast<const unsigned long*>(in_data);
      in_data += sizeof(unsigned long);
      in_data_size = *reinterpret_cast<const size_t*>(in_data);
      in_data += sizeof(size_t);
      
      // generate events for the time between last event and this event
      if (timestamp > last_timestamp)
        generate_events(midi_out, out_data, last_timestamp, timestamp);
      
      last_timestamp = timestamp;
      
      if (in_data_size == 3) {
        const unsigned char& key = in_data[1];
        
        // note off - erase the key from the array
        if (((in_data[0] & 0xF0) == 0x80) ||
            ((in_data[0] & 0xF0) == 0x90 && in_data[2] == 0)) {
          unsigned pos = 0;
          for ( ; pos < m_num_keys && m_keys[pos] != key; ++pos);
          if (pos < m_num_keys) {
            --m_num_keys;
            if (m_next_key > pos)
              --m_next_key;
            for (unsigned i = pos; i < m_num_keys; ++i)
              m_keys[i] = m_keys[i + 1];
            //for (unsigned i = 0; i < m_num_keys; ++i)
            //  cerr<<" "<<int(m_keys[i]);
            //cerr<<endl;
          }
        }
          
        // note on - insert the key in the array
        else if ((in_data[0] & 0xF0) == 0x90) {
          unsigned pos = 0;
          for ( ; pos < m_num_keys && m_keys[pos] < key; ++pos);
          if (m_keys[pos] != key || pos == m_num_keys) {
            for (unsigned i = m_num_keys; i > pos; --i)
              m_keys[i] = m_keys[i - 1];
            m_keys[pos] = key;
            ++m_num_keys;
            
            //for (unsigned i = 0; i < m_num_keys; ++i)
            //  cerr<<" "<<int(m_keys[i]);
            //cerr<<endl;
            if (m_next_key > pos)
              ++m_next_key;
          }
        }
        
      }
      
      in_data += in_data_size;
    }
    
    // generate events for the time between the last event and the 
    // end of the cycle
    generate_events(midi_out, out_data, last_timestamp, nframes);
    
    m_frame_counter += nframes;
    
    midi_out->size = out_data - midi_out->data;
  }
  
  
  void generate_events(LV2_MIDI* midi_out, unsigned char*& out_data,
                       unsigned long from, unsigned long to) {

    if (m_num_keys == 0)
      return;
    
    unsigned long step_length = 12000;
    unsigned long frame = m_frame_counter + from + 
      step_length - ((m_frame_counter + from) % step_length);
    
    
    
    
    size_t event_size = sizeof(unsigned long) + sizeof(size_t) + 3;
    
    for ( ; frame < m_frame_counter + to; frame += step_length) {
      
      //cerr<<"writing event at frame "<<(frame - m_frame_counter)<<endl;

      if (out_data - midi_out->data + event_size >= midi_out->capacity)
        return;
      unsigned long step = frame / step_length;
      *reinterpret_cast<unsigned long*>(out_data) = frame - m_frame_counter;
      out_data += sizeof(unsigned long);
      *reinterpret_cast<size_t*>(out_data) = 3;
      out_data += sizeof(size_t);
      out_data[0] = 0x90;
      out_data[1] = m_keys[m_next_key];
      out_data[2] = 0x60;
      out_data += 3;
      ++midi_out->event_count;
      m_next_key = (m_next_key + 1) % m_num_keys;
    }
  }
  
                       
protected:
  
  int m_keys[128];
  unsigned char m_num_keys;
  unsigned long m_frame_counter;
  unsigned char m_next_key;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2<Arpeggiator>("http://ll-plugins.nongnu.org/lv2/dev/arpeggiator/0.0.0");
}
