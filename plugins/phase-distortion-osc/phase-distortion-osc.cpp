/****************************************************************************
    
    phase-distortion-osc.cpp - A phase distortion oscillator
    
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

#include "lv2plugin.hpp"
#include "pdosc.hpp"


class PhaseDistortionOscillator : public LV2Plugin {
public:
  
  PhaseDistortionOscillator(double rate, const char*, 
                            const LV2_Host_Feature* const*) 
    : LV2Plugin(4),
      m_osc(rate) {

  }
  
  
  void activate() {
    m_osc.reset();
  }

  
  void run(uint32_t nframes) {
    float* f = static_cast<float*>(m_ports[0]);
    float shape = *static_cast<float*>(m_ports[1]);
    float amount = *static_cast<float*>(m_ports[2]);
    float* output = static_cast<float*>(m_ports[3]);
    
    int ishape = int(shape);
    ishape = (ishape < 0 ? 0 : ishape);
    ishape = (ishape > 5 ? 5 : ishape);
    amount = (amount < 0 ? 0 : amount);
    amount = (amount > 0.999 ? 0.999 : amount);
    
    for (uint32_t i = 0; i < nframes; ++i)
      output[i] = m_osc.run(440, ishape, amount);
  }
  

protected:
  
  PDOsc m_osc;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2<PhaseDistortionOscillator>("http://ll-plugins.nongnu.org/lv2/dev/phase-distortion-osc/0.0.0");
}
