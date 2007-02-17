#ifndef SEGMENTATION_HPP
#define SEGMENTATION_HPP

#include <vector>


class Segmentation {
public:
  
  Segmentation(size_t length);
  
  const std::vector<size_t>& get_segments() const;
  
protected:
  
  std::vector<size_t> m_segments;
  
};


#endif
