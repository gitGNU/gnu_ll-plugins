#include <cstring>

#include "lv2instrument.hpp"
#include "pdosc.hpp"
#include "voicehandler.hpp"
#include "frequencytable.hpp"
#include "euphoria.peg"
#include "shaper.hpp"
#include "envelope.hpp"


class EuphoriaVoice {
public:
  
  enum State {
    ON,
    OFF,
  };
  
  EuphoriaVoice(uint32_t rate)
    : m_shaper(rate),
      m_shape_env(rate),
      m_state(OFF),
      m_inv_rate(1.0 / rate),
      m_freq(0),
      m_phase(0) {

    m_shaper.set_string("-1 -1 0 1 0.5 -1 1 1");
    
  }
  
  
  void reset() {
    m_state = OFF;
    m_phase = 0;
  }
  

  void on(unsigned char key, unsigned char velocity) {
    m_state = ON;
    m_freq = 0.125 * m_table[key];
    m_shape_env.on();
  }
  
  void off(unsigned char velocity) {
    m_state = OFF;
    m_shape_env.off();
  }

  void fast_off() {
    m_state = OFF;
    m_shape_env.fast_off();
  }

  void bend(int value) {

  }

  
  void run(float& left, float& right, float& shape, float& smoothness,
           float& attack, float& decay, float& release) {
    
    float shape_env = m_shape_env.run(attack, decay, 0.5, release);
    left += 0.25 * m_shaper.run(shape * shape_env * sin(m_phase), 
                                m_freq + smoothness * 7000);
    right = left;
    m_phase += m_freq * 2 * M_PI * m_inv_rate;
    m_phase = (m_phase > 1 ? m_phase - 1 : m_phase);
  }
  
  //protected:
  
  Shaper m_shaper;
  Envelope m_shape_env;
  State m_state;
  float m_inv_rate;
  float m_freq;
  float m_phase;
  
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
              attack, decay, release);
      }

    }
  }
  
  
  char* configure(const char* key, const char* value) {
    if (!strcmp(key, "shape"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_shaper.set_string(value);
    else if (!strcmp(key, "shape_env"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_shape_env.set_string(value);
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
