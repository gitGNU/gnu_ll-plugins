/****************************************************************************

    samplemodel.cpp

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

#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "samplemodel.hpp"


SampleModel::SampleModel(const std::string& name, size_t length, double rate, 
			 const std::string& left, const std::string& right)
  : m_name(name),
    m_length(length),
    m_rate(rate),
    m_left(0),
    m_right(0),
    m_stereo(right != "") {
  
  // clear the peak data structures
  for (unsigned i = 0; i < 2; ++i) {
    for (unsigned j = 0; j < 3; ++j)
      m_peak_data[i][j] = 0;
  }
  
  // add splitpoints at start and end
  m_seg.push_back(0);
  m_seg.push_back(length);
  
  // load sound data
  if (!load_channel(0, left))
    return;
  if (m_stereo) {
    if (!load_channel(1, right))
      return;
  }
  
  generate_peak_data();
  
}
  

SampleModel::~SampleModel() {
  if (m_left) {
    munmap(m_left, m_length * sizeof(float));
    delete [] m_peak_data[0][1];
    delete [] m_peak_data[0][1];
    delete [] m_peak_data[0][2];
    if (m_stereo) {
      delete [] m_peak_data[1][1];
      delete [] m_peak_data[1][1];
      delete [] m_peak_data[1][2];
    }
  }
}


bool SampleModel::is_valid() const {
  return (m_left != 0);
}


size_t SampleModel::get_length() const {
  return m_length;
}


double SampleModel::get_rate() const {
  return m_rate;
}


size_t SampleModel::get_channels() const {
  return !m_left ? 0 : !m_right ? 1 : 2;
}


const SampleModel::PeakData* const* 
SampleModel::get_peak_data(size_t channel) const {
  return m_peak_data[channel];
}


const std::string& SampleModel::get_name() const {
  return m_name;
}


const float* SampleModel::get_data(size_t channel) const {
  if (channel == 0)
    return m_left;
  else if (channel == 1)
    return m_right;
  return 0;
}


const std::vector<size_t>& SampleModel::get_splitpoints() const {
  return m_seg;
}


void SampleModel::set_name(const std::string& name) {
  m_name = name;
}


bool SampleModel::set_data(const std::string& left, const std::string& right) {
  if (!load_channel(0, left))
    return false;
  if (right != "") {
    if (!load_channel(1, right))
      return false;
    m_stereo = true;
  }
  else
    m_stereo = false;
  generate_peak_data();
  return true;
}


void SampleModel::add_splitpoint(size_t frame) {
  if (frame >= m_length)
    return;
  size_t i;
  for (i = 0; i < m_seg.size(); ++i) {
    if (m_seg[i] == frame)
      return;
    else if (m_seg[i] > frame)
      break;
  }
  m_seg.insert(m_seg.begin() + i, frame);
}


void SampleModel::remove_splitpoint(size_t frame) {
  if (frame >= m_length || frame == 0)
    return;
  size_t i;
  for (i = 0; i < m_seg.size(); ++i) {
    if (m_seg[i] == frame)
      break;
    else if (m_seg[i] > frame)
      return;
  }
  m_seg.erase(m_seg.begin() + i);
}


void SampleModel::move_splitpoint(size_t frame, size_t newframe) {
  if (frame >= m_length || frame == 0)
    return;
  size_t i;
  for (i = 0; i < m_seg.size(); ++i) {
    if (m_seg[i] == frame)
      break;
    else if (m_seg[i] > frame)
      return;
  }
  if (newframe <= m_seg[i - 1] || newframe >= m_seg[i + 1])
    return;
  m_seg[i] = newframe;
}


const EffectStackModel& SampleModel::get_effect_stack_model() const {
  return m_stack;
}
  

EffectStackModel& SampleModel::get_effect_stack_model() {
  return m_stack;
}


bool SampleModel::load_channel(size_t channel, const std::string& name) {
  int fd = shm_open(name.c_str(), O_RDONLY, 0);
  if (fd == -1)
    return false;
  float* data = 0;
  data = static_cast<float*>(mmap(0, m_length * sizeof(float), 
				  PROT_READ, MAP_SHARED, fd, 0));
  close(fd);

  if (data) {
    if (channel == 0) {
      if (m_left)
	munmap(m_left, m_length * sizeof(float));
      m_left = data;
    }
    else if (channel == 1) {
      if (m_right)
	munmap(m_right, m_length * sizeof(float));
      m_right = data;
    }
    return true;
  }
  
  return false;
}


void SampleModel::generate_peak_data() {
  // clear the peak data structures
  for (unsigned i = 0; i < 2; ++i) {
    for (unsigned j = 0; j < 3; ++j) {
      delete [] m_peak_data[i][j];
      m_peak_data[i][j] = 0;
    }
  }
  
  // allocate peak data arrays
  m_peak_data[0][0] = new PeakData[m_length / 16 + 1];
  m_peak_data[0][1] = new PeakData[m_length / (16*16) + 1];
  m_peak_data[0][2] = new PeakData[m_length / (16*16*16) + 1];
  if (m_stereo) {
    m_peak_data[1][0] = new PeakData[m_length / 16 + 1];
    m_peak_data[1][1] = new PeakData[m_length / (16*16) + 1];
    m_peak_data[1][2] = new PeakData[m_length / (16*16*16) + 1];
  }
  
  // peak level 0
  for (unsigned i = 0; i < m_length / 16 + 1; ++i) {
    for (int c = 0; c < (m_stereo ? 2 : 1); ++c) {
      float& cmin = m_peak_data[c][0][i].min;
      float& cmax = m_peak_data[c][0][i].max;
      cmin = get_data(c)[16 * i];
      cmax = get_data(c)[16 * i];
      for (unsigned j = 1; j < 16; ++j) {
	cmin = cmin < get_data(c)[16 * i + j] ? cmin : get_data(c)[16 * i + j];
	cmax = cmax > get_data(c)[16 * i + j] ? cmax : get_data(c)[16 * i + j];
      }
    }
  }

  // peak level 1
  for (unsigned i = 0; i < m_length / (16*16) + 1; ++i) {
    for (int c = 0; c < (m_stereo ? 2 : 1); ++c) {
      float& cmin = m_peak_data[c][1][i].min;
      float& cmax = m_peak_data[c][1][i].max;
      cmin = m_peak_data[c][0][16 * i].min;
      cmax = m_peak_data[c][0][16 * i].max;
      for (unsigned j = 1; j < 16; ++j) {
	cmin = cmin < m_peak_data[c][0][16 * i + j].min ? 
	  cmin : m_peak_data[c][0][16 * i + j].min;
	cmax = cmax > m_peak_data[c][0][16 * i + j].max ? 
	  cmax : m_peak_data[c][0][16 * i + j].max;
      }
    }
  }

  // peak level 2
  for (unsigned i = 0; i < m_length / (16*16*16) + 1; ++i) {
    for (int c = 0; c < (m_stereo ? 2 : 1); ++c) {
      float& cmin = m_peak_data[c][2][i].min;
      float& cmax = m_peak_data[c][2][i].max;
      cmin = m_peak_data[c][1][16 * i].min;
      cmax = m_peak_data[c][1][16 * i].max;
      for (unsigned j = 1; j < 16; ++j) {
	cmin = cmin < m_peak_data[c][1][16 * i + j].min ? 
	  cmin : m_peak_data[c][1][16 * i + j].min;
	cmax = cmax > m_peak_data[c][1][16 * i + j].max ?
	  cmax : m_peak_data[c][1][16 * i + j].max;
      }
    }
  }

}

