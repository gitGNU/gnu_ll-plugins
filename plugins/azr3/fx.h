/****************************************************************************
    
    AZR-3 - An LV2 synth plugin
    
    Copyright (C) 2006 Lars Luthman <lars.luthman@gmail.com>
    
    based on source code from the VST plugin AZR-3, (C) 2006 Philipp Mott
    (well, almost all of it is his code)
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published
    by the Free Software Foundation.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef __FX_h__
#define __FX_h__

#define DENORMALIZE(fv) (fv<.00000001f && fv>-.00000001f)?0:(fv)
#define	PI	3.14159265358979323846f

class delay		// non-interpolating delay
{
public:
	delay(int buflen, bool interpolate);
	~delay();
	void	set_delay(float dtime);
	void	set_samplerate(float samplerate);
	void	flood(float value);
	float	clock(float input);
	void	report();
protected:
	float	*buffer;
	int		p_buflen;
	bool	interp;
	float	offset;
	float	samplerate;
	int		readp,readp2,writep;

	float	outPointer;
	float	alpha,alpha2,alpha3,omAlpha,coeff,lag,lastIn;
	float	ym1;
	float	y0;
	float	y1;
	float	y2;
};

class lfo
{
public:
	lfo(float sr);
	~lfo(){};
	float clock();
	void	set_samplerate(float samplerate);
	void	set_rate(float srate,int type);	// Hz; type: 0=sin, 1=tri
	void	set_phase(float phase);
  void  offset_phase(lfo& l, float phase_offset);
	float	get_rate();
private:
	int		my_type;
	float	output;
	float	samplerate;
	float	inc;
	int		dir;
	float	c,s,ci,si,nc,ns;
	float	my_srate;
};

class filt1
{
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

class filt_lp : public filt1
{
public:
	filt_lp();
	float	clock(float input);
};

class filt_allpass {
 public:
  filt_allpass() : a1(0.f), zm1(0.f) {
    a1=zm1=my_delay=y=0;
  }
    
    void reset() {
      a1=zm1=y=0;
      set_delay(my_delay);
    }
    
    void set_delay(float delay) {
      my_delay=delay;
      a1=(1-delay)/(1+delay);
      a1=DENORMALIZE(a1);
    }
    
    float clock(float input) {
      if(input<.00000001f && input>-.00000001f)	// prevent Pentium FPU Normalizing
	return(0);
      
      y=-a1*input + zm1;
      zm1=y*a1+input;
      return(y);
    }
 private:
    float a1,zm1,my_delay,y;
};

#endif
