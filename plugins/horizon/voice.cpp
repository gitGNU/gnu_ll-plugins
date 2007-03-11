#include <iostream>

#include "chunk.hpp"
#include "sample.hpp"
#include "voice.hpp"


using namespace std;


Voice::Voice() 
  : m_chunk(0),
    m_pos(0) {

}

  
void Voice::set_chunk(const Chunk* chunk) {
  m_chunk = chunk;
  if (m_chunk) {
    m_pos = m_chunk->get_sample().
      get_segmentation().get_segments()[m_chunk->get_start()];
  }
}


void Voice::render_to(float* left, float* right, uint32_t nframes, float gain) {
  if (!m_chunk)
    return;
  
  // make sure chunk boundaries are sane
  const Segmentation& seg = m_chunk->get_sample().get_segmentation();
  const SampleBuffer& buf = m_chunk->get_sample().get_processed_buffer();
  uint32_t start = m_chunk->get_start();
  //cerr<<"start: "<<start<<endl;
  start = start < seg.get_segments().size() ? 
    start : seg.get_segments().size() - 1;
  //cerr<<"start: "<<start<<endl;
  uint32_t startpos = seg.get_segments()[start];
  //cerr<<"startpos: "<<startpos<<endl;
  startpos = startpos < buf.get_length() ? startpos : buf.get_length();
  //cerr<<"startpos: "<<startpos<<endl;
  uint32_t end = m_chunk->get_end();
  end = end < seg.get_segments().size() ? 
    end : seg.get_segments().size() - 1;
  uint32_t endpos = seg.get_segments()[end];
  endpos = endpos < buf.get_length() ? endpos : buf.get_length();
  if (startpos >= endpos)
    return;
  
  const float* data = buf.get_data(0);
  for (uint32_t i = 0; i < nframes; ++i) {
    left[i] += gain * data[m_pos];
    right[i] += gain * data[m_pos];
    ++m_pos;
    if (m_pos >= endpos)
      m_pos = startpos;
  }
}

