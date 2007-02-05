#include <cmath>
#include <cstring>

#include "chorus.hpp"


LFO3::LFO3(uint32_t rate)
  : m_invrate(1.0 / rate),
    m_phase(0) {
}
  

void LFO3::run(float freq, float& p1, float& p2, float& p3) {
  p1 = std::sin(m_phase);
  p2 = std::sin(m_phase + 2 * M_PI / 3);
  p3 = std::sin(m_phase + 4 * M_PI / 3);
  m_phase += freq * 2 * M_PI * m_invrate;
  if (m_phase > 2000 * M_PI)
    m_phase -= 2000 * M_PI;
}

  
Chorus::Chorus(uint32_t rate)
  : m_rate(rate),
    m_maxfdelay(0.04 * m_rate),
    m_maxfdepth(0.02 * m_rate),
    m_linesize(m_maxfdelay + m_maxfdepth + 10),
    m_data_l(new float[m_linesize]),
    m_data_r(new float[m_linesize]),
    m_counter(0),
    m_lfo(m_rate) {
  
  std::memset(m_data_l, 0, sizeof(float) * m_linesize);
  std::memset(m_data_r, 0, sizeof(float) * m_linesize);
}


Chorus::~Chorus() {
  delete [] m_data_l;
  delete [] m_data_r;
}


void Chorus::run(float* left, float* right, uint32_t nframes, float toggle,
		 float freq, float depth, float delay, float mix) {
  
  if (toggle > 0) {
    
    int fdelay = delay * m_rate;
    fdelay = fdelay < 0 ? 0 : fdelay;
    fdelay = fdelay > m_maxfdelay ? m_maxfdelay : fdelay;
    int fdepth = depth * m_rate;
    fdepth = fdepth < 0 ? 0 : fdepth;
    fdepth = fdepth > m_maxfdepth ? m_maxfdepth : fdepth;
    fdepth /= 2;
    mix = mix < 0 ? 0 : mix;
    mix = mix > 1 ? 1 : mix;
    freq = freq < 0 ? 0 : freq;
    freq = freq > 10 ? 10 : freq;
    float d1, d2, d3;
    int fd1, fd2, fd3;
    float l, c, r;
    
    for (uint32_t i = 0; i < nframes; ++i, ++m_counter) {
      m_counter %= m_linesize;
      m_data_l[m_counter] = left[i];
      m_data_r[m_counter] = right[i];
      m_lfo.run(freq, d1, d2, d3);
      fd1 = fdelay + int(d1 * fdepth + fdepth);
      fd2 = fdelay + int(d2 * fdepth + fdepth);
      fd3 = fdelay + int(d3 * fdepth + fdepth);
      l = 0.66 * m_data_l[(m_counter + m_linesize - fd1) % m_linesize];
      r = 0.66 * m_data_r[(m_counter + m_linesize - fd2) % m_linesize];
      c = 0.33 * m_data_l[(m_counter + m_linesize - fd3) % m_linesize] + 
	0.33 * m_data_r[(m_counter + m_linesize - fd3) % m_linesize];
      left[i] = (1 - mix) * left[i] + mix * (l + c);
      right[i] = (1 - mix) * right[i] + mix * (r + c);
    }

  }
  
}



