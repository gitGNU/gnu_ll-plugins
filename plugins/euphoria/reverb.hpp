#ifndef REVERB_HPP
#define REVERB_HPP

#include "householderfdn.hpp"


namespace {

  
  class Reverb {
  public:
    
    Reverb(uint32_t rate);
    
    void run(float* left, float* right, uint32_t nframes, 
	     float time, float mix, float damp);
    
  private:
    
    HouseholderFDN m_hhfdn;
    uint32_t m_rate;
    
  };


  Reverb::Reverb(uint32_t rate)
    : m_hhfdn(rate),
      m_rate(rate) {

  }
  
  
  void Reverb::run(float* left, float* right, uint32_t nframes, 
		   float time, float mix, float damp) {
    m_hhfdn.run(left, right, nframes, time, mix, damp);
  }
  
}


#endif
