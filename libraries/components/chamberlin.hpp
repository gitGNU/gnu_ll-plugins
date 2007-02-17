#ifndef CHAMBERLIN_HPP
#define CHAMBERLIN_HPP

#include <cmath>
#include <stdint.h>


namespace {


  class Chamberlin {
  public:
    Chamberlin(uint32_t rate);
    void run(float* input, float* output, float cutoff, 
	     uint32_t nframes, float q,
	     float low_gain, float high_gain, float band_gain);
  protected:
    float m_invrate;
    float m_low;
    float m_band;
  };

  
  Chamberlin::Chamberlin(uint32_t rate)
    : m_invrate(1.0f / rate),
      m_low(0),
      m_band(0) {

  }


  void Chamberlin::run(float* input, float* output, uint32_t nframes,
		       float cutoff, float q,
		       float low_gain, float high_gain, float band_gain) {
    float f = 2 * std::sin(M_PI * cutoff * m_invrate);
    for (uint32_t i = 0; i < nframes; ++i) {
      m_low = m_low + f * m_band;
      float high = q * input[i] - m_low - q * m_band;
      m_band = f * high + m_band;
      output[i] = m_low * low_gain + m_high * high_gain + m_band + band_gain;
    }
  }


}


#endif
