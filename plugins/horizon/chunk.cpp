#include "chunk.hpp"

  
Chunk::Chunk(const Sample& sample, uint32_t start, uint32_t end)
  : m_sample(sample),
    m_start(start),
    m_end(end) {

}
  

uint32_t Chunk::get_start() const {
  return m_start;
}
  

uint32_t Chunk::get_end() const {
  return m_end;
}


const Sample& Chunk::get_sample() const {
  return m_sample;
}
