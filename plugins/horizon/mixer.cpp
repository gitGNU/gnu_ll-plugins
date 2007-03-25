#include <cstring>
#include <iostream>

#include "mixer.hpp"


using namespace std;


Mixer::Mixer()
  : m_sample("/home/ll/div/loops/tomsphases.wav"),
    m_left(0),
    m_right(0),
    m_gain(0) {
  
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
  memset(m_left, 0, nframes * sizeof(float));
  memset(m_right, 0, nframes * sizeof(float));
  m_voice.render_to(m_left, m_right, nframes, m_gain);
  m_left += nframes;
  m_right += nframes;
}


void Mixer::play_chunk(const Chunk* chunk) {
  m_voice.set_chunk(chunk);
  m_gain = 1;
}


void Mixer::stop() {
  m_voice.set_chunk(0);
  m_gain = 0;
}
