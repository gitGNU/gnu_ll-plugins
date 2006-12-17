/****************************************************************************
    
    euphoria.cpp - An LV2 synth
    
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

#include "lv2instrument.hpp"
#include "pdosc.hpp"
#include "voicehandler.hpp"
#include "frequencytable.hpp"
#include "euphoria.peg"
#include "shaper.hpp"
#include "envelope.hpp"
#include "pdoscillator.hpp"
#include "markov.hpp"


class EuphoriaVoice {
public:
  
  enum State {
    ON,
    OFF,
  };
  
  EuphoriaVoice(uint32_t rate)
    : m_shaper(rate),
      m_shp_amount_env(rate),
      m_shp_amp_env(rate),
      m_pdosc(rate),
      m_pd_dist_env(rate),
      m_pd_amp_env(rate),
      m_mrk_01_env(rate),
      m_mrk_10_env(rate),
      m_mrk_amp_env(rate),
      m_state(OFF),
      m_inv_rate(1.0 / rate),
      m_freq(0),
      m_shp_phase(0),
      m_shp_phase2(0),
      m_mrk_phase(0),
      m_markov(2) {
    
    m_shaper.set_string("-1 -1 1 1");
    
  }
  
  
  void reset() {
    m_state = OFF;
    m_shp_phase = m_shp_phase2 = 0;
  }
  

  void on(unsigned char key, unsigned char velocity) {
    m_state = ON;
    m_freq = m_table[key];
    m_shp_amount_env.on();
    m_shp_amp_env.on();
    m_pd_dist_env.on();
    m_pd_amp_env.on();
    m_mrk_01_env.on();
    m_mrk_10_env.on();
    m_mrk_amp_env.on();
  }
  
  void off(unsigned char velocity) {
    m_state = OFF;
    m_shp_amount_env.off();
    m_shp_amp_env.off();
    m_pd_dist_env.off();
    m_pd_amp_env.off();
    m_mrk_01_env.off();
    m_mrk_10_env.off();
    m_mrk_amp_env.off();
  }

  void fast_off() {
    m_state = OFF;
    m_shp_amount_env.fast_off();
    m_shp_amp_env.fast_off();
    m_pd_dist_env.fast_off();
    m_pd_amp_env.fast_off();
    m_mrk_01_env.fast_off();
    m_mrk_10_env.fast_off();
    m_mrk_amp_env.fast_off();
  }

  void bend(int value) {

  }

  
  void run(float& left, float& right, float& shape, float& smoothness,
           float& attack, float& decay, float& release, float& mrk_freq) {
    
    // run shaper engine
    /*
    float shp_amount_env = m_shp_amount_env.run(attack, decay, 0.5, release);
    float shp_amp_env = m_shp_amp_env.run(attack, decay, 0.5, release);
    float left_input = 0.90 * sin(m_shp_phase) + 0.1 * sin(m_shp_phase2);
    float right_input = 0.90 * sin(m_shp_phase2) + 0.1 * sin(m_shp_phase);
    m_shp_phase += m_freq * 2 * M_PI * m_inv_rate;
    m_shp_phase2 += m_freq * 0.999 * 2 * M_PI * m_inv_rate;
    m_shp_phase = (m_shp_phase > 2 * M_PI ? 
                   m_shp_phase - 2 * M_PI : m_shp_phase);
    m_shp_phase2 = (m_shp_phase2 > 2 * M_PI ? 
                    m_shp_phase2 - 2 * M_PI : m_shp_phase2);
    left += shp_amp_env * m_shaper.run(shape * shp_amount_env * left_input, 
                                       m_freq + smoothness * 2000);
    right += shp_amp_env * m_shaper.run(shape * shp_amount_env * right_input, 
                                        m_freq + smoothness * 2000);
    */                                    
                                        
    // run Markov engine
    float mrk_01_env = m_mrk_01_env.run(attack, decay, 0.5, release);
    float mrk_10_env = m_mrk_10_env.run(attack, decay, 0.5, release);
    float mrk_amp_env = m_mrk_amp_env.run(attack, decay, 0.5, release);
    m_markov.set_probability(0, 1, mrk_01_env);
    m_markov.set_probability(0, 0, 1 - mrk_01_env);
    m_markov.set_probability(1, 0, mrk_10_env);
    m_markov.set_probability(1, 1, 1 - mrk_10_env);
    float mrk_sound = sin(m_mrk_phase);
    m_mrk_phase += mrk_freq * 2 * M_PI * m_inv_rate * (1 +m_markov.get_state());
    if (m_mrk_phase > 2 * M_PI * (1 + m_markov.get_state())) {
      m_markov.transition();
      m_mrk_phase -= 2 * M_PI;
    }
    mrk_sound *= mrk_amp_env * 0.5;
    left += mrk_sound;
    right += mrk_sound;
  }
  
  //protected:
  
  Shaper m_shaper;
  Envelope m_shp_amount_env;
  Envelope m_shp_amp_env;
  float m_shp_phase;
  float m_shp_phase2;

  PDOscillator m_pdosc;
  Envelope m_pd_dist_env;
  Envelope m_pd_amp_env;
  
  Markov m_markov;
  Envelope m_mrk_01_env;
  Envelope m_mrk_10_env;
  Envelope m_mrk_amp_env;
  float m_mrk_phase;
  
  State m_state;
  float m_inv_rate;
  float m_freq;
  
  static FrequencyTable m_table;
};


FrequencyTable EuphoriaVoice::m_table;


class Euphoria : public LV2Instrument {
public:
  
  Euphoria(uint32_t rate, const char*, const LV2_Host_Feature**) 
    : LV2Instrument(e_n_ports),
      m_handler(3, rate),
      m_trigger(0) {
    
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
    float* left = p(e_left);
    float* right = p(e_right);
    
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
      left[i] *= *p(e_gain);
      right[i] *= *p(e_gain);
    }
  }
  
  
  char* configure(const char* key, const char* value) {
    if (!strcmp(key, "shape"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_shaper.set_string(value);
    else if (!strcmp(key, "shp_amount_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_shp_amount_env.set_string(value);
    else if (!strcmp(key, "shp_amp_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_shp_amp_env.set_string(value);
    else if (!strcmp(key, "phase"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_pdosc.set_string(value);
    else if (!strcmp(key, "pd_dist_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_pd_dist_env.set_string(value);
    else if (!strcmp(key, "pd_amp_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_pd_amp_env.set_string(value);
    else if (!strcmp(key, "mrk_01_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_mrk_01_env.set_string(value);
    else if (!strcmp(key, "mrk_10_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_mrk_10_env.set_string(value);
    else if (!strcmp(key, "mrk_amp_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_mrk_amp_env.set_string(value);
    else
      return strdup("Unknown configure key");
    return 0;
  }
  
  
protected:
  
  VoiceHandler<EuphoriaVoice> m_handler;
  int m_trigger;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2_inst<Euphoria>("http://ll-plugins.nongnu.org/lv2/dev/euphoria/0.0.0", true);
}
