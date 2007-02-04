#ifndef ECHO_HPP
#define ECHO_HPP

#include <stdint.h>


class Echo {
public:
  
  Echo(uint32_t rate);
  ~Echo();

  void run(float* left, float* right, uint32_t nframes);

private:
  

};


#endif
