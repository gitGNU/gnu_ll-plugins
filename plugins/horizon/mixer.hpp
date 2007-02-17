#ifndef MIXER_HPP
#define MIXER_HPP

#include "sample.hpp"


class Mixer {
public:
  
  Mixer();
  
  void set_buffers(float* left, float* right);
  
  void run(uint32_t nframes);

protected:
  
  Sample m_sample;
  uint32_t m_pos;
  float* m_left;
  float* m_right;
};


#endif
