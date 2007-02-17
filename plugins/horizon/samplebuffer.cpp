#include <sndfile.h>

#include "samplebuffer.hpp"


SampleBuffer::SampleBuffer(const std::string& filename) 
  : m_length(0),
    m_rate(0),
    m_channels(0),
    m_data(0) {
  
  // open the file
  SF_INFO s_info;
  SNDFILE* s_file = sf_open(filename.c_str(), SFM_READ, &s_info);
  if (!s_file)
    return;
  
  // check that it is sane
  if (s_info.frames == 0 || s_info.samplerate == 0 ||
      s_info.channels == 0 || s_info.channels > 2) {
    sf_close(s_file);
    return;
  }
  
  // allocate temporary buffer and read interleaved data into it
  float* tmp_data = new float[s_info.channels * s_info.frames];
  if (sf_read_float(s_file, tmp_data, s_info.frames) != s_info.frames) {
    sf_close(s_file);
    delete [] tmp_data;
    return;
  }
  sf_close(s_file);
  
  // deinterleave the data and delete the temporary buffer
  m_data = new float*[s_info.channels];
  if (s_info.channels > 1) {
    for (int c = 0; c < s_info.channels; ++c)
      m_data[c] = new float[s_info.frames];
    for (int f = 0; f < s_info.frames; ++f) {
      for (int c = 0; c < s_info.channels; ++c)
	m_data[c][f] = tmp_data[s_info.channels * f + c];
    }
    delete [] tmp_data;
  }
  else 
    m_data[0] = tmp_data;
  
  // set values
  m_length = s_info.frames;
  m_rate = s_info.samplerate;
  m_channels = s_info.channels;
}


SampleBuffer::~SampleBuffer() {
  if (m_data) {
    for (size_t i = 0; i < m_channels; ++i)
      delete [] m_data[i];
    delete [] m_data;
  }
}


bool SampleBuffer::is_valid() const {
  return m_data != 0;
}


const float* SampleBuffer::get_data(size_t channel) const {
  return m_data[channel];
}


float* SampleBuffer::get_data(size_t channel) {
  return m_data[channel];
}


size_t SampleBuffer::get_length() const {
  return m_length;
}


float SampleBuffer::get_rate() const {
  return m_rate;
}


size_t SampleBuffer::get_channels() const {
  return m_channels;
}

