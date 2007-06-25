#ifndef EFFECT_HPP
#define EFFECT_HPP

#include <string>
#include <vector>


class Effect {
public:
  
  Effect(const std::string& name);
  
  const std::vector<float>& get_parameters() const;
  const std::string& get_name() const;
  
  bool set_parameter(size_t index, float value);
  void set_name(const std::string& name);
  
  virtual void process(size_t nframes, float* input, float* output) = 0;
  
protected:
  
  std::vector<float> m_params;
  std::string m_name;
  
};


#endif
