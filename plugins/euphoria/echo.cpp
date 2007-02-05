#include <cstring>

#include "echo.hpp"

  
Echo::Echo(uint32_t rate) 
  : m_rate(rate),
    m_linesize(m_rate * 3),
    m_data_l(new float[m_linesize]),
    m_data_r(new float[m_linesize]),
    m_counter(0),
    m_active(false) {

  std::memset(m_data_l, 0, sizeof(float) * m_linesize);
  std::memset(m_data_r, 0, sizeof(float) * m_linesize);
}


Echo::~Echo() {
  delete [] m_data_l;
  delete [] m_data_r;
}


void Echo::run(float* left, float* right, uint32_t nframes, float toggle,
	       float delay, float feedback, float pan, float mix) {
  
  int offset = m_linesize - int(delay * m_rate);
  offset = offset < 1 ? 1 : offset;
  
  m_active = toggle > 0;
  
  if (m_active && toggle <= 0) {
    std::memset(m_data_l, 0, sizeof(float) * m_linesize);
    std::memset(m_data_r, 0, sizeof(float) * m_linesize);
    m_active = false;
  }
  
  if (m_active) {
    for (uint32_t i = 0; i < nframes; ++i, ++m_counter) {
      m_counter %= m_linesize;
      float l = feedback * m_data_r[(m_counter + offset) % m_linesize];
      float r = feedback * m_data_l[(m_counter + offset) % m_linesize];
      m_data_l[m_counter] = 0.5 * left[i] + l;
      m_data_r[m_counter] = right[i] + r;
      left[i] += mix * l;
      right[i] += mix * r;
    }
  }
  
}
