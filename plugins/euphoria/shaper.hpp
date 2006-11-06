#ifndef SHAPER_HPP
#define SHAPER_HPP

#include <string>
#include <vector>


class Shaper {
public:
  
  Shaper(unsigned int rate);
  ~Shaper();
  
  float run(float input, float max_freq);
  
  bool set_string(const std::string& str);
  
protected:
  
  static const int WAVETABLE_SIZE = 65536;
  
  static double function(double x, void* params);
  
  typedef float *float_ptr;
  float_ptr m_tables[10];
  unsigned m_rate;
  
};


#endif
