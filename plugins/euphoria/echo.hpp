#ifndef ECHO_HPP
#define ECHO_HPP

#include <stdint.h>


class Echo {
public:
  
  Echo(uint32_t rate);
  ~Echo();

  void run(float* left, float* right, uint32_t nframes, float toggle,
	   float delay, float feedback, float pan, float mix);

private:
  
  
  uint32_t m_rate;
  uint32_t m_linesize;
  float* m_data_l;
  float* m_data_r;
  uint32_t m_counter;
  bool m_active;
  
};


#endif
