#ifndef FILTER_HPP
#define FILTER_HPP

#include <cmath>
#include <stdint.h>


namespace {


  class Filter {
  public:
    
    Filter(uint32_t rate);
    
    void set_cutoff(float cutoff);
    
    float run1(float input);

  protected:
    
    uint32_t m_rate;
    float m_p;
    float m_x;

  };
  
  
  Filter::Filter(uint32_t rate)
    : m_rate(rate),
      m_p(0),
      m_x(0) {

  }
  
  
  void Filter::set_cutoff(float cutoff) {
    m_p = std::pow(1 - 2 * cutoff / m_rate, 2);
  }
  
  float Filter::run1(float input) {
    return (m_x = (1 - m_p) * input + m_p * m_x);
  }


}


#endif
