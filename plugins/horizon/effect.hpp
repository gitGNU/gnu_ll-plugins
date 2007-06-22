#ifndef EFFECT_HPP
#define EFFECT_HPP

#include <vector>


class Effect {
public:
  
  const std::vector<float>& get_parameters() const;
  bool set_parameter(size_t index, float value);
  void process(size_t nframes, float* input, float* output);
  
protected:
  
  std::vector<float> m_params;
  
};


#endif
