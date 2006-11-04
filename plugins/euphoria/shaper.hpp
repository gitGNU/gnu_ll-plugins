#ifndef SHAPER_HPP
#define SHAPER_HPP

#include <string>
#include <vector>


class Shaper {
public:
  
  Shaper(unsigned int rate);
  
  float run(float input, float max_freq);
  
  bool set_string(const std::string& str);
  
protected:
  
  static const int WAVETABLE_SIZE = 65536;
  
  static double function(double x, void* params);
  
  float* m_tables[10];
  unsigned m_rate;
  
};


#endif
