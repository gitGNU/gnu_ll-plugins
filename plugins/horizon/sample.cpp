#include "sample.hpp"
#include "samplebuffer.hpp"
#include "segmentation.hpp"


Sample::Sample(const std::string& filename)
  : m_orig_sample(filename),
    m_seg(m_orig_sample.get_length()),
    m_proc_sample(0) {

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
  
  

