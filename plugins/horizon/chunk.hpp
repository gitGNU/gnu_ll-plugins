#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <stdint.h>

#include <vector>


class Envelope;
class EffectStack;
class Sample;


class Chunk {
public:
  
  Chunk(const Sample& sample, uint32_t start, uint32_t end);
  
  uint32_t get_start() const;
  
  uint32_t get_end() const;
  
  const Sample& get_sample() const;  
  
protected:
  
  std::vector<Envelope*> m_envelopes;
  EffectStack* m_dyn_fx;
  const Sample& m_sample;
  uint32_t m_start;
  uint32_t m_end;
  
};


#endif
