#include "effectstackmodel.hpp"


EffectStackModel::EffectStackModel() {
  
}


bool EffectStackModel::insert_effect(size_t pos, const std::string& name) {
  if (pos > m_effects.size())
    return false;
  m_effects.insert(m_effects.begin() + pos, EffectModel(name));
  return true;
}
  

bool EffectStackModel::remove_effect(size_t pos) {
  if (pos >= m_effects.size())
    return false;
  m_effects.erase(m_effects.begin() + pos);
  return true;
}


bool EffectStackModel::bypass_effect(size_t pos, bool bypass) {
  if (pos >= m_effects.size())
    return false;
  m_effects[pos].set_bypassed(bypass);
  return true;
}


const std::vector<EffectModel>& EffectStackModel::get_effects() const {
  return m_effects;
}
