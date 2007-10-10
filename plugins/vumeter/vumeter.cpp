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

#include "lv2plugin.hpp"


template <unsigned C>
class VUMeter : public LV2::Plugin< VUMeter<C> > {
public:

  using LV2::Plugin< VUMeter<C> >::p;
  
  VUMeter(double rate, const char*, const LV2_Feature* const*) 
    : LV2::Plugin< VUMeter<C> >(2 * C),
      m_dy(1.0 / (1.0 * rate)) {
    for (unsigned i = 0; i < C; ++i)
      m_values[i] = 0.0;
  }
  
  void run(uint32_t nframes) {
    for (unsigned c = 0; c < C; ++c) {
      for (uint32_t i = 0; i < nframes; ++i) {
	float f = std::abs(p(2 * c)[i]);
	m_values[c] = f > m_values[c] ? f : m_values[c];
      }
      *p(2 * c + 1) = m_values[c] > 1e-10 ? m_values[c] : 0;
      if (m_values[c] > m_dy * nframes)
	m_values[c] -= m_dy * nframes;
      else
	m_values[c] = 0.0;
    }
  }
  
protected:
  
  float m_values[C];
  float m_dy;
  
};


static unsigned _1 = VUMeter<1>::register_class("http://ll-plugins.nongnu.org/lv2/dev/vumeter/0");
static unsigned _2 = VUMeter<2>::register_class("http://ll-plugins.nongnu.org/lv2/dev/vumeter-stereo/0");
