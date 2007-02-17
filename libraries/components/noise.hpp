#ifndef NOISE_HPP
#define NOISE_HPP


namespace {


  class Noise {
  public:
    Noise(unsigned int seed = 0);
    float run1();
  protected:
    unsigned int m_seed;
  };
  
  
  Noise::Noise(unsigned int seed) 
    : m_seed(seed) {

  }
  
  float Noise::run1() {
    m_seed = (m_seed * 196314165) + 907633515;
    return m_seed / float(INT_MAX) - 1;
  }


}

#endif
