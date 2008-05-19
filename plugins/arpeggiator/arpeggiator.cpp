/****************************************************************************
    
    arpeggiator.cpp - simple MIDI arpeggiator plugin
    
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

#include <cstring>

#include <lv2plugin.hpp>
#include <lv2_event_helpers.h>


using namespace std;


/** A plugin that takes chord MIDI input and writes arpeggio MIDI output. */
class Arpeggiator : public LV2::Plugin<Arpeggiator, LV2::URIMap<true> > {
public:
  
  Arpeggiator(double rate) 
    : LV2::Plugin<Arpeggiator, LV2::URIMap<true> >(4),
      m_rate(rate),
      m_num_keys(0),
      m_frame_counter(0),
      m_next_key(0),
      m_last_key(0),
      m_midi_type(uri_to_id(LV2_EVENT_URI, 
			    "http://lv2plug.in/ns/ext/midi#MidiEvent")),
      m_running(false),
      m_upwards(true) {
    
  }
  
  
  void activate() {
    m_num_keys = 0;
    m_frame_counter = 0;
    m_next_key = 0;
  }

  
  void run(uint32_t nframes) {
    
    float& npb = *p(0);
    m_upwards = *p(1) > 0.5;
    
    LV2_Event_Iterator iter_in, iter_out;
    lv2_event_begin(&iter_in, p<LV2_Event_Buffer>(2));
    LV2_Event_Buffer* outbuf = p<LV2_Event_Buffer>(3);
    lv2_event_buffer_reset(outbuf, outbuf->stamp_type, outbuf->data);
    lv2_event_begin(&iter_out, outbuf);
    
    double event_time;
    uint32_t event_size;
    unsigned char* event_data;
    double frames_done = 0;
    
    // iterate over incoming MIDI events
    while (true) {
      uint32_t event_time = nframes;
      LV2_Event* ev = 0;
      if (lv2_event_is_valid(&iter_in)) {
	ev = lv2_event_get(&iter_in, &event_data);
	event_time = ev->frames;
	lv2_event_increment(&iter_in);
      }
      if (event_time > frames_done)
        generate_events(&iter_out, frames_done, event_time);
      frames_done = event_time;
      
      if (!ev)
	break;
      
      if (ev->type == m_midi_type && ev->size == 3) {
        const unsigned char& key = event_data[1];
        
        // note off - erase the key from the array
        if ((event_data[0] & 0xF0) == 0x80) {
          unsigned pos = 0;
          for ( ; pos < m_num_keys && m_keys[pos] != key; ++pos);
          if (pos < m_num_keys) {
            --m_num_keys;
            if (m_upwards && m_next_key > pos)
              --m_next_key;
	    else if (!m_upwards && m_next_key == pos) {
	      if (m_next_key == 0)
		m_next_key = m_num_keys - 1;
	      else
		--m_next_key;
	    }
            for (unsigned i = pos; i < m_num_keys; ++i)
              m_keys[i] = m_keys[i + 1];
          }
        }
          
        // note on - insert the key in the array
        else if ((event_data[0] & 0xF0) == 0x90) {
          unsigned pos = 0;
          for ( ; pos < m_num_keys && m_keys[pos] < key; ++pos);
          if (m_keys[pos] != key || pos == m_num_keys) {
            for (unsigned i = m_num_keys; i > pos; --i)
              m_keys[i] = m_keys[i - 1];
            m_keys[pos] = key;
            ++m_num_keys;
            if (m_next_key > pos)
              ++m_next_key;
	    if (pos == m_num_keys - 1 && m_next_key == 0)
	      m_next_key = pos;
          }
	  if (!m_running) {
	    m_running = true;
	    if (m_upwards)
	      m_next_key = 0;
	    else
	      m_next_key = m_num_keys - 1;
	  }
        }
        
      }
      
    }
    
  }
  
  
  void generate_events(LV2_Event_Iterator* midi_out, 
		       uint32_t from, uint32_t to) {

    if (!m_running)
      return;
    
    if (*p(0) <= 0) {
      m_frame_counter = 0;
      return;
    }
      
    uint32_t step_length = m_rate / *p(0);
    
    uint32_t frame = from + m_frame_counter;
    frame = frame >= from ? frame : from;
    
    for ( ; frame < to; frame += step_length) {
      unsigned char data[] = { 0x80, m_last_key, 0x60 };
      lv2_event_write(midi_out, frame, 0, m_midi_type, 3, data);
      if (m_num_keys == 0) {
	m_running = false;
	m_frame_counter = 0;
	return;
      }
      data[0] = 0x90;
      data[1] = m_keys[m_next_key];
      lv2_event_write(midi_out, frame, 0, m_midi_type, 3, data);
      m_last_key = data[1];
      if (m_upwards)
	m_next_key = (m_next_key + 1) % m_num_keys;
      else
	m_next_key = (m_next_key + m_num_keys - 1) % m_num_keys;
    }
    
    m_frame_counter = frame - to;
  }
  
                       
protected:
  
  double m_rate;
  int m_keys[128];
  unsigned char m_num_keys;
  uint32_t m_frame_counter;
  unsigned char m_next_key;
  unsigned char m_last_key;
  uint32_t m_midi_type;
  bool m_running;
  bool m_upwards;
  
};



static unsigned _ = 
  Arpeggiator::register_class("http://ll-plugins.nongnu.org/lv2/arpeggiator#0"); 
