#ifndef REVERB_HPP
#define REVERB_HPP

#include "householderfdn.hpp"


namespace {

  
  class Reverb {
  public:
    
    Reverb(uint32_t rate);
    
    void run(float* left, float* right, uint32_t nframes, float mix);
    
  private:
    
    HouseholderFDN m_hhfdn1;
    HouseholderFDN m_hhfdn2;
    
  };


  Reverb::Reverb(uint32_t rate)
    : m_hhfdn1(rate),
      m_hhfdn2(rate) {

  }
  
  
  void Reverb::run(float* left, float* right, uint32_t nframes, float mix) {
    m_hhfdn1.run(left, nframes, mix);
    m_hhfdn2.run(right, nframes, mix);
  }
  
}


#endif
