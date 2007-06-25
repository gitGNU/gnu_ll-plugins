#include "reverseeffect.hpp"


ReverseEffect::ReverseEffect()
  : Effect("Reverse") {
  
}
  

void ReverseEffect::process(size_t nframes, float* input, float* output) {
  for (size_t i = 0; i < nframes; ++i)
    output[i] = input[nframes - i - 1];
}
