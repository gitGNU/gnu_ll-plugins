/****************************************************************************

    samplebuffer.cpp

    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#include <iostream>
#include <cstdio>

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <sndfile.h>

#include "samplebuffer.hpp"


SampleBuffer::SampleBuffer(const std::string& filename) 
  : m_length(0),
    m_rate(0),
    m_channels(0),
    m_data(0),
    m_shm_names(0) {
  
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

  // set values
  m_length = s_info.frames;
  m_rate = s_info.samplerate;
  m_channels = s_info.channels;
  
  // allocate temporary buffer and read interleaved data into it
  m_shm_names = new std::string[s_info.channels];
  float* tmp_data;
  if (s_info.channels == 1)
    tmp_data = shm_alloc(0, s_info.frames);
  else
    tmp_data = new float[s_info.channels * s_info.frames];
  if (sf_readf_float(s_file, tmp_data, s_info.frames) != s_info.frames) {
    sf_close(s_file);
    if (s_info.channels == 1)
      shm_free(0);
    else
      delete [] tmp_data;
    return;
  }
  sf_close(s_file);
  
  // deinterleave the data and delete the temporary buffer
  m_data = new float*[s_info.channels];
  if (s_info.channels > 1) {
    for (int c = 0; c < s_info.channels; ++c)
      m_data[c] = shm_alloc(c, s_info.frames);
    for (int f = 0; f < s_info.frames; ++f) {
      for (int c = 0; c < s_info.channels; ++c)
	m_data[c][f] = tmp_data[s_info.channels * f + c];
    }
    delete [] tmp_data;
  }
  else 
    m_data[0] = tmp_data;
  
}


SampleBuffer::SampleBuffer(const SampleBuffer& buf) 
  : m_length(buf.get_length()),
    m_rate(buf.get_rate()),
    m_channels(buf.get_channels()),
    m_data(new float*[m_channels]),
    m_shm_names(0) {

  if (!buf.is_valid())
    return;
  
  m_shm_names = new std::string[m_channels];
  
  for (int c = 0; c < m_channels; ++c) {
    m_data[c] = shm_alloc(c, m_length);
    memcpy(m_data[c], buf.get_data(c), m_length * sizeof(float));
  }
}


SampleBuffer::~SampleBuffer() {
  if (m_data) {
    for (size_t i = 0; i < m_channels; ++i)
      shm_free(i);
    delete [] m_data;
  }
  
  if (m_shm_names)
    delete [] m_shm_names;

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


const std::string& SampleBuffer::get_shm_name(size_t channel) const {
  return m_shm_names[channel];
}


float* SampleBuffer::shm_alloc(size_t channel, size_t floats) {
  char name_buffer[16];
  float* buffer = 0;
  
  for (unsigned i = 0; i < 10000; ++i) {
    std::sprintf(name_buffer, "/horizon-%u", i);
    int fd = shm_open(name_buffer, O_RDWR | O_CREAT | O_EXCL, S_IRWXU);
    if (fd == -1) {
      if (errno == EEXIST)
	continue;
      break;
    }
    if (!ftruncate(fd, sizeof(float) * floats)) {
      buffer = static_cast<float*>(mmap(0, sizeof(float) * floats, 
					PROT_READ | PROT_WRITE, MAP_SHARED,
					fd, 0));
      if (!buffer)
	shm_unlink(name_buffer);
      close(fd);
    }
    break;
  }
  
  if (buffer)
    m_shm_names[channel] = name_buffer;
  return buffer;
}


void SampleBuffer::shm_free(size_t channel) {
  std::cerr<<"Unlinking "<<m_shm_names[channel]<<std::endl;
  shm_unlink(m_shm_names[channel].c_str());
  munmap(m_data[channel], sizeof(float) * m_length);
}
