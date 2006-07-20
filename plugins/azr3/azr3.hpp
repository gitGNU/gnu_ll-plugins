#ifndef AZR3_HPP
#define AZR3_HPP

#include <pthread.h>

#include "lv2instrument.hpp"
#include "voice_classes.h"
#include "Globals.h"


enum
{
	evt_none=0,
	evt_noteon = 0x90,
	evt_noteoff = 0x80,
	evt_alloff,
	evt_pedal,
	evt_progchange,
	evt_pitch,
	evt_modulation,
	evt_volume,
	evt_channel_volume,
	evt_drawbar,
};


class flpProgram
{
friend class AZR3;
public:
	flpProgram() {};
	~flpProgram() {}
private:
	float	p[kNumParams];
	char name[24];
};


class AZR3 : public LV2Instrument {
public:
  
  AZR3(unsigned long rate, const char* bundle_path, const LV2_Host_Feature**);
  
  ~AZR3();
  
  void activate();
  
  const LV2_ProgramDescriptor* get_program(unsigned long index);
  
  void select_program(unsigned long number);
  
  void run(unsigned long nframes);
  
  char* configure(const char* key, const char* value);
  
protected:  
  
	void setSampleRate(float sampleRate);
	void setFactorySounds();
	bool make_waveforms(int shape);
  void setParameter(long index, float value);
	void calc_waveforms(int number);
	void calc_click();
  unsigned char* event_clock(unsigned long offset);
  
protected:
  
	notemaster	n1;
	bool	mute;
	float	click[16];
	float	volume[16];
	float	mono_before;
	float	*out1,*out2;
	float	samplerate;
	long	samplecount;
	bool	waitforsplit;
	long	splitpoint;

	float	p[kNumParams],*my_p;
	float	last_value[kNumParams];
	float	*p_mono,mono,mono1,mono2;
  
	flpProgram	programs[kNumPrograms];

	float tonewheel[WAVETABLESIZE];	// master waveform

	float sin_16[WAVETABLESIZE];
	float sin_8[WAVETABLESIZE];
	float sin_513[WAVETABLESIZE];
	float sin_4[WAVETABLESIZE];
	float sin_223[WAVETABLESIZE];
	float sin_2[WAVETABLESIZE];
	float sin_135[WAVETABLESIZE];
	float sin_113[WAVETABLESIZE];
	float sin_1[WAVETABLESIZE];

// TABLES_PER_CHANNEL tables per channel; 3 channels; 1 spare table
#define TABLES_PER_CHANNEL	8
	float wavetable[WAVETABLESIZE*TABLES_PER_CHANNEL*3+1];	

	lfo		vlfo;
	delay	vdelay1,vdelay2;
	float	viblfo;
	bool	vibchanged1,vibchanged2,lfo_calced;
	float	vmix1,vmix2;
	filt_lp	warmth;

	filt1	fuzz_filt,body_filt,postbody_filt;
	float	dist,sin_dist,i_dist,dist4,dist8;
	float	fuzz;
	bool	do_dist;
	bool	odchanged;
	float	odmix,n_odmix,n2_odmix,n25_odmix,odmix75;

	float	oldspread,spread,spread2;
	float	cross1;
	bool	fastmode;
	float	lslow,lfast,uslow,ufast,lspeed,uspeed;
	float	ubelt_up,ubelt_down,lbelt_up,lbelt_down;
	float	er_r,er_r_before,er_l,er_feedback;
	float	lp,right,left,lright,lleft,upper,lower,upper_damp;
	float	llfo_out,llfo_nout,llfo_d_out,llfo_d_nout;
	float	lfo_out,lfo_nout,lfo_d_out,lfo_d_nout;
	float	last_out1,last_out2;
	bool	lfos_ok;
	filt1	split;
	filt1	horn_filt,damp;
	delay	wand_r,wand_l,delay1,delay2,delay3,delay4;
	lfo		lfo1,lfo2,lfo3,lfo4;

	float		gp_value;
	int			last_shape;
	float		last_r,last_l;
	unsigned char* evt;

	filt_allpass	allpass_l[4],allpass_r[4];
	float		lfo_phaser1,lfo_phaser2;
  
  unsigned char* midi_ptr;
  
#ifdef DEVELOP
	float		ctrl1,ctrl2,ctrl3,ctrl4;
#endif
  
  LV2_ProgramDescriptor pdesc;
  
  pthread_mutex_t m_lock;
  
  std::vector<bool> is_real_param;
  std::vector<unsigned long> real_param;
};


#endif
