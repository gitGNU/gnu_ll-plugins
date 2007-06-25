#ifndef NORMALISEEFFECT_HPP
#define NORMALISEEFFECT_HPP

#include "effect.hpp"


class NormaliseEffect : public Effect {
public:
  
  NormaliseEffect();
  
  void process(size_t nframes, float* input, float* output);

};


#endif
