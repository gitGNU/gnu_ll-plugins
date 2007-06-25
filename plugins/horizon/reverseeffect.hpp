#ifndef REVERSEEFFECT_HPP
#define REVERSEEFFECT_HPP

#include "effect.hpp"


class ReverseEffect : public Effect {
public:
  
  ReverseEffect();
  
  void process(const float* input, float* output, size_t nframes);

};


#endif
