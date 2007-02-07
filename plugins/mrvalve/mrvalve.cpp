/****************************************************************************
    
    Mr Valve - LV2 distortion effect
    
    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>
    
    based on source code from the VST plugin AZR-3, (C) 2006 Philipp Mott
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
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
#include <iostream>

#include "distortion.hpp"
#include "lv2plugin.hpp"
#include "mrvalve.peg"


using namespace std;


static inline float clamp(float v, float l, float u) {
  v = v < l ? l : v;
  v = v > u ? u : v;
  return v;
}


/** This is the class that contains all the code and data for the audio
    identity plugin. */
class MrValve : public LV2Plugin {
public:
  
  MrValve(uint32_t rate, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(6),
      m_dist(rate) {
    
  }
  
  
  void run(uint32_t nframes) {
    float drive = clamp(*p(v_drive), 0, 1);
    float set = clamp(*p(v_set), 0, 1);
    float tone = clamp(*p(v_tone), 0, 1);
    float mix = clamp(*p(v_mix), 0, 1);
    
    if (p(v_input) != p(v_output))
      std::memcpy(p(v_output), p(v_input), nframes * sizeof(float));
    
    m_dist.run(p(v_output), nframes, 1, drive, set, tone, mix);
  }
  
protected:
  
  Distortion m_dist;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2<MrValve>("http://ll-plugins.nongnu.org/lv2/dev/mrvalve/0.0.0");
}
