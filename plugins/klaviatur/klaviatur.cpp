/****************************************************************************
    
    klaviatur.cpp - The plugin part for a MIDI keyboard plugin
    
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

#include "lv2plugin.hpp"
#include "klaviatur.peg"
#include "lv2-midifunctions.h"


class Klaviatur : public LV2::Plugin<Klaviatur> {
public:

  Klaviatur(double rate, const char* bundle, 
	    const LV2_Feature* const* f)
    : LV2::Plugin<Klaviatur>(k_n_ports) {
    
  }
  
  
  void run(uint32_t nframes) {

    LV2_MIDIState in = { p<LV2_MIDI>(k_midi_input), nframes, 0 };
    LV2_MIDIState out = { p<LV2_MIDI>(k_midi_output), nframes, 0 };
    
    out.midi->size = 0;
    out.midi->event_count = 0;
    
    double event_time;
    uint32_t event_size;
    unsigned char* event;

    while (lv2midi_get_event(&in, &event_time, &event_size, &event) < nframes){
      lv2midi_put_event(&out, event_time, event_size, event);
      lv2midi_step(&in);
    }
    
  }
  
};


static unsigned _ = Klaviatur::register_class(k_uri);
