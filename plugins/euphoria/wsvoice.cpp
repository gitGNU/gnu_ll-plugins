/****************************************************************************
    
    wsvoice.cpp - a waveshaping synth voice
    
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

#include "wsvoice.hpp"


using namespace std;
  

WSVoice::WSVoice(uint32_t rate)
  : m_shaper(rate),
    m_shp_amount_env(rate),
    m_shp_amp_env(rate),
    m_shp_phase(0),
    m_shp_phase2(0),
    m_inv_rate(1.0f / rate) {

    m_shaper.set_string("-1 -1 1 1");

}


WSVoice::~WSVoice() {

}


void WSVoice::run(float* left, float* right, uint32_t nframes, float shape, 
		  float smoothness, float attack, float decay, float sustain, 
		  float release, float freq) {

  for (uint32_t i = 0; i < nframes; ++i) {
    float shp_amount_env = m_shp_amount_env.run(attack, decay, 0.5, release);
    float shp_amp_env = m_shp_amp_env.run(attack, decay, 0.5, release);
    float left_input = 0.90 * sin(m_shp_phase) + 0.1 * sin(m_shp_phase2);
    float right_input = 0.90 * sin(m_shp_phase2) + 0.1 * sin(m_shp_phase);
    m_shp_phase += freq * 2 * M_PI * m_inv_rate;
    m_shp_phase2 += freq * 0.999 * 2 * M_PI * m_inv_rate;
    m_shp_phase = (m_shp_phase > 2 * M_PI ? 
		   m_shp_phase - 2 * M_PI : m_shp_phase);
    m_shp_phase2 = (m_shp_phase2 > 2 * M_PI ? 
		    m_shp_phase2 - 2 * M_PI : m_shp_phase2);
    left[i] += shp_amp_env * m_shaper.run(shape * shp_amount_env * left_input, 
					  freq + smoothness * 2000);
    right[i] += shp_amp_env * m_shaper.run(shape* shp_amount_env * right_input,
					   freq + smoothness * 2000);
  }
  
}


void WSVoice::on(unsigned char key, unsigned char velocity) {
  m_shp_amount_env.on();
  m_shp_amp_env.on();
}
  

void WSVoice::off() {
  m_shp_amount_env.off();
  m_shp_amp_env.off();
}


void WSVoice::fast_off() {
  m_shp_amount_env.fast_off();
  m_shp_amp_env.fast_off();
}


void WSVoice::reset() {
  m_shp_phase = m_shp_phase2 = 0;
}


void WSVoice::set_shape(const std::string& str) {
  m_shaper.set_string(str);
}


void WSVoice::set_amount_env(const std::string& str) {
  m_shp_amount_env.set_string(str);
}
  

void WSVoice::set_gain_env(const std::string& str) {
  m_shp_amp_env.set_string(str);
}
