#include "reverseeffect.hpp"


ReverseEffect::ReverseEffect()
  : Effect("Reverse") {
  
}
  

void ReverseEffect::process(const float* input, float* output, size_t nframes) {
  for (size_t i = 0; i < nframes / 2; ++i) {
    float tmp = input[i];
    output[i] = input[nframes - i - 1];
    output[nframes - i - 1] = tmp;
  }
}
