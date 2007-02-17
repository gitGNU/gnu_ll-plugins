#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <vector>


class Envelope;
class EffectStack;


class Chunk {
public:
  
protected:
  
  std::vector<Envelope*> m_envelopes;
  EffectStack* m_dyn_fx;
  
};


#endif
