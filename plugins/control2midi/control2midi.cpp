/****************************************************************************
    
    control2midi.cpp - A plugin that converts control input to MIDI events
    
    Copyright (C) 2006-2007  Lars Luthman <lars.luthman@gmail.com>
    
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

#include <lv2plugin.hpp>
#include <lv2_event_helpers.h>


/** This is the class that contains all the code and data for the Sineshaper
    synth plugin. */
class Control2MIDI : public LV2::Plugin<Control2MIDI, LV2::UriMapExt<true> > {
public:
  
  Control2MIDI(double) 
    : LV2::Plugin<Control2MIDI, LV2::UriMapExt<true> >(5),
      m_last_value(0),
      m_last_cc(0),
      m_midi_type(uri_to_id(LV2_EVENT_URI, 
			    "http://lv2plug.in/ns/ext/midi#MidiEvent")) { 
    
  }
  
  void run(uint32_t sample_count) {
    
    float value = *p(0);
    float min = *p(1);
    float max = *p(2);
    float cc = *p(3);
    LV2_Event_Buffer* midi = p<LV2_Event_Buffer>(4);
    lv2_event_buffer_reset(midi, midi->stamp_type, midi->data);
    LV2_Event_Iterator iter;
    lv2_event_begin(&iter, midi);
    
    unsigned char ccc = (unsigned char)cc;
    
    if (max - min < 0.001)
      max = min + 0.001;
    if (value < min)
      value = min;
    else if (value > max)
      value = max;

    unsigned char cvalue = (unsigned char)((value - min) * 127 / (max - min));
    if (ccc != m_last_cc || cvalue != m_last_value) {
      unsigned char bytes[] = { 0xB0, ccc, cvalue };
      lv2_event_write(&iter, 0, 0, m_midi_type, 3, bytes);
      m_last_cc = ccc;
      m_last_value = cvalue;
    }
    
  }
  
protected:
  
  unsigned char m_last_value;
  unsigned char m_last_cc;
  uint32_t m_midi_type;
  
};


static unsigned _ = Control2MIDI::register_class("http://ll-plugins.nongnu.org/lv2/dev/control2midi/0.0.0");
