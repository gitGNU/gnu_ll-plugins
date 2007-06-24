#ifndef EFFECTMODEL_HPP
#define EFFECTMODEL_HPP

#include <string>


class EffectModel {
public:
  
  EffectModel(const std::string& name);
  
  const std::string& get_name() const;
  bool get_bypassed() const;
  
  void set_name(const std::string& name);
  void set_bypassed(bool bypass);
  
protected:
  
  std::string m_name;
  
  bool m_bypassed;

};


#endif
