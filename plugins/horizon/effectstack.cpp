#include <cstring>

#include "effect.hpp"
#include "effectstack.hpp"
#include "samplebuffer.hpp"


const std::vector<EffectStack::EffectInfo>& EffectStack::get_effects() const {
  return m_effects;
}

  
std::vector<EffectStack::EffectInfo>& EffectStack::get_effects() {
  return m_effects;
}

  
bool EffectStack::add_effect(const std::string& uri, size_t index) {
  // XXX ignoring the index for now
  m_effects.push_back(EffectInfo(new Effect));
  return true;
}


bool EffectStack::remove_effect(size_t index) {
  if (index >= m_effects.size())
    return false;
  delete m_effects[index].effect;
  m_effects.erase(m_effects.begin() + index);
  return true;
}


bool EffectStack::bypass_effect(size_t index, bool bypass) {
  if (index >= m_effects.size())
    return false;
  m_effects[index].bypassed = bypass;
  return true;
}

  
void EffectStack::process(const SampleBuffer& input,
                          float* output, size_t start, size_t end) {
  std::memcpy(output, input.get_data(0) + start, (end - start) * sizeof(float));
}


void EffectStack::process(const SampleBuffer& input, SampleBuffer& output) {
  process(input, output.get_data(0), 0, input.get_length());
}


