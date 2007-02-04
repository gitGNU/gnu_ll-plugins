#ifndef WSVOICE_HPP
#define WSVOICE_HPP

#include <string>

#include "shaper.hpp"
#include "envelope.hpp"


class WSVoice {
public:
  
  WSVoice(uint32_t rate);
  ~WSVoice();
  
  void run(float* left, float* right, uint32_t nframes, float shape, 
	   float smoothness, float attack, float decay, float sustain, 
	   float release, float freq);
  
  void on(unsigned char key, unsigned char velocity);
  
  void off();
  
  void fast_off();
  
  void reset();
  
  void set_shape(const std::string& str);
  
  void set_amount_env(const std::string& str);
  
  void set_gain_env(const std::string& str);
  
private:
  
  Shaper m_shaper;
  Envelope m_shp_amount_env;
  Envelope m_shp_amp_env;
  float m_shp_phase;
  float m_shp_phase2;
  float m_inv_rate;
  
};


#endif
