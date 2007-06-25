#ifndef SAMPLEBUFFER_HPP
#define SAMPLEBUFFER_HPP

#include <string>


class SampleBuffer {
public:
  
  SampleBuffer(const std::string& filename);
  SampleBuffer(const SampleBuffer& buf);
  ~SampleBuffer();
  
  bool is_valid() const;
  
  const float* get_data(size_t channel) const;
  float* get_data(size_t channel);
  size_t get_length() const;
  float get_rate() const;
  size_t get_channels() const;
  const std::string& get_shm_name(size_t channel) const;
  
protected:
  
  float* shm_alloc(size_t channel, size_t floats);
  void shm_free(size_t channel);
  
  size_t m_length;
  float m_rate;
  size_t m_channels;
  float** m_data;
  std::string* m_shm_names;
  
};


#endif
