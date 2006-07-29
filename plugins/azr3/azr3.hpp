/****************************************************************************
    
    AZR-3 - An LV2 synth plugin
    
    Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef AZR3_HPP
#define AZR3_HPP

#include <pthread.h>
#include <semaphore.h>

#include "lv2instrument.hpp"
#include "voice_classes.h"
#include "Globals.h"
#include "ringbuffer.hpp"


enum
{
	evt_noteon = 0x90,
	evt_noteoff = 0x80,
	evt_alloff,
	evt_pedal,
	evt_pitch = 0xE0,
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


struct PortChange {
  PortChange(unsigned long p, float v) : port(p), value(v) { }
  unsigned long port;
  float value;
};


class AZR3 : public LV2Instrument {
public:
  
  AZR3(unsigned long rate, const char* bundle_path, const LV2_Host_Feature**);
  
  ~AZR3();
  
  void activate();
  
  void deactivate();
  
  const LV2_ProgramDescriptor* get_program(unsigned long index);
  
  void select_program(unsigned long number);
  
  void run(unsigned long nframes);
  
  char* configure(const char* key, const char* value);
  
protected:  
  
	void setFactorySounds();
	bool make_waveforms(int shape);
	void calc_waveforms(int number);
	void calc_click();
  unsigned char* event_clock(unsigned long offset);
  
  static void* worker_function(void* arg);
  
protected:
  
	notemaster	n1;
	bool	mute;
	float	click[16];
	float	volume[16];
	float	mono_before;
	float	samplerate;
	long	samplecount;
	bool	waitforsplit;
	long	splitpoint;
  
  bool slow_controls[kNumParams];
	float	last_value[kNumParams];
  
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
	delay	vdelay1, vdelay2;
	float	viblfo;
	bool	vibchanged1, vibchanged2;
	float	vmix1, vmix2;
	filt_lp	warmth;

	filt1	fuzz_filt, body_filt, postbody_filt;
	float	fuzz;
  float oldmrvalve;
	bool	odchanged;
  float oldmix;
	float	odmix, n_odmix, n2_odmix, n25_odmix, odmix75;

	float	oldspread, spread, spread2;
	float	cross1;
	float	lspeed, uspeed;
	float	er_r, er_r_before, er_l, er_feedback;
	float	llfo_out, llfo_nout, llfo_d_out, llfo_d_nout;
	float	lfo_out, lfo_nout, lfo_d_out, lfo_d_nout;
	bool	lfos_ok;
	filt1	split;
	filt1	horn_filt, damp;
	delay	wand_r, wand_l, delay1, delay2, delay3, delay4;
	lfo		lfo1, lfo2, lfo3, lfo4;

	int			last_shape;
	float		last_r, last_l;

	filt_allpass	allpass_l[4], allpass_r[4];
  
  unsigned char* midi_ptr;
  
  LV2_ProgramDescriptor pdesc;
  
  pthread_mutex_t m_notemaster_lock;
  Ringbuffer<PortChange, 1024> m_queue;
  sem_t m_qsem;
  pthread_t m_worker;
};


#endif
