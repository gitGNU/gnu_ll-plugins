/****************************************************************************
    
    pdvoice.cpp - a phase distortion synth voice
    
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

#include "pdvoice.hpp"

  
PDVoice::PDVoice(uint32_t rate)
  : m_pdosc(rate),
    m_pd_dist_env(rate),
    m_pd_amp_env(rate) {

}


PDVoice::~PDVoice() {

}


void PDVoice::run(float* left, float* right, uint32_t nframes) {

}

  
void PDVoice::on(unsigned char key, unsigned char velocity) {
  m_pd_dist_env.on();
  m_pd_amp_env.on();
}

  
void PDVoice::off() {
  m_pd_dist_env.off();
  m_pd_amp_env.off();
}

  
void PDVoice::fast_off() {
  m_pd_dist_env.fast_off();
  m_pd_amp_env.fast_off();
}

  
void PDVoice::reset() {

}


void PDVoice::set_phase(const std::string& str) {
  m_pdosc.set_string(str);
}

  
void PDVoice::set_dist_env(const std::string& str) {
  m_pd_dist_env.set_string(str);
}

  
void PDVoice::set_gain_env(const std::string& str) {
  m_pd_amp_env.set_string(str);
}

  

