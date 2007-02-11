#ifndef ONEPOLE_HPP
#define ONEPOLE_HPP

#include <stdint.h>


namespace {


  class OnePole {
  public:
    
    OnePole(uint32_t rate);
    
    void set_b(float b);
    
    float run1(float input);

  protected:
    
    float m_b;
    float m_s;

  };

  
  
  OnePole::OnePole(uint32_t rate) 
    : m_b(0),
      m_s(0) {

  }

  
  void OnePole::set_b(float b) {
    m_b = b;
  }
  
  
  float run1(float input) {
    return (m_s = (1 - b) * input + b * m_s);
  }
  
  
}


#endif
