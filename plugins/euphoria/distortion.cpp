/****************************************************************************
    
    distortion.cpp - A distortion effect
    
    Copyright (C) 2007  Lars Luthman <lars.luthman@gmail.com>

    This distortion effect is based on source code from the VST plugin 
    AZR-3, (C) 2006 Philipp Mott
        

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

#include <cmath>

#include "distortion.hpp"


filt1::filt1()
  : m_l(0),
    m_h(0),
    m_b(0),
    m_f(0),
    q(0) {

}


void filt1::clock(float input) {
  float in;
  in = DENORMALIZE(input);
  m_l = DENORMALIZE(m_l);
  m_b = DENORMALIZE(m_b);
        
  m_h = in - m_l - q * m_b;
  m_b += m_f * m_h;
  m_l += m_f * m_b;
}


float filt1::lp() {
  return m_l;
}


float filt1::bp() {
  return m_b;
}


float filt1::hp() {
  return m_h;
}


void filt1::set_samplerate(float samplerate) {
  fs = samplerate;
  m_l = m_h = m_b = 0;
  setparam(fc, q, fs);
}


void filt1::setparam(float cutoff, float mq, float samplerate) {
  fc = cutoff;
  q = mq;
  fs = samplerate;
  m_f = 2.0f * sinf(PI * fc / fs);
}


filt_lp::filt_lp() 
  : filt1() {

}


float filt_lp::clock(float input) {
  float in;
  in = DENORMALIZE(input);
  m_l = DENORMALIZE(m_l);
  m_b = DENORMALIZE(m_b);
        
  m_h = in - m_l - q * m_b;
  m_b += m_f * m_h;
  m_l += m_f * m_b;

  return m_l;
}

  
Distortion::Distortion(uint32_t rate) 
  : m_rate(rate),
    oldmrvalve(0),
    odchanged(false),
    oldmix(0),
    odmix(0),
    samplecount(0),
    n_odmix(0),
    n2_odmix(0),
    odmix75(0),
    n25_odmix(0) {

  warmth_l.setparam(2700, 1.2f, m_rate);
  body_filt_l.setparam(190, 1.5f, m_rate);
  postbody_filt_l.setparam(1100, 1.5f, m_rate);
  warmth_r.setparam(2700, 1.2f, m_rate);
  body_filt_r.setparam(190, 1.5f, m_rate);
  postbody_filt_r.setparam(1100, 1.5f, m_rate);

}


Distortion::~Distortion() {

}


void Distortion::run(float* left, float* right, uint32_t nframes, float toggle,
                     float drive, float set, float tone, float mix) {

  // has the distortion switch changed?
  if (toggle != oldmrvalve) {
    odchanged = true;
    oldmrvalve = toggle;
  }
  
  // compute distortion parameters
  float value = drive;
  bool do_dist;
  if (value > 0)
    do_dist = true;
  else
    do_dist = false;
  float dist = 2 * (0.1f + value);
  float sin_dist = sinf(dist);
  float i_dist = 1 / dist;
  float dist4 = 4 * dist;
  float dist8 = 8 * dist;
  fuzz_filt_l.setparam(800 + tone * 3000, 0.7f, m_rate);
  fuzz_filt_r.setparam(800 + tone * 3000, 0.7f, m_rate);
  value = mix;
  if (value != oldmix) {
    odmix = value;
    if (toggle > 0)
      odchanged = true;
  }
  
  for (uint32_t i = 0; i < nframes; ++i) {
    
    // smoothing of OD switch
    if(odchanged && samplecount % 10 == 0) {
      if(toggle > 0) {
        odmix += 0.05f;
        if (odmix >= mix)
          odchanged = false;
      }
      else {
        odmix -= 0.05f;
        if (odmix <= 0)
          odchanged = false;
      }
      n_odmix = 1 - odmix;
      n2_odmix = 2 - odmix;
      odmix75 = 0.75f * odmix;
      n25_odmix = n_odmix * 0.25f;
    }
    
    // Mr. Valve
    /*
      Completely rebuilt.
      Multiband distortion:
      The first atan() waveshaper is applied to a lower band. The second
      one is applied to the whole spectrum as a clipping function (combined
      with an fabs() branch).
      The "warmth" filter is now applied _after_ distortion to flatten
      down distortion overtones. It's only applied with activated distortion
      effect, so we can switch warmth off and on without adding another 
      parameter.
    */
    if (toggle > 0 || odchanged) {

      // left channel
      float mono = left[i];
      if (do_dist) {
        body_filt_l.clock(mono);
        postbody_filt_l.clock(atanf(body_filt_l.lp() * dist8) * 6);
        float fuzz = atanf(mono * dist4) * 0.25f + 
          postbody_filt_l.bp() + postbody_filt_l.hp();
        
        if (std::abs(mono) > set)
          fuzz = atanf(fuzz * 10);
        fuzz_filt_l.clock(fuzz);
        mono = ((fuzz_filt_l.lp() * odmix * sin_dist + mono * (n2_odmix)) * 
                sin_dist) * i_dist;
      }
      else {
        fuzz_filt_l.clock(mono);
        mono = fuzz_filt_l.lp() * odmix75 + mono * n25_odmix * i_dist;
      }
      mono = warmth_l.clock(mono);      
      left[i] = mono;
      
      // right channel
      mono = right[i];
      if (do_dist) {
        body_filt_r.clock(mono);
        postbody_filt_r.clock(atanf(body_filt_r.lp() * dist8) * 6);
        float fuzz = atanf(mono * dist4) * 0.25f + 
          postbody_filt_r.bp() + postbody_filt_r.hp();
        
        if (std::abs(mono) > set)
          fuzz = atanf(fuzz * 10);
        fuzz_filt_r.clock(fuzz);
        mono = ((fuzz_filt_r.lp() * odmix * sin_dist + mono * (n2_odmix)) * 
                sin_dist) * i_dist;
      }
      else {
        fuzz_filt_r.clock(mono);
        mono = fuzz_filt_r.lp() * odmix75 + mono * n25_odmix * i_dist;
      }
      mono = warmth_r.clock(mono);
      right[i] = mono;

    }
    
    ++samplecount;
  }

}

