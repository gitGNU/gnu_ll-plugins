#include "effectstackmodel.hpp"


EffectStackModel::EffectStackModel() {
  m_effects.push_back(EffectModel("Reverse"));
  m_effects.push_back(EffectModel("Moog filter"));
}


bool EffectStackModel::insert_effect(size_t pos, const std::string& name) {
  if (pos > m_effects.size())
    return false;
  m_effects.insert(m_effects.begin() + pos, EffectModel(name));
}
  

void EffectStackModel::bypass_effect(size_t pos, bool bypass) {
  if (pos >= m_effects.size())
    return;
  m_effects[pos].set_bypassed(bypass);
}


const std::vector<EffectModel>& EffectStackModel::get_effects() const {
  return m_effects;
}
