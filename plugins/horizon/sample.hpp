#ifndef SAMPLE_HPP
#define SAMPLE_HPP

#include <string>
#include <vector>

#include "samplebuffer.hpp"
#include "segmentation.hpp"
#include "effectstack.hpp"


class Chunk;


class Sample {
public:
  
  Sample(const std::string& filename);
  ~Sample();
  
  bool is_valid() const;
  
  const Segmentation& get_segmentation() const;
  const SampleBuffer& get_processed_buffer() const;
  const EffectStack& get_effect_stack() const;
  const std::vector<Chunk*>& get_chunks() const;
    
protected:
  
  SampleBuffer m_orig_sample;
  Segmentation m_seg;
  EffectStack m_static_fx;
  SampleBuffer* m_proc_sample;
  std::vector<Chunk*> m_chunks;
  
};


#endif
