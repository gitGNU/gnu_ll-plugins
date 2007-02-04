#ifndef DISTORTION_HPP
#define DISTORTION_HPP

#include <stdint.h>


class Distortion {
public:
  
  Distortion(uint32_t rate);
  ~Distortion();
  
  void run(float* left, float* right, uint32_t nframes, 
	   float drive, float set, float tone, float mix);
  
private:
  
  uint32_t m_rate;

};


#endif
