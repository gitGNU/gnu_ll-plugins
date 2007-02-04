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

#ifndef DISTORTION_HPP
#define DISTORTION_HPP

#include <stdint.h>

#define DENORMALIZE(fv) (fv<.00000001f && fv>-.00000001f)?0:(fv)
#define	PI	3.14159265358979323846f


class filt1 {
  friend class filt_lp;
public:
  filt1();
  ~filt1(){};
  float	lp();
  float	bp();
  float	hp();
  void	clock(float input);
  void	setparam(float cutoff, float q, float samplerate);
  void	set_samplerate(float samplerate);
private:
  float	fs;		// sampling freq
  float	fc;		// cutoff freq
  float	q;		// resonance
  float	m_f,m_q,m_qnrm;
  float	m_h;	// hp out
  float	m_b;	// bp out
  float	m_l;	// lp out
};


class filt_lp : public filt1 {
public:
  filt_lp();
  float	clock(float input);
};


class Distortion {
public:
  
  Distortion(uint32_t rate);
  ~Distortion();
  
  void run(float* left, float* right, uint32_t nframes, float toggle,
	   float drive, float set, float tone, float mix);
  
private:
  
  uint32_t m_rate;
  
  float oldmrvalve;
  bool odchanged;
  filt1 fuzz_filt_l;
  filt1 body_filt_l;
  filt1 postbody_filt_l;
  filt_lp warmth_l;
  filt1 fuzz_filt_r;
  filt1 body_filt_r;
  filt1 postbody_filt_r;
  filt_lp warmth_r;
  float oldmix;
  float odmix;
  uint32_t samplecount;
  float n_odmix;
  float n2_odmix;
  float odmix75;
  float n25_odmix;
  
};


#endif
