#ifndef COMB_HPP
#define COMB_HPP

#include <cstring>
#include <stdint.h>


namespace {


  class Comb {
  public:
    
    Comb(uint32_t rate, unsigned int delay);
    ~Comb();
    
    void run(float* input, float* output, 
	     uint32_t nframes, unsigned int delay, float g);
    float run1(float input, unsigned int delay, float g);
    
  protected:
    
    float* m_data;
    unsigned int m_linesize;
    unsigned int m_counter;
    
  };

  
  
  Comb::Comb(uint32_t rate, unsigned int delay) 
    : m_data(new float[delay]),
      m_linesize(delay),
      m_counter(0) {
    
    std::memset(m_data, 0, m_linesize * sizeof(float));
    
  }
  
  
  Comb::~Comb() {
    delete [] m_data;
  }
  
  
  void Comb::run(float* input, float* output, 
		    uint32_t nframes, unsigned delay, float g) {
    for (uint32_t i = 0; i < nframes; ++i, ++m_counter) {
      m_counter %= m_linesize;
      output[i] = input[i] + g * m_data[m_counter];
      m_data[(m_counter+delay) % m_linesize] = input[i];
    }
  }

  float Comb::run1(float input, unsigned delay, float g) {
    m_counter %= m_linesize;
    float tmp = input + g * m_data[m_counter];
    m_data[(m_counter + delay) % m_linesize] = input;
    ++m_counter;
    return tmp;
  }

 
}


#endif
