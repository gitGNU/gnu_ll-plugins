#include <cstring>

#include "lv2instrument.hpp"
#include "pdosc.hpp"
#include "voicehandler.hpp"
#include "frequencytable.hpp"
#include "euphoria.peg"
#include "shaper.hpp"


class EuphoriaVoice {
public:
  
  enum State {
    ON,
    OFF,
  };
  
  EuphoriaVoice(uint32_t rate)
    : m_shaper(rate),
      m_state(OFF),
      m_inv_rate(1.0 / rate) {

    m_shaper.set_string("-1 -1 0 1 0.5 -1 1 1");
    
  }
  
  
  void reset() {
    m_state = OFF;
  }
  

  void on(unsigned char key, unsigned char velocity) {
    m_state = ON;
    m_freq = 0.5 * m_table[key];
  }
  
  void off(unsigned char velocity) {
    m_state = OFF;
  }

  void fast_off() {
    m_state = OFF;
  }

  void bend(int value) {

  }

  
  void run(float& left, float& right, int shape) {
    
    if (m_state != OFF) {
      left += 0.25 * m_shaper.run(sin(m_phase), m_freq);
      right = left;
      m_phase += m_freq * 2 * M_PI * m_inv_rate;
    }
    
  }
  
  //protected:
  
  Shaper m_shaper;
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
    int shape = int(*p<float>(e_shape));
    float* left = p<float>(e_left);
    float* right = p<float>(e_right);
    
    m_handler.set_midi_port(midi);
    for (uint32_t i = 0; i < nframes; ++i) {
      m_handler.run(i);
      left[i] = 0;
      right[i] = 0;
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j) {
        m_handler.get_voices()[j].voice->
          run(left[i], right[i], shape);
      }

    }
  }
  
  
  char* configure(const char* key, const char* value) {
    if (!strcmp(key, "shape"))
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j)
        m_handler.get_voices()[j].voice->m_shaper.set_string(value);
  }
  

protected:
  
  VoiceHandler<EuphoriaVoice> m_handler;
  int m_trigger;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2_inst<Euphoria>("http://ll-plugins.nongnu.org/lv2/dev/euphoria/0.0.0", true);
}
