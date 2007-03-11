#ifndef VOICE_HPP
#define VOICE_HPP

#include <stdint.h>


class Chunk;


class Voice {
public:
  
  Voice();
  
  void set_chunk(const Chunk* chunk);
  
  void render_to(float* left, float* right, uint32_t nframes, float gain);
  
protected:
  
  const Chunk* m_chunk;
  uint32_t m_pos;

};


#endif
