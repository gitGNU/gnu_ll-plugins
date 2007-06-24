#include "chunk.hpp"
#include "sample.hpp"
#include "samplebuffer.hpp"
#include "segmentation.hpp"


Sample::Sample(const std::string& filename)
  : m_orig_sample(filename),
    m_seg(m_orig_sample.get_length()),
    m_proc_sample(0) {
  
  if (is_valid()) {
    m_chunks.push_back(new Chunk(*this, 0, 1));
    m_chunks.push_back(new Chunk(*this, 1, 2));
    m_chunks.push_back(new Chunk(*this, 2, 3));
    m_chunks.push_back(new Chunk(*this, 3, 4));
    
    for (unsigned i = filename.length() - 1; i >= 0; --i) {
      if (filename[i] == '/') {
	m_name = filename.substr(i + 1);
	break;
      }
    }
  }
  
}


Sample::~Sample() {
  if (m_proc_sample) {
    delete m_proc_sample;
  }
}


bool Sample::is_valid() const {
  return m_orig_sample.is_valid();
}
  

const Segmentation& Sample::get_segmentation() const {
  return m_seg;
}


const SampleBuffer& Sample::get_processed_buffer() const {
  if (m_proc_sample)
    return *m_proc_sample;
  return m_orig_sample;
}


const EffectStack& Sample::get_effect_stack() const {
  return m_static_fx;
}


const std::vector<Chunk*>& Sample::get_chunks() const {
  return m_chunks;
}
  

const std::string& Sample::get_name() const {
  return m_name;
}


void Sample::set_name(const std::string& name) {
  m_name = name;
}


bool Sample::add_splitpoint(size_t frame) {
  return m_seg.add_splitpoint(frame);
}


bool Sample::remove_splitpoint(size_t frame) {
  return m_seg.remove_splitpoint(frame);
}


bool Sample::move_splitpoint(size_t frame, size_t newframe) {
  return m_seg.move_splitpoint(frame, newframe);
}


bool Sample::add_static_effect(size_t pos, const std::string& effect_uri) {
  return m_static_fx.add_effect(effect_uri, pos);
}


bool Sample::remove_static_effect(size_t pos) {
  return m_static_fx.remove_effect(pos);
}


bool Sample::bypass_static_effect(size_t pos, bool bypass) {
  return m_static_fx.bypass_effect(pos, bypass);
}

