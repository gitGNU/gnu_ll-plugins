/****************************************************************************
    
    wsvoice.hpp - a waveshaping synth voice
    
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

#ifndef WSVOICE_HPP
#define WSVOICE_HPP

#include <string>

#include "shaper.hpp"
#include "envelope.hpp"


class WSVoice {
public:
  
  WSVoice(uint32_t rate);
  ~WSVoice();
  
  void run(float* left, float* right, uint32_t nframes, float shape, 
	   float smoothness, float attack, float decay, float sustain, 
	   float release, float freq);
  
  void on(unsigned char key, unsigned char velocity);
  
  void off();
  
  void fast_off();
  
  void reset();
  
  void set_shape(const std::string& str);
  
  void set_amount_env(const std::string& str);
  
  void set_gain_env(const std::string& str);
  
private:
  
  Shaper m_shaper;
  Envelope m_shp_amount_env;
  Envelope m_shp_amp_env;
  float m_shp_phase;
  float m_shp_phase2;
  float m_inv_rate;
  
};


#endif
