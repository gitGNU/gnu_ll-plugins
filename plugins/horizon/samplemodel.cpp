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
  
  m_seg.push_back(0);
  m_seg.push_back(length);
  
  // load left channel (or mono)
  int fd = shm_open(left.c_str(), O_RDONLY, 0);
  if (fd == -1)
    return;
  m_left = static_cast<float*>(mmap(0, m_length * sizeof(float), 
				    PROT_READ, MAP_SHARED, fd, 0));
  close(fd);
  
  // load right channel (or not)
  if (m_stereo) {
    fd = shm_open(right.c_str(), O_RDONLY, 0);
    if (fd == -1)
      return;
    m_right = static_cast<float*>(mmap(0, m_length * sizeof(float),
				       PROT_READ, MAP_SHARED, fd, 0));
    close(fd);
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
  else {
    m_peak_data[1][0] = 0;
    m_peak_data[1][1] = 0;
    m_peak_data[1][2] = 0;
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
