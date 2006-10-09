#ifndef PDOSC_HPP
#define PDOSC_HPP

#include <cmath>


class PDOsc {
public:
  
  inline PDOsc(uint32_t rate);
  inline void reset();
  inline float run(float freq, int shape, float amount);

protected:
  
  float m_phase;
  float m_inv_rate;
  
};


PDOsc::PDOsc(uint32_t rate)
  : m_phase(0.25),
    m_inv_rate(1.0 / rate) {
  
}
  
  
void PDOsc::reset() {
  m_phase = 0.25;
}

  
float PDOsc::run(float freq, int shape, float amount) {
  
  m_phase = fmodf(m_phase + freq * m_inv_rate, 1.0);
  float dphase;
  
  switch (shape) {
  case 0:
    if (m_phase < 0.5 * (1 - amount))
      dphase = m_phase / (1 - amount);
    else
      dphase = 0.5 + (m_phase - 0.5 * (1 - amount)) / (1 + amount);
    break;
    
  case 1:
    if (m_phase < 0.5 * (1 - amount))
      dphase = m_phase / (1 - amount);
    else if (m_phase < 0.5)
      dphase = 0.5;
    else if (m_phase < 0.5 + 0.5 * (1 - amount))
      dphase = 0.5 + (m_phase - 0.5) / (1 - amount);
    else
      dphase = 1.0;
    break;
    
  case 2:
    if (m_phase < amount)
      dphase = 0.25;
    else
      dphase = 0.25 + (m_phase - amount) / (1 - amount);
    break;
    
  case 3:
    if (m_phase < amount)
      dphase = 0.25;
    else
      dphase = 0.25 + 0.5 * (m_phase - amount) / (1 - amount);
    break;

  case 4:
    if (m_phase < amount)
      dphase = 0.25;
    else
      dphase = 0.25 + 2 * (m_phase - amount) / (1 - amount);
    break;

  case 5:
    if (m_phase < amount)
      dphase = 0.25;
    else
      dphase = 0.25 + 3 * (m_phase - amount) / (1 - amount);
    break;

  }
  
  return cos(dphase * 2 * M_PI);
}


#endif
