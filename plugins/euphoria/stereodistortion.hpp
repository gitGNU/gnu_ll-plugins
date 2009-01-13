/****************************************************************************
    
    stereodistortion.hpp - a stereo distortion effect
    
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

#ifndef STEREODISTORTION_HPP
#define STEREODISTORTION_HPP

#include <cmath>

#include "distortion.hpp"


class StereoDistortion {
public:
  
  StereoDistortion(unsigned int rate) 
    : m_left(rate),
      m_right(rate) {
    
  }
  
  void run(float* left, float* right, uint32_t nframes, float toggle,
	   float drive, float set, float mix, float tone) {
    if (toggle < 0.5)
      return;
    for (uint32_t i = 0; i < nframes; ++i) {
      left[i] = m_left.run(left[i], drive, set, mix, tone);
      right[i] = m_right.run(right[i], drive, set, mix, tone);
    }
  }
  
protected:
  
  Distortion m_left;
  Distortion m_right;
  
};


#endif
