#ifndef SAMPLEBUFFER_HPP
#define SAMPLEBUFFER_HPP

#include <string>


class SampleBuffer {
public:
  
  SampleBuffer(const std::string& filename);
  ~SampleBuffer();
  
  bool is_valid() const;
  
  const float* get_data(size_t channel) const;
  float* get_data(size_t channel);
  size_t get_length() const;
  float get_rate() const;
  size_t get_channels() const;
  
protected:
  
  size_t m_length;
  float m_rate;
  size_t m_channels;
  float** m_data;
  
};


#endif
