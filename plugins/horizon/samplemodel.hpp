/****************************************************************************

    samplemodel.hpp

    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

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
  const std::vector<PeakData>* get_peak_data(size_t channel) const;
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
  
  std::vector<PeakData> m_peak_data[2][3];
  
  std::string m_name;
  size_t m_length;
  double m_rate;
  
  float* m_left;
  float* m_right;
  
  bool m_stereo;
  
  std::vector<size_t> m_seg;
  
  EffectStackModel m_stack;
  
};


#endif
