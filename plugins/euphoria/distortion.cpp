#include <cmath>

#include "distortion.hpp"

  
Distortion::Distortion(uint32_t rate) 
  : m_rate(rate) {

}


Distortion::~Distortion() {

}


void Distortion::run(float* left, float* right, uint32_t nframes, float drive,
		     float set, float tone, float mix) {

}

