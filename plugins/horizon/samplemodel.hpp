#ifndef SAMPLEMODEL_HPP
#define SAMPLEMODEL_HPP

#include <string>
#include <vector>


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
  size_t get_channels() const;
  const std::string& get_name() const;
  const PeakData* const* get_peak_data(size_t channel) const;
  const float* get_data(size_t channel) const;
  const std::vector<size_t>& get_splitpoints() const;
  
  void set_name(const std::string& name);
  void add_splitpoint(size_t frame);
  void remove_splitpoint(size_t frame);
  
protected:
  
  PeakData* m_peak_data[2][3];
  
  std::string m_name;
  size_t m_length;
  double m_rate;
  
  float* m_left;
  float* m_right;
  
  bool m_stereo;
  
  std::vector<size_t> m_seg;
  
};


#endif
