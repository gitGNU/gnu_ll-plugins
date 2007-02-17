#ifndef EFFECTSTACK_HPP
#define EFFECTSTACK_HPP

#include <vector>


class Effect;
class SampleBuffer;

class EffectStack {
public:
  
  void process(const SampleBuffer& input,
               float* output, size_t start, size_t end);
  inline void process(const SampleBuffer& input, SampleBuffer& output);
  
protected:
  
  std::vector<Effect*> m_effects;
  
};


#endif
