#include <cstring>

#include "effectstack.hpp"
#include "samplebuffer.hpp"


void EffectStack::process(const SampleBuffer& input,
                          float* output, size_t start, size_t end) {
  std::memcpy(output, input.get_data(0) + start, (end - start) * sizeof(float));
}


void EffectStack::process(const SampleBuffer& input, SampleBuffer& output) {
  process(input, output.get_data(0), 0, input.get_length());
}


