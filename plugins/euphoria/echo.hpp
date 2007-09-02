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

#ifndef ECHO_HPP
#define ECHO_HPP

#include <stdint.h>


class Echo {
public:
  
  Echo(uint32_t rate);
  ~Echo();

  void run(float* left, float* right, uint32_t nframes, float toggle,
	   float delay, float feedback, float pan, float mix);

private:
  
  
  uint32_t m_rate;
  uint32_t m_linesize;
  float* m_data_l;
  float* m_data_r;
  uint32_t m_counter;
  bool m_active;
  float m_l1;
  float m_r1;
  float m_p;
  
};


#endif
