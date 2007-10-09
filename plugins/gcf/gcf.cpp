/****************************************************************************
    
    gcf.cpp - guitar tuner plugin
    
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

#include <cstring>

#include "lv2plugin.hpp"


using namespace std;


class GCF : public LV2::Plugin {
public:
  
  GCF(double rate, const char*, const LV2_Feature* const*) 
    : LV2::Plugin(3),
      m_rate(rate),
      m_counter(0),
      m_frames(0),
      m_below(true) {
    for (unsigned i = 0; i < PERIOD_COUNT; ++i)
      m_periods[i] = 0;
  }
  
  
  void run(uint32_t nframes) {
    
    // simple zero-crossing counter
    for (uint32_t i = 0; i < nframes; ++i) {
      ++m_frames;
      if (m_below && p(0)[i] > 0) {
	m_periods[m_counter] = m_frames;
	m_counter = (m_counter + 1) % PERIOD_COUNT;
	m_frames = 0;
      }
      m_below = p(0)[i] < 0;
    }
    float acc = 0;
    for (unsigned i = 0; i < PERIOD_COUNT; ++i)
      acc += m_periods[i];
    acc /= PERIOD_COUNT;
    if (acc == 0)
      *p(1) = 0;
    else
      *p(1) = m_rate / acc;
    
    // MPM algorithm
    
    
    
  }

protected:
  
  enum {
    PERIOD_COUNT = 100
  };
  
  double m_rate;
  float m_periods[PERIOD_COUNT];
  uint32_t m_counter;
  uint32_t m_frames;
  bool m_below;
  
};


static LV2::Register<GCF> 
reg("http://ll-plugins.nongnu.org/lv2/dev/gcf/0");
