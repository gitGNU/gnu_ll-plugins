#ifndef HOUSEHOLDERFDN_HPP
#define HOUSEHOLDERFDN_HPP

#include <stdint.h>

#include "delayline.hpp"
#include "filter.hpp"


namespace {
  

  class HouseholderFDN {
  public:
    
    HouseholderFDN(uint32_t rate);
    
    void run(float* input, uint32_t nframes, float mix);
    
  protected:
    
    static const int N = 8;
    
    DelayLine m_d1;
    DelayLine m_d2;
    DelayLine m_d3;
    DelayLine m_d4;
    DelayLine m_d5;
    DelayLine m_d6;
    DelayLine m_d7;
    DelayLine m_d8;
    Filter m_f1;
    Filter m_f2;
    Filter m_f3;
    Filter m_f4;
    Filter m_f5;
        
  };
  
  
  HouseholderFDN::HouseholderFDN(uint32_t rate)
    : m_d1(2473),
      m_d2(2767),
      m_d3(3217),
      m_d4(3557),
      m_d5(3907),
      m_d6(4127),
      m_d7(2143),
      m_d8(1933),
      m_f1(rate, 5000),
      m_f2(rate, 3000),
      m_f3(rate, 5000),
      m_f4(rate, 5000),
      m_f5(rate, 5000) {
    
  }
  
  
  void HouseholderFDN::run(float* input, uint32_t nframes, float mix) {
    
    for (uint32_t i = 0; i < nframes; ++i) {
      
      // read from delay lines and compute output
      float u1 = m_d1.read();
      float u2 = m_d2.read();
      float u3 = m_d3.read();
      float u4 = m_d4.read();
      float u5 = m_d5.read();
      float u6 = m_d6.read();
      float u7 = m_d7.read();
      float u8 = m_d8.read();
      float output = u1 + u2 + u3 + u4 + u5 + u6 + u7 + u8;
      
      // write to delay lines
      float u = input[i] - output * 2.0 / N;
      float f = 0.9;
      m_d1.write(f * (u + u1));
      m_d2.write(f * (u + u2));
      m_d3.write(f * (u + u3));
      m_d4.write(f * (u + u4));
      m_d5.write(f * (u + u5));
      m_d6.write(f * (u + u6));
      m_d7.write(f * (u + u7));
      m_d8.write(f * (u + u8));
    
      input[i] = (1 - mix) * input[i] + mix * output;
      
    }
    
  }
  

}


#endif
