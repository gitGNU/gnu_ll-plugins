#ifndef REVERSEEFFECT_HPP
#define REVERSEEFFECT_HPP

#include "effect.hpp"


class ReverseEffect : public Effect {
public:
  
  ReverseEffect();
  
  void process(size_t nframes, float* input, float* output);

};


#endif
