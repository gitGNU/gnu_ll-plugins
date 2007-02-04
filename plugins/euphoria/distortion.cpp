#include <cmath>

#include "distortion.hpp"

  
filt1::filt1() {
  m_l=m_h=m_b=m_f=q=0;
}

void filt1::clock(float input) {
  float	in;
  in=DENORMALIZE(input);
  m_l=DENORMALIZE(m_l);
  m_b=DENORMALIZE(m_b);
  
  m_h = in - m_l - q*m_b;
  m_b += m_f*m_h;
  m_l += m_f*m_b;
}


float filt1::lp() {
  return(m_l);
}


float filt1::bp() {
  return(m_b);
}


float filt1::hp() {
  return(m_h);
}


void filt1::set_samplerate(float samplerate) {
  fs=samplerate;
  m_l=m_h=m_b=0;
  setparam(fc,q,fs);
}


void filt1::setparam(float cutoff,float mq,float samplerate) {
  fc=cutoff;
  q=mq;
  fs=samplerate;
  m_f=2.0f*sinf(PI*fc/fs);
}


Distortion::Distortion(uint32_t rate) 
  : m_rate(rate),
    oldmix(0),
    odmix(0),
    odchanged(false) {

}


Distortion::~Distortion() {

}


void Distortion::run(float* left, float* right, uint32_t nframes, float drive,
		     float set, float tone, float mix) {

  // compute distortion parameters
  float value = drive;
  bool do_dist;
  if (value > 0)
    do_dist = true;
  else
    do_dist = false;
  float dist = 2 * (0.1f + value);
  float sin_dist = std::sin(dist);
  float i_dist = 1 / dist;
  float dist4 = 4 * dist;
  float dist8 = 8 * dist;
  fuzz_filt.setparam(800 + tone * 3000, 0.7f, m_rate);
  value = mix;
  if (value != oldmix) {
    odmix = value;
    odchanged = true;
  }
  
  
  if (do_dist) {
    for (uint32_t i = 0; i < nframes; ++i) {
      body_filt.clock(mono);
      postbody_filt.clock(atanf(body_filt.lp() * dist8) * 6);
      fuzz = atanf(mono * dist4) * 0.25f + 
	postbody_filt.bp() + postbody_filt.hp();
      
      if (_fabsf(mono) > *p(n_set))
	fuzz = atanf(fuzz * 10);
      fuzz_filt.clock(fuzz);
      mono = ((fuzz_filt.lp() * odmix * sin_dist + mono * (n2_odmix)) * 
	      sin_dist) * i_dist;
    }
    else {
      fuzz_filt.clock(mono);
      mono = fuzz_filt.lp() * odmix75 + mono * n25_odmix * i_dist;
    }
  }
  mono = warmth.clock(mono);

}

