#ifndef CHORUS_HPP
#define CHORUS_HPP

#include <stdint.h>


class LFO3 {
public:

  LFO3(uint32_t rate);
  
  void run(float freq, float& p1, float& p2, float& p3);

protected:
  
  float m_invrate;
  float m_phase;
};




class Chorus {
public:
  
  Chorus(uint32_t rate);
  ~Chorus();

  void run(float* left, float* right, uint32_t nframes, float toggle,
	   float freq, float depth, float delay, float mix);

private:
  
  uint32_t m_rate;
  
  uint32_t m_maxfdelay;
  uint32_t m_maxfdepth;
  uint32_t m_linesize;
  
  float* m_data_l;
  float* m_data_r;
  uint32_t m_counter;
  
  LFO3 m_lfo;
};


#endif
