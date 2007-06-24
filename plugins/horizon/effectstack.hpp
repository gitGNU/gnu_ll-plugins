#ifndef EFFECTSTACK_HPP
#define EFFECTSTACK_HPP

#include <vector>


class Effect;
class SampleBuffer;

class EffectStack {
public:
  
  struct EffectInfo {

    EffectInfo(Effect* e)
      : effect(e),
	bypassed(false) {

    }
    
    Effect* effect;
    bool bypassed;
  };
  
  const std::vector<EffectInfo>& get_effects() const;
  
  std::vector<EffectInfo>& get_effects();
  
  bool add_effect(const std::string& uri, size_t index);
  bool remove_effect(size_t index);
  bool bypass_effect(size_t index, bool bypass);
  
  void process(const SampleBuffer& input,
               float* output, size_t start, size_t end);
  inline void process(const SampleBuffer& input, SampleBuffer& output);
  
protected:
  
  std::vector<EffectInfo> m_effects;
  
};


#endif
