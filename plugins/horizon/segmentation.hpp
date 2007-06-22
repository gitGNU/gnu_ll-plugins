#ifndef SEGMENTATION_HPP
#define SEGMENTATION_HPP

#include <vector>


class Segmentation {
public:
  
  Segmentation(size_t length);
  
  const std::vector<size_t>& get_segments() const;
  
  bool add_splitpoint(size_t frame);
  bool remove_splitpoint(size_t frame);
  bool move_splitpoint(size_t frame, size_t newframe);
  
protected:
  
  std::vector<size_t> m_segments;
  
};


#endif
