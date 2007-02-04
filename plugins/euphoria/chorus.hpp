#ifndef CHORUS_HPP
#define CHORUS_HPP

#include <stdint.h>


class Chorus {
public:
  
  Chorus(uint32_t rate);
  ~Chorus();

  void run(float* left, float* right, uint32_t nframes);

private:
  

};


#endif
