#ifndef REVERB_HPP
#define REVERB_HPP

#include <stdint.h>


class Reverb {
public:
  
  Reverb(uint32_t rate);
  ~Reverb();

  void run(float* left, float* right, uint32_t nframes);

private:
  

};


#endif
