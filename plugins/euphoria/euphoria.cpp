/****************************************************************************
    
    euphoria.cpp - An LV2 synth
    
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
#include <iostream>

#include <lv2synth.hpp>
#include <lv2_osc.h>

#include "pdosc.hpp"
#include "voicehandler.hpp"
#include "frequencytable.hpp"
#include "euphoria.peg"
#include "shaper.hpp"
#include "envelope.hpp"
#include "pdoscillator.hpp"
#include "markov.hpp"
#include "wsvoice.hpp"
#include "pdvoice.hpp"
#include "stereodistortion.hpp"
#include "chorus.hpp"
#include "echo.hpp"
#include "reverb.hpp"


using namespace LV2;


class EuphoriaVoice : public Voice {
public:
  
  enum State {
    ON,
    OFF
  };
  
  EuphoriaVoice(double rate)
    : m_wsvoice(rate),
      m_pdvoice(rate),
      m_state(OFF),
      m_inv_rate(1.0 / rate),
      m_freq(0),
      m_key(INVALID_KEY) {
    
  }
  
  
  void reset() {
    m_state = OFF;
    m_wsvoice.reset();
    m_pdvoice.reset();
  }
  

  void on(unsigned char key, unsigned char velocity) {
    m_state = ON;
    m_key = key;
    m_freq = m_table[key];
    m_wsvoice.on(key, velocity);
    m_pdvoice.on(key, velocity);
  }
  
  void off(unsigned char velocity) {
    m_state = OFF;
    m_wsvoice.off();
    m_pdvoice.off();
  }
  
  unsigned char get_key() const {
    return m_key;
  }

  void fast_off() {
    m_state = OFF;
    m_wsvoice.fast_off();
    m_pdvoice.fast_off();
  }

  void bend(int value) {

  }

  void render(uint32_t from, uint32_t to) {
    m_wsvoice.run(p(e_left + from), p(e_right + from), 1, *p(e_shape), 
		  *p(e_shape_smoothness), *p(e_shape_attack), *p(e_shape_decay),
		  0.5, *p(e_shape_release), m_freq);
  }
  
  //protected:
  
  WSVoice m_wsvoice;
  PDVoice m_pdvoice;
  
  State m_state;
  float m_inv_rate;
  float m_freq;
  unsigned char m_key;
  
  static FrequencyTable m_table;
};


FrequencyTable EuphoriaVoice::m_table;


class Euphoria : public Synth<EuphoriaVoice, Euphoria, MsgContext<true> > {
public:
  
  Euphoria(double rate) 
    : Synth<EuphoriaVoice, Euphoria, MsgContext<true> >(e_n_ports,e_midi_input),
      m_dist(rate),
      m_chorus(rate),
      m_echo(rate),
      m_reverb(rate) {
    add_voices(new EuphoriaVoice(rate), new EuphoriaVoice(rate),
	       new EuphoriaVoice(rate), new EuphoriaVoice(rate));
  }
  
  
  void post_process(uint32_t from, uint32_t to) {

    float& reverb_damp = *p(e_reverb_damping);
    float* left = p(e_left);
    float* right = p(e_right);
    
    // apply effects
    m_dist.run(left + from, right + from, to - from, 
	       *p(e_dist_switch), *p(e_dist_drive), 
	       *p(e_dist_set), *p(e_dist_tone), *p(e_dist_mix));
    m_chorus.run(left + from, right + from, to - from,
		 *p(e_chorus_switch), *p(e_chorus_freq), 
		 *p(e_chorus_depth) * 0.001, *p(e_chorus_delay) * 0.001, 
		 *p(e_chorus_mix));
    m_echo.run(left + from, right + from, to - from, 
	       *p(e_echo_switch), *p(e_echo_delay),
	       *p(e_echo_feedback), *p(e_echo_pan), *p(e_echo_mix));
    if (*p(e_reverb_switch) > 0)
      m_reverb.run(left + from, right + from, to - from, *p(e_reverb_time), 
		   *p(e_reverb_mix), reverb_damp*20000);
    
    // gain
    for (uint32_t i = from; i < to; ++i) {
      left[i] *= *p(e_gain);
      right[i] *= *p(e_gain);
    }
    
  }
  
  
  bool blocking_run(uint8_t* outputs_written) { 
    LV2_Event_Iterator iter;
    lv2_event_begin(&iter, p<LV2_Event_Buffer>(64));
    cerr<<"Message context woken up with these events:"<<endl;
    while (lv2_event_is_valid(&iter)) {
      uint8_t* data;
      LV2_Event* ev = lv2_event_get(&iter, &data);
      if (ev->type == 1) {
	cerr<<" MIDI:"<<hex;
	for (uint32_t i = 0; i < ev->size; ++i)
	  cerr<<" 0x"<<int(data[i]);
	cerr<<dec<<endl;
      }
      else if (ev->type == 2) {
	const LV2_OSC_Event* osc = reinterpret_cast<LV2_OSC_Event*>(data);
	const char* types = lv2_osc_get_types(osc);
	const char* path = lv2_osc_get_path(osc);
	cerr<<"  OSC: \""<<path<<"\" "<<types;
	for (uint32_t i = 0; i < strlen(types); ++i) {
	  switch (types[i]) {
	  case 'i': cerr<<" "<<lv2_osc_get_argument(osc, i)->i; break;
	  case 'f': cerr<<" "<<lv2_osc_get_argument(osc, i)->f; break;
	  case 's': cerr<<" \""<<&lv2_osc_get_argument(osc, i)->s<<"\""; break;
	  case 'b': cerr<<" <BLOB>"; break;
	  default: cerr<<" <\?\?\?>";
	  }
	}
	cerr<<endl;
	
	if (!strcmp(types, "ss")) {
	  free(command(&lv2_osc_get_argument(osc, 0)->s,
		       &lv2_osc_get_argument(osc, 1)->s));
	}
	
      }
      lv2_event_increment(&iter);
    }
    return false;
  }


  
  // XXX this should be changed to message_run()
  char* command(const char* key, const char* value) {
    
    if (!strcmp(key, "shape"))
      for (unsigned j = 0; j < m_voices.size(); ++j)
        m_voices[j]->m_wsvoice.set_shape(value);
    else if (!strcmp(key, "shp_amount_env"))
      for (unsigned j = 0; j < m_voices.size(); ++j)
        m_voices[j]->m_wsvoice.set_amount_env(value);
    else if (!strcmp(key, "shp_amp_env"))
      for (unsigned j = 0; j < m_voices.size(); ++j)
        m_voices[j]->m_wsvoice.set_gain_env(value);
    else if (!strcmp(key, "phase"))
      for (unsigned j = 0; j < m_voices.size(); ++j)
        m_voices[j]->m_pdvoice.set_phase(value);
    else if (!strcmp(key, "pd_dist_env"))
      for (unsigned j = 0; j < m_voices.size(); ++j)
        m_voices[j]->m_pdvoice.set_dist_env(value);
    else if (!strcmp(key, "pd_amp_env"))
      for (unsigned j = 0; j < m_voices.size(); ++j)
        m_voices[j]->m_pdvoice.set_gain_env(value);
    else
      return strdup("Unknown configure key");
    
    return 0;
  }
  
  
protected:
  
  StereoDistortion m_dist;
  Chorus m_chorus;
  Echo m_echo;
  Reverb m_reverb;
  
};


static unsigned _ = Euphoria::register_class(e_uri);
