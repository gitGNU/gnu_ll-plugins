#ifndef REVERB_HPP
#define REVERB_HPP

#include <stdint.h>


namespace {


  class Allpass {
  public:
    
    Allpass(uint32_t rate, unsigned int delay);
    ~Allpass();
    
    void run(float* input, float* output, 
	     uint32_t nframes, unsigned int delay, float g);
    
  protected:
    
    float* m_data;
    unsigned int m_linesize;
    unsigned int m_counter;
    
  };
  
  
  class Reverb {
  public:
    
    Reverb(uint32_t rate);
    //~Reverb();
    
    void run(float* left, float* right, uint32_t nframes);
    
  private:
    
    Allpass m_lap1;
    Allpass m_lap2;
    Allpass m_lap3;
    Allpass m_lap4;
    Allpass m_rap1;
    Allpass m_rap2;
    Allpass m_rap3;
    Allpass m_rap4;
    
  };
  
  
  
  Allpass::Allpass(uint32_t rate, unsigned int delay) 
    : m_data(new float[delay]),
      m_linesize(delay),
      m_counter(0) {

  }
  
  
  Allpass::~Allpass() {
    delete [] m_data;
  }
  
  
  void Allpass::run(float* input, float* output, 
		    uint32_t nframes, unsigned delay, float g) {
    for (uint32_t i = 0; i < nframes; ++i, ++m_counter) {
      m_counter %= m_linesize;
      float d = m_data[(m_counter + m_linesize - delay) % m_linesize];
      m_data[m_counter] = input[i] - g * d;
      output[i] = input[i] * g;
      output[i] += d;
    }
  }
  


  Reverb::Reverb(uint32_t rate)
    : m_lap1(rate, 2048),
      m_lap2(rate, 2048),
      m_lap3(rate, 2048),
      m_lap4(rate, 2048),
      m_rap1(rate, 2048),
      m_rap2(rate, 2048),
      m_rap3(rate, 2048),
      m_rap4(rate, 2048) {
    

  }
  
  
  void Reverb::run(float* left, float* right, uint32_t nframes) {
    
    m_lap1.run(left, left, nframes, 256, 0.7);
    m_lap2.run(left, left, nframes, 729, 0.7);
    m_lap3.run(left, left, nframes, 625, 0.7);
    m_lap4.run(left, left, nframes, 343, 0.7);

    m_lap1.run(right, right, nframes, 256, 0.7);
    m_lap2.run(right, right, nframes, 729, 0.7);
    m_lap3.run(right, right, nframes, 625, 0.7);
    m_lap4.run(right, right, nframes, 343, 0.7);
    
  }
    
  
}


#endif
