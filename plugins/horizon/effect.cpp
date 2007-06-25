#include "effect.hpp"


using namespace std;


Effect::Effect(const std::string& name)
  : m_name(name) {
  
}
  

const vector<float>& Effect::get_parameters() const {
  return m_params;
}


const string& Effect::get_name() const {
  return m_name;
}


bool Effect::set_parameter(size_t index, float value) {
  if (index >= m_params.size())
    return false;
  m_params[index] = value;
  return true;
}


void Effect::set_name(const std::string& name) {
  m_name = name;
}


