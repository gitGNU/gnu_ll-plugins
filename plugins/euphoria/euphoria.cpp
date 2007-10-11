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

#include <lv2plugin.hpp>

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
#include "distortion.hpp"
#include "chorus.hpp"
#include "echo.hpp"
#include "reverb.hpp"


class EuphoriaVoice {
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
      m_freq(0) {
  }
  
  
  void reset() {
    m_state = OFF;
    m_wsvoice.reset();
    m_pdvoice.reset();
  }
  

  void on(unsigned char key, unsigned char velocity) {
    m_state = ON;
    m_freq = m_table[key];
    m_wsvoice.on(key, velocity);
    m_pdvoice.on(key, velocity);
  }
  
  void off(unsigned char velocity) {
    m_state = OFF;
    m_wsvoice.off();
    m_pdvoice.off();
  }

  void fast_off() {
    m_state = OFF;
    m_wsvoice.fast_off();
    m_pdvoice.fast_off();
  }

  void bend(int value) {

  }

  
  void run(float& left, float& right, float& shape, float& smoothness,
           float& attack, float& decay, float& release, float& mrk_freq) {
    
    m_wsvoice.run(&left, &right, 1, shape, smoothness, attack, decay, 0.5,
		  release, m_freq);
    m_pdvoice.run(&left, &right, 1);
  }
  
  //protected:
  
  WSVoice m_wsvoice;
  PDVoice m_pdvoice;
  
  State m_state;
  float m_inv_rate;
  float m_freq;
  
  static FrequencyTable m_table;
};


FrequencyTable EuphoriaVoice::m_table;


class Euphoria : public LV2::Plugin<Euphoria, LV2::CommandExt, true> {
public:
  
  Euphoria(double rate, const char*, const LV2_Feature* const* f) 
    : LV2::Plugin<Euphoria, LV2::CommandExt, true>(e_n_ports),
      m_handler(3, rate),
      m_trigger(0),
      m_dist(rate),
      m_chorus(rate),
      m_echo(rate),
      m_reverb(rate) {
    
  }
  
  
  void activate() {
    m_handler.set_voices(m_handler.get_voices().size());
    m_trigger = 0;
  }

  
  void run(uint32_t nframes) {
    
    LV2_MIDI* midi = p<LV2_MIDI>(e_midi_input);
    float& shape = *p(e_shape);
    float& shape_smoothness = *p(e_shape_smoothness);
    float& attack = *p(e_shape_attack);
    float& decay = *p(e_shape_decay);
    float& release = *p(e_shape_release);
    float& reverb_damp = *p(e_reverb_damping);
    float* left = p(e_left);
    float* right = p(e_right);
    
    // render voices
    m_handler.set_midi_port(midi);
    for (uint32_t i = 0; i < nframes; ++i) {
      m_handler.run(i);
      left[i] = 0;
      right[i] = 0;
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j) {
        m_handler.get_voices()[j].voice->
          run(left[i], right[i], shape, shape_smoothness, 
              attack, decay, release, *p(e_mrk_max_freq));
      }
    }
    
    // apply effects
    m_dist.run(left, right, nframes, *p(e_dist_switch), *p(e_dist_drive), 
	       *p(e_dist_set), *p(e_dist_tone), *p(e_dist_mix));
    m_chorus.run(left, right, nframes, *p(e_chorus_switch), *p(e_chorus_freq),
		 *p(e_chorus_depth) * 0.001, *p(e_chorus_delay) * 0.001, 
		 *p(e_chorus_mix));
    m_echo.run(left, right, nframes, *p(e_echo_switch), *p(e_echo_delay),
	       *p(e_echo_feedback), *p(e_echo_pan), *p(e_echo_mix));
    if (*p(e_reverb_switch) > 0)
      m_reverb.run(left, right, nframes, *p(e_reverb_time), 
		   *p(e_reverb_mix), reverb_damp*20000);
    
    // gain
    for (uint32_t i = 0; i < nframes; ++i) {
      left[i] *= *p(e_gain);
      right[i] *= *p(e_gain);
    }
  }
  
  
  char* command(uint32_t argc, const char* const* argv) {
    
    if (argc != 2)
      return strdup("Unknown command");
    
    const char* key = argv[0];
    const char* value = argv[1];
    
    if (!strcmp(key, "shape"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_wsvoice.set_shape(value);
    else if (!strcmp(key, "shp_amount_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_wsvoice.set_amount_env(value);
    else if (!strcmp(key, "shp_amp_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_wsvoice.set_gain_env(value);
    else if (!strcmp(key, "phase"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_pdvoice.set_phase(value);
    else if (!strcmp(key, "pd_dist_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_pdvoice.set_dist_env(value);
    else if (!strcmp(key, "pd_amp_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_pdvoice.set_gain_env(value);
    else
      return strdup("Unknown configure key");
    
    return 0;
  }
  
  
protected:
  
  VoiceHandler<EuphoriaVoice> m_handler;
  int m_trigger;
  
  StereoDistortion m_dist;
  Chorus m_chorus;
  Echo m_echo;
  Reverb m_reverb;
  
};


static unsigned _ = Euphoria::register_class(e_uri);
