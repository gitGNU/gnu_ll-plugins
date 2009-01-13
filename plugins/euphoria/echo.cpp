/****************************************************************************
    
    echo.hpp - an echo effect
    
    Copyright (C) 2007 Lars Luthman <larsl@users.sourceforge.net>
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#include <cmath>
#include <cstring>

#include "echo.hpp"

  
Echo::Echo(uint32_t rate) 
  : m_rate(rate),
    m_linesize(m_rate * 3),
    m_data_l(new float[m_linesize]),
    m_data_r(new float[m_linesize]),
    m_counter(0),
    m_active(false),
    m_l1(0),
    m_r1(0),
    m_p(std::pow(1 - 2 * 1000.0 / rate, 2)) {

  std::memset(m_data_l, 0, sizeof(float) * m_linesize);
  std::memset(m_data_r, 0, sizeof(float) * m_linesize);
}


Echo::~Echo() {
  delete [] m_data_l;
  delete [] m_data_r;
}


void Echo::run(float* left, float* right, uint32_t nframes, float toggle,
	       float delay, float feedback, float pan, float mix) {
  
  int offset = m_linesize - int(delay * m_rate);
  offset = offset < 1 ? 1 : offset;
  
  m_active = toggle > 0;
  
  if (m_active && toggle <= 0) {
    std::memset(m_data_l, 0, sizeof(float) * m_linesize);
    std::memset(m_data_r, 0, sizeof(float) * m_linesize);
    m_active = false;
    m_l1 = 0;
    m_r1 = 0;
  }
  
  if (m_active) {
    for (uint32_t i = 0; i < nframes; ++i, ++m_counter) {
      m_counter %= m_linesize;
      float l = feedback * m_data_r[(m_counter + offset) % m_linesize];
      float r = feedback * m_data_l[(m_counter + offset) % m_linesize];
      m_l1 = (1 - m_p) * l + m_p * m_l1;
      m_r1 = (1 - m_p) * r + m_p * m_r1;
      m_data_l[m_counter] = 0.5 * left[i] + m_l1;
      m_data_r[m_counter] = right[i] + m_r1;
      left[i] += mix * m_l1;
      right[i] += mix * m_r1;
    }
  }
  
}
