#include "echo.hpp"

  
Echo::Echo(uint32_t rate) 
  : m_rate(rate),
    m_linesize(m_rate * 3),
    m_data_l(new float[m_linesize]),
    m_data_r(new float[m_linesize]),
    m_counter(0),
    m_clean(0) {

}


Echo::~Echo() {
  delete [] m_data_l;
  delete [] m_data_r;
}


void Echo::run(float* left, float* right, uint32_t nframes, float toggle,
	       float delay, float feedback, float pan, float mix) {
  
  unsigned int fdelay = unsigned(delay * m_rate);
  fdelay = fdelay > m_linesize ? m_linesize : fdelay;
  
  for (uint32_t i = 0; i < nframes; ++i, ++m_counter) {
    m_counter %= m_linesize;
    float l = feedback * m_data_r[(m_counter - fdelay) % m_linesize];
    float r = feedback * m_data_l[(m_counter - fdelay) % m_linesize];
    m_data_l[m_counter] = 0.5 * left[i] + l;
    m_data_r[m_counter] = right[i] + r;
    left[i] += mix * l;
    right[i] += mix * r;
  }
  
}
