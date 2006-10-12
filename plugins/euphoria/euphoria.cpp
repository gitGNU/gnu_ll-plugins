#include "lv2instrument.hpp"
#include "pdosc.hpp"
#include "voicehandler.hpp"
#include "frequencytable.hpp"


class EuphoriaVoice {
public:
  
  enum State {
    ON,
    OFF,
  };
  
  EuphoriaVoice(uint32_t rate)
    : m_osc1(rate),
      m_osc2(rate),
      m_osc3(rate),
      m_osc4(rate),
      m_osc5(rate),
      m_amount(0.99),
      m_state(OFF),
      m_gain(0),
      m_inv_rate(1.0 / rate) {
    
  }
  
  
  void reset() {
    m_osc1.reset();
    m_osc2.reset();
    m_osc3.reset();
    m_osc4.reset();
    m_osc5.reset();
    m_amount = 0;
    m_state = OFF;
    m_gain = 0;
  }
  

  void on(unsigned char key, unsigned char velocity) {
    m_amount = 0.09 + 0.90 * (velocity / 127.0);
    m_state = ON;
    m_freq = 0.5 * m_table[key];
  }
  
  void off(unsigned char velocity) {
    m_state = OFF;
  }

  void fast_off() {
    m_amount = 0;
    m_state = OFF;
  }

  void bend(int value) {

  }

  
  void run(float& left, float& right, int shape, float timbre, float spread, 
           float gmod, float fmod, float amod, float decay, float release) {
    
    float ileft, iright;
    
    float f = m_freq * (1 + fmod * pow(m_amount, 10));
    float amount = 0.5 + (m_amount - 0.5) * amod;
    //amount = (amount < 0 ? 0 : amount);
    //amount = (amount > 0.999 ? 0.999 : amount);
    float main_osc = 0.5 * m_osc1.run(f, shape, amount);
    ileft = iright = main_osc;
    ileft += 0.15 * m_osc2.run(f * (1 + 1.05684 * spread), shape, amount);
    ileft += 0.15 * m_osc3.run(f * (1 - 0.98273 * spread), shape, amount);
    iright += 0.10 * m_osc4.run(f * (1 + 2.10231 * spread), shape, amount);
    iright += 0.10 * m_osc5.run(f * (1 - 1.99823 * spread), shape, amount);
    
    float gain = 0.2 * (0.5 * (1 - gmod) + gmod * m_amount);
    ileft *= gain * m_gain;
    iright *= gain * m_gain;
    
    left += ileft;
    right += iright;
    
    m_amount *= decay;
    //m_amount *= 0.99999;
    //float release = 0.5;
    if (m_state == ON && m_gain < 1)
      m_gain += 1000 * m_inv_rate;
    else if (m_state == OFF && m_gain > 0) {
      m_gain -= 1.0 / release * m_inv_rate;
      m_gain = (m_gain < 0 ? 0 : m_gain);
    }
  }
  
protected:
  
  PDOsc m_osc1;
  PDOsc m_osc2;
  PDOsc m_osc3;
  PDOsc m_osc4;
  PDOsc m_osc5;
  
  float m_amount;
  State m_state;
  float m_gain;
  float m_inv_rate;
  float m_freq;
  
  static FrequencyTable m_table;
};


FrequencyTable EuphoriaVoice::m_table;


class Euphoria : public LV2Instrument {
public:
  
  Euphoria(uint32_t rate, const char*, const LV2_Host_Feature**) 
    : LV2Instrument(12),
      m_handler(3, rate),
      m_trigger(0) {
    
  }
  
  
  void activate() {
    m_handler.set_voices(m_handler.get_voices().size());
    m_trigger = 0;
  }

  
  void run(uint32_t nframes) {
    
    LV2_MIDI* midi = p<LV2_MIDI>(0);
    int shape = int(*p<float>(1));
    int timbre = int(*p<float>(2));
    float& spread = *p<float>(3);
    float& width = *p<float>(4);
    float& gmod = *p<float>(5);
    float& fmod = *p<float>(6);
    float& amod = *p<float>(7);
    float decay = *p<float>(8);
    decay = (decay < 0.999 ? 0.999 : decay);
    decay = (decay > 0.99999 ? 0.99999 : decay);
    float release = *p<float>(9);
    release = (release < 0.01 ? 0.01 : release);
    release = (release > 10 ? 10 : release);
    float* left = p<float>(10);
    float* right = p<float>(11);
    
    m_handler.set_midi_port(midi);
    for (uint32_t i = 0; i < nframes; ++i) {
      m_handler.run(i);
      left[i] = 0;
      right[i] = 0;
      for (unsigned j = 0; j < m_handler.get_voices().size(); ++j) {
        m_handler.get_voices()[j].voice->
          run(left[i], right[i], shape, timbre, spread, 
              gmod, fmod, amod, decay, release);
      }

      float tmp = left[i];
      left[i] += (1 - width) * right[i] + width * left[i];
      right[i] += (1 - width) * left[i] + width * right[i];

    }
      
  }
  

protected:
  
  VoiceHandler<EuphoriaVoice> m_handler;
  int m_trigger;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2_inst<Euphoria>("http://ll-plugins.nongnu.org/lv2/dev/euphoria/0.0.0", true);
}
