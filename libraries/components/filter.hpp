#ifndef FILTER_HPP
#define FILTER_HPP

#include <cmath>
#include <stdint.h>


namespace {


  class Filter {
  public:
    
    Filter(uint32_t rate, float cutoff);
    
    float run1(float input);

  protected:
    
    float m_p;
    float m_x;

  };
  
  
  Filter::Filter(uint32_t rate, float cutoff)
    : m_p(std::pow(1 - 2 * cutoff / rate, 2)),
      m_x(0) {

  }

  
  float Filter::run1(float input) {
    return (m_x = (1 - m_p) * input + m_p * m_x);
  }


}


#endif
