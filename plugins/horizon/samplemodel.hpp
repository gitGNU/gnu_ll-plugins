#ifndef SAMPLEMODEL_HPP
#define SAMPLEMODEL_HPP

#include <string>
#include <vector>

#include "effectstackmodel.hpp"


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
  const EffectStackModel& get_effect_stack_model() const;
  
  EffectStackModel& get_effect_stack_model();
  
  void set_name(const std::string& name);
  bool set_data(const std::string& left, const std::string& right = "");
  void add_splitpoint(size_t frame);
  void remove_splitpoint(size_t frame);
  void move_splitpoint(size_t frame, size_t newframe);
  
protected:
  
  bool load_channel(size_t channel, const std::string& name);
  void generate_peak_data();
  
  PeakData* m_peak_data[2][3];
  
  std::string m_name;
  size_t m_length;
  double m_rate;
  
  float* m_left;
  float* m_right;
  
  bool m_stereo;
  
  std::vector<size_t> m_seg;
  
  float* m_envelope;
  
  EffectStackModel m_stack;
  
};


#endif
