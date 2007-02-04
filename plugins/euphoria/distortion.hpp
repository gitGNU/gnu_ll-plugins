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


class Distortion {
public:
  
  Distortion(uint32_t rate);
  ~Distortion();
  
  void run(float* left, float* right, uint32_t nframes, 
	   float drive, float set, float tone, float mix);
  
private:
  
  uint32_t m_rate;
  float oldmix;
  float odmix;
  bool odchanged;
  filt1 fuzz_filt;
  filt1 body_filt;
  
};


#endif
