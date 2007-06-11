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
  
  int fd = shm_open(left.c_str(), O_RDONLY, 0);
  if (fd == -1)
    return;
  m_left = static_cast<float*>(mmap(0, m_length * sizeof(float), 
				    PROT_READ, MAP_SHARED, fd, 0));
  close(fd);
  
  // allocate peak data arrays
  m_peak_data[0] = new PeakData[m_length / 16 + 1];
  m_peak_data[1] = new PeakData[m_length / (16*16) + 1];
  m_peak_data[2] = new PeakData[m_length / (16*16*16) + 1];
  
  // peak level 0
  for (unsigned i = 0; i < m_length / 16 + 1; ++i) {
    float& cmin = m_peak_data[0][i].min;
    float& cmax = m_peak_data[0][i].max;
    cmin = m_left[16 * i];
    cmax = m_left[16 * i];
    for (unsigned j = 1; j < 16; ++j) {
      cmin = cmin < m_left[16 * i + j] ? cmin : m_left[16 * i + j];
      cmax = cmax > m_left[16 * i + j] ? cmax : m_left[16 * i + j];
    }
  }

  // peak level 1
  for (unsigned i = 0; i < m_length / (16*16) + 1; ++i) {
    float& cmin = m_peak_data[1][i].min;
    float& cmax = m_peak_data[1][i].max;
    cmin = m_peak_data[0][16 * i].min;
    cmax = m_peak_data[0][16 * i].max;
    for (unsigned j = 1; j < 16; ++j) {
      cmin = cmin < m_peak_data[0][16 * i + j].min ? 
	cmin : m_peak_data[0][16 * i + j].min;
      cmax = cmax > m_peak_data[0][16 * i + j].max ? 
	cmax : m_peak_data[0][16 * i + j].max;
    }
  }

  // peak level 2
  for (unsigned i = 0; i < m_length / (16*16*16) + 1; ++i) {
    float& cmin = m_peak_data[2][i].min;
    float& cmax = m_peak_data[2][i].max;
    cmin = m_peak_data[1][16 * i].min;
    cmax = m_peak_data[1][16 * i].max;
    for (unsigned j = 1; j < 16; ++j) {
      cmin = cmin < m_peak_data[1][16 * i + j].min ? 
	cmin : m_peak_data[1][16 * i + j].min;
      cmax = cmax > m_peak_data[1][16 * i + j].max ?
	cmax : m_peak_data[1][16 * i + j].max;
    }
  }
}
  

SampleModel::~SampleModel() {
  if (m_left) {
    munmap(m_left, m_length * sizeof(float));
    delete [] m_peak_data[0];
    delete [] m_peak_data[1];
    delete [] m_peak_data[2];
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


const SampleModel::PeakData* const* SampleModel::get_peak_data() const {
  return m_peak_data;
}
