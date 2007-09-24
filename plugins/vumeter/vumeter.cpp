/****************************************************************************
    
    vumeter.cpp - simple audio meter plugin
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
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

#include <cmath>
#include <cstring>
#include <iostream>

#include "lv2plugin.hpp"


using namespace std;


class VUMeter : public LV2::Plugin {
public:
  
  VUMeter(double rate, const char*, const LV2_Host_Feature* const*) 
    : LV2::Plugin(2),
      m_value(0.0),
      m_dy(1.0 / (1.0 * rate)){
    
  }
  
  
  void run(uint32_t nframes) {
    for (uint32_t i = 0; i < nframes; ++i) {
      const float& f = abs(p(0)[i]);
      if (f > m_value)
	m_value = f;
    }
    *p(1) = m_value > 1e-10 ? m_value : 0;
    if (m_value > m_dy * nframes)
      m_value -= m_dy * nframes;
    else
      m_value = 0;
  }
  
protected:
  
  float m_value;
  float m_dy;
  
};


static LV2::Register<VUMeter> 
reg("http://ll-plugins.nongnu.org/lv2/dev/vumeter/0");
