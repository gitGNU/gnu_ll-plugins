/****************************************************************************
    
    midiproc.cpp - A regular expression processor for MIDI events
    
    Copyright (C) 2008 Lars Luthman <lars.luthman@gmail.com>
    
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

#include <iostream>
#include <lv2plugin.hpp>
#include "midiproc.peg"
#include "lv2_event_helpers.h"


using namespace LV2;
using namespace std;


class MIDIProc : public Plugin<MIDIProc, URIMap<true> > {
public:
  
  MIDIProc(double rate) 
    : Plugin<MIDIProc, URIMap<true> >(m_n_ports),
      m_midi_type(uri_to_id(LV2_EVENT_URI,
			    "http://lv2plug.in/ns/ext/midi#MidiEvent")) {
    
  }
  
  
  void run(uint32_t nframes) {
    
    LV2_Event_Buffer* inbuf = p<LV2_Event_Buffer>(m_midi_input);
    LV2_Event_Buffer* outbuf = p<LV2_Event_Buffer>(m_midi_output);
    lv2_event_buffer_reset(outbuf, inbuf->stamp_type, outbuf->data);
    LV2_Event_Iterator in, out;
    lv2_event_begin(&in, inbuf);
    lv2_event_begin(&out, outbuf);
    
    while (lv2_event_is_valid(&in)) {
      uint8_t* data;
      LV2_Event* ev = lv2_event_get(&in, &data);
      lv2_event_increment(&in);
      // XXX handle type 0 events
      if (ev->type == m_midi_type && ev->size == 3)
	lv2_event_write_event(&out, ev, data);
    }
    
  }
  
protected:
  
  uint32_t m_midi_type;
  
};


static unsigned _ = MIDIProc::register_class(m_uri);
