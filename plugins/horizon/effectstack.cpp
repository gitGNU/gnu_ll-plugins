#include <cstring>

#include "normaliseeffect.hpp"
#include "reverseeffect.hpp"
#include "effectstack.hpp"
#include "samplebuffer.hpp"


const std::vector<EffectStack::EffectInfo>& EffectStack::get_effects() const {
  return m_effects;
}

  
std::vector<EffectStack::EffectInfo>& EffectStack::get_effects() {
  return m_effects;
}

  
const Effect* EffectStack::add_effect(const std::string& uri, size_t index) {
  if (index > m_effects.size())
    return 0;

  if (uri == "reverse") {
    m_effects.insert(m_effects.begin() + index, 
		     EffectInfo(new ReverseEffect));
    return m_effects[index].effect;
  }
  
  else if (uri == "normalise") {
    m_effects.insert(m_effects.begin() + index,
		     EffectInfo(new NormaliseEffect));
    return m_effects[index].effect;
  }
  
  return 0;
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


