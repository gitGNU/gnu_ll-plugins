#ifndef NORMALISEEFFECT_HPP
#define NORMALISEEFFECT_HPP

#include "effect.hpp"


class NormaliseEffect : public Effect {
public:
  
  NormaliseEffect();
  
  void process(const float* input, float* output, size_t nframes);

};


#endif
