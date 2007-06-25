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
  const std::string& get_name() const;
  
  void set_name(const std::string& name);
  bool add_splitpoint(size_t frame);
  bool remove_splitpoint(size_t frame);
  bool move_splitpoint(size_t frame, size_t newframe);
  const Effect* add_static_effect(size_t pos, const std::string& effect_uri);
  bool remove_static_effect(size_t pos);
  bool bypass_static_effect(size_t pos, bool bypass);
    
protected:
  
  SampleBuffer m_orig_sample;
  Segmentation m_seg;
  EffectStack m_static_fx;
  SampleBuffer* m_proc_sample;
  std::vector<Chunk*> m_chunks;
  std::string m_name;
  
};


#endif
