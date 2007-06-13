#ifndef SAMPLEMODEL_HPP
#define SAMPLEMODEL_HPP

#include <string>


class SampleModel {
public:
  

  struct PeakData {
    float min;
    float max;
  };
  

  SampleModel(const std::string& name, size_t length, double rate, 
	      const std::string& left, const std::string& right = "");
  
  ~SampleModel();
  
  bool is_valid() const;
  size_t get_length() const;
  double get_rate() const;
  const std::string& get_name() const;
  const PeakData* const* get_peak_data() const;
  const float* get_data(size_t channel) const;

  void set_name(const std::string& name);
  
protected:
  
  PeakData* m_peak_data[3];
  
  std::string m_name;
  size_t m_length;
  double m_rate;
  
  float* m_left;
  float* m_right;
  
  bool m_stereo;
  
};


#endif
