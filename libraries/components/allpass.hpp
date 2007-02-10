#ifndef ALLPASS_HPP
#define ALLPASS_HPP

#include <cstring>
#include <stdint.h>


namespace {


  class Allpass {
  public:
    
    Allpass(uint32_t rate, unsigned int delay);
    ~Allpass();
    
    void run(float* input, float* output, 
	     uint32_t nframes, unsigned int delay, float g);
    float run1(float input, unsigned int delay, float g);
    
  protected:
    
    float* m_data;
    unsigned int m_linesize;
    unsigned int m_counter;
    
  };
  
  
  Allpass::Allpass(uint32_t rate, unsigned int delay) 
    : m_data(new float[delay]),
      m_linesize(delay),
      m_counter(0) {
    
    std::memset(m_data, 0, m_linesize * sizeof(float));
    
  }
  
  
  Allpass::~Allpass() {
    delete [] m_data;
  }
  
  
  void Allpass::run(float* input, float* output, 
		    uint32_t nframes, unsigned delay, float g) {
    for (uint32_t i = 0; i < nframes; ++i, ++m_counter) {
      m_counter %= m_linesize;
      float o = m_data[m_counter] - g * input[i];
      m_data[(m_counter + delay) % m_linesize] = input[i] + g * o;
      output[i] = o;
    }
  }

  float Allpass::run1(float input, unsigned delay, float g) {
    float output = m_data[m_counter] - g * input;
    m_data[(m_counter + delay) % m_linesize] = input + g * output;
    ++m_counter;
    m_counter %= m_linesize;
    return output;
  }

  
}


#endif
