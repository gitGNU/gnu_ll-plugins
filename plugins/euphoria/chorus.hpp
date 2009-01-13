/****************************************************************************
    
    chorus.hpp - a chorus effect
    
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

#ifndef CHORUS_HPP
#define CHORUS_HPP

#include <stdint.h>


class LFO3 {
public:

  LFO3(uint32_t rate);
  
  void run(float freq, float& p1, float& p2, float& p3);

protected:
  
  float m_invrate;
  float m_phase;
};




class Chorus {
public:
  
  Chorus(uint32_t rate);
  ~Chorus();

  void run(float* left, float* right, uint32_t nframes, float toggle,
	   float freq, float depth, float delay, float mix);

private:
  
  uint32_t m_rate;
  
  uint32_t m_maxfdelay;
  uint32_t m_maxfdepth;
  uint32_t m_linesize;
  
  float* m_data_l;
  float* m_data_r;
  uint32_t m_counter;
  
  LFO3 m_lfo;
};


#endif
