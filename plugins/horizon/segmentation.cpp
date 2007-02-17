#include "segmentation.hpp"


Segmentation::Segmentation(size_t length) {
  m_segments.push_back(0);
  if (length > 0)
    m_segments.push_back(length);
}


const std::vector<size_t>& Segmentation::get_segments() const {
  return m_segments;
}


