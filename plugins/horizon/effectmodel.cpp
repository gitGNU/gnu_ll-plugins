#include "effectmodel.hpp"


EffectModel::EffectModel(const std::string& name)
  : m_name(name),
    m_bypassed(false) {

}
  

const std::string& EffectModel::get_name() const {
  return m_name;
}


bool EffectModel::get_bypassed() const {
  return m_bypassed;
}
  

void EffectModel::set_name(const std::string& name) {
  m_name = name;
}


void EffectModel::set_bypassed(bool bypass) {
  m_bypassed = bypass;
}
