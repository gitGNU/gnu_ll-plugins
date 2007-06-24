#ifndef EFFECTSTACKMODEL_HPP
#define EFFECTSTACKMODEL_HPP

#include <string>
#include <vector>

#include "effectmodel.hpp"


class EffectStackModel {
public:
  
  EffectStackModel();
  
  bool insert_effect(size_t pos, const std::string& name);
  bool remove_effect(size_t pos);
  bool bypass_effect(size_t pos, bool bypass);
  
  const std::vector<EffectModel>& get_effects() const;
  
protected:
  
  std::vector<EffectModel> m_effects;

};


#endif
