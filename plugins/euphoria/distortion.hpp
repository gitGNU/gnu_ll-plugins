/****************************************************************************
    
    distortion.hpp - a distortion effect
    
    Copyright (C) 2008 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA

****************************************************************************/

#ifndef DISTORTION_HPP
#define DISTORTION_HPP

#include <cmath>

#include "chamberlinsv.hpp"


class Distortion {
public:
  
  Distortion(unsigned int rate)
    : m_pre_filter(rate, 190, 1.5),
      m_hp_filter(rate, 1100, 1.5),
      m_tone_filter(rate, 3800, 0.7),
      m_clean_filter(rate, 2700, 1.2) {
    
  }
  
  float run(float input, float drive, float set, float mix, float tone) {
    
    // useful parameter
    float dist = 2 * drive + 0.2;
    
    // lower band distortion
    float output = 6 * std::atan(8 * dist * m_pre_filter.run(input).low());
    
    // get rid of the lowest bass
    m_hp_filter.run(output);
    output = m_hp_filter.band() + m_hp_filter.high();
    
    // add full band distortion
    output += 0.25 * std::atan(4 * dist * input);
    
    // add some grit
    output = std::abs(input) > set ? std::atan(10 * output) : output;
    
    // lowpass tone filter
    m_tone_filter.set_frequency(800 + 3000 * tone); 
    output = m_tone_filter.run(output).low();
    
    // mix with original signal
    float sd = std::sin(dist);
    output = output * mix * sd + (2 - mix) * input;
    
    // scale and reduce nasty upper harmonics
    output = m_clean_filter.run(output * sd / dist).low();
    
    return output;
  }
  
protected:
  
  ChamberlinSV m_pre_filter;
  ChamberlinSV m_hp_filter;
  ChamberlinSV m_tone_filter;
  ChamberlinSV m_clean_filter;
  
};


#endif
