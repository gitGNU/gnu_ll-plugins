#include "segmentation.hpp"


Segmentation::Segmentation(size_t length) {
  m_segments.push_back(0);
  if (length > 0)
    m_segments.push_back(length);
}


const std::vector<size_t>& Segmentation::get_segments() const {
  return m_segments;
}


bool Segmentation::add_splitpoint(size_t frame) {
  if (frame >= m_segments[m_segments.size() - 1])
    return false;
  size_t i;
  for (i = 0; i < m_segments.size(); ++i) {
    if (m_segments[i] == frame)
      return false;
    else if (m_segments[i] > frame)
      break;
  }
  m_segments.insert(m_segments.begin() + i, frame);
  return true;
}


bool Segmentation::remove_splitpoint(size_t frame) {
  if (frame >= m_segments[m_segments.size() - 1] || frame == 0)
    return false;
  size_t i;
  for (i = 0; i < m_segments.size(); ++i) {
    if (m_segments[i] == frame)
      break;
    else if (m_segments[i] > frame)
      return false;
  }
  m_segments.erase(m_segments.begin() + i);
  return true;
}

