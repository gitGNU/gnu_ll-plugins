#include "effect.hpp"


using namespace std;
  

const vector<float>& Effect::get_parameters() const {
  return m_params;
}


bool Effect::set_parameter(size_t index, float value) {
  if (index >= m_params.size())
    return false;
  m_params[index] = value;
  return true;
}


void Effect::process(size_t nframes, float* input, float* output) {
  for (size_t i = 0; i < nframes; ++i)
    output[i] = input[nframes - i - 1];
}
