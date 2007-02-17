#include <iostream>

#include "mixer.hpp"


using namespace std;


Mixer::Mixer()
  : m_sample("/home/ll/div/loops/us10phase.wav"),
    m_pos(0),
    m_left(0),
    m_right(0) {
  
  if (m_sample.is_valid()) {
    const SampleBuffer& buf = m_sample.get_processed_buffer();
    cerr<<"The sample was loaded OK!"<<endl
	<<" channels:   "<<buf.get_channels()<<endl
	<<" samplerate: "<<buf.get_rate()<<endl
	<<" frames:     "<<buf.get_length()<<endl;
    
  }
  else
    cerr<<"The sample was not loaded OK!"<<endl;

}


void Mixer::set_buffers(float* left, float* right) {
  m_left = left;
  m_right = right;
}


void Mixer::run(uint32_t nframes) {
  const SampleBuffer& buf = m_sample.get_processed_buffer();
  do {
    unsigned n = nframes;
    n = n > buf.get_length() - m_pos ? buf.get_length() - m_pos : n;
    memcpy(m_left, buf.get_data(0) + m_pos, n * sizeof(float));
    memcpy(m_right, buf.get_data(0) + m_pos, n * sizeof(float));
    nframes -= n;
    m_pos += n;
    m_pos %= buf.get_length();
  } while (nframes > 0);
}
