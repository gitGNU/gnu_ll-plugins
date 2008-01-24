/****************************************************************************
    
    rudolf556.cpp - Simple drum machine plugin
    
    Copyright (C) 2008 Lars Luthman <lars.luthman@gmail.com>
    
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

#include <cstring>

#include <sndfile.h>

#include "lv2synth.hpp"
#include "rudolf556.peg"


using namespace std;


class Voice : public LV2::Voice {
public:
  
  enum VoiceType {
    Bass = 0,
    Snare = 1,
    Hihat = 2
  };
  
  Voice(VoiceType type, int number, unsigned quickfade_max,
	float* buf1, unsigned len1, float* buf2, unsigned len2, 
	float* buf3 = 0, unsigned len3 = 0)
    : m_key(LV2::INVALID_KEY),
      m_frame(0),
      m_last_sample(0.0),
      m_quickfade(0),
      m_quickfade_max(quickfade_max),
      m_type(type),
      m_number(number - 1),
      m_buf1(buf1),
      m_buf2(buf2),
      m_buf3(buf3),
      m_len1(len1),
      m_len2(len2),
      m_len3(len3) {
    
  }
  
  void on(unsigned char key, unsigned char velocity) {
    
    // if we are already playing a note we need to fade it out really quick
    // before starting this one to avoid clicks
    // don't do it for hihats, they are noisy enough to cover the clicks
    if (m_type != Hihat && m_key != LV2::INVALID_KEY) {
      m_quickfade = m_quickfade_max;
    }
    
    m_key = key;
    m_frame = 0;
    
    // copy the current parameters (we keep them constant for the duration 
    // of this drum hit)
    m_l = *p(1 + 6 * m_type + 3 * m_number + 0);
    m_l = m_l < 0 ? 0 : m_l;
    m_l = m_l > 1 ? 1 : m_l;
    m_h = *p(1 + 6 * m_type + 3 * m_number + 1);
    m_h = m_h < 0 ? 0 : m_h;
    m_h = m_h > 1 ? 1 : m_h;
    m_v = *p(1 + 6 * m_type + 3 * m_number + 2);
    m_v = m_v < 0 ? 0 : m_v;
    m_v = m_v > 1 ? 1 : m_v;
    
    if (m_type == Bass) {
      float e = (pow(100, 0.8 * m_l) - 1) / 99;
      m_decay_start = (0.02 + 0.98 * e) * m_len1;
      m_end = m_decay_start + (0.02 + 0.98 * e) * m_len1;
      m_end = m_end < m_len1 ? m_end : m_len1;
    }
    else if (m_type == Snare) {
      float e = (pow(50, m_l) - 1) / 49;
      m_decay_start = (0.02 + 0.98 * e) * m_len1;
      m_end = m_decay_start + (0.02 + 0.98 * e) * m_len1;
      m_end = m_end < m_len1 ? m_end : m_len1;
    }
    else {
      float e = (pow(100, 0.8 * m_l) - 1) / 99;
      m_decay_start = (0.02 + 0.98 * e) * m_len1;
      m_end = m_decay_start + (0.02 + 0.98 * e) * m_len1;
      m_end = m_end < m_len1 ? m_end : m_len1;
    }
  }
  
  unsigned char get_key() const {
    return m_key;
  }
  
  void render(uint32_t from, uint32_t to) {
    
    // if we're not playing a note, just return
    if (m_key == LV2::INVALID_KEY)
      return;
    
    // if we are doing a quickfade from last note, finish that first
    // it might be nicer to do a BLEP here, but a linear ramp seems to work OK
    if (m_quickfade) {
      uint32_t n = from + m_quickfade;
      n = n > to ? to : n;
      for (uint32_t i = from; i < n; ++i, --m_quickfade)
	p(r_output_mix)[i] += (m_last_sample * m_quickfade) / m_quickfade_max;
      from += n;
    }
    
    // different drum types use slightly different algorithms
    switch (m_type) {
      
    case Bass: {
      
      /* The bass drum uses three samples of different "hardness" and 
	 mixes two of them depending on whether the hardness parameter is 
	 smaller or larger than 0.5. It also uses a fadeout with onset and 
	 length depending on the current length parameter.
      */
      
      // choose the right pair of sample buffers to mix
      float h;
      float* buf1;
      float* buf2;
      if (m_h < 0.5) {
	h = 2 * m_h;
	buf1 = m_buf1;
	buf2 = m_buf2;
      }
      else {
	h = 2 * m_h - 1;
	buf1 = m_buf2;
	buf2 = m_buf3;
      }
      
      // write the audio to the output port
      uint32_t n, f0;
      if (m_frame < m_decay_start) {
	uint32_t n = m_decay_start;
	uint32_t f0 = m_frame;
	if (n - m_frame > to - from)
	  n = m_frame + to - from;
	for ( ; m_frame < n; ++m_frame) {
	  m_last_sample = 
	    0.6 * m_v * ((1-h) * buf1[m_frame] + h * buf2[m_frame]);
	  p(r_output_mix)[from + m_frame - f0] += m_last_sample;
	}
	from += n - f0;
      }
      n = m_end;
      f0 = m_frame;
      if (n - m_frame > to - from)
	n = m_frame + to - from;
      for ( ; m_frame < n; ++m_frame) {
	float s = 1 - (m_frame - m_decay_start) / float(m_end - m_decay_start);
	m_last_sample = 
	  s * 0.6 * m_v * ((1-h) * buf1[m_frame] + h * buf2[m_frame]);
	p(r_output_mix)[from + m_frame - f0] += m_last_sample;
      }
      
      // if we're done with this note, reset
      if (m_frame >= m_end) {
	m_frame = 0;
	m_key = LV2::INVALID_KEY;
      }
      
      break;
    }
      
    case Snare: {
      
      /* The snare drum uses two samples, one for the "drum" part (pretty
	 much a sped up bass drum) and one for the "snare" part (a noise burst).
	 The volume of the snare depends on the hardness parameter. It has
	 a fadeout similar to the bass drum.
      */

      uint32_t n, f0;
      if (m_frame < m_decay_start) {
	uint32_t n = m_decay_start;
	uint32_t f0 = m_frame;
	if (n - m_frame > to - from)
	  n = m_frame + to - from;
	for ( ; m_frame < n; ++m_frame) {
	  m_last_sample = 
	    0.6 * m_v * (m_buf1[m_frame] + m_h * 0.2 * m_buf2[m_frame]);
	  p(r_output_mix)[from + m_frame - f0] += m_last_sample;
	}
	from += n - f0;
      }
      n = m_end;
      f0 = m_frame;
      if (n - m_frame > to - from)
	n = m_frame + to - from;
      for ( ; m_frame < n; ++m_frame) {
	float s = 1 - (m_frame - m_decay_start) / float(m_end - m_decay_start);
	m_last_sample = 
	  s * 0.6 * m_v * (m_buf1[m_frame] + m_h * 0.2 * m_buf2[m_frame]);
	p(r_output_mix)[from + m_frame - f0] += m_last_sample;
      }

      if (m_frame >= m_end) {
	m_frame = 0;
	m_key = LV2::INVALID_KEY;
      }
      break;
    }
      
    case Hihat: {
      
      /* The hihat uses two samples, one "base" which is a long hihat hit and
	 one "hit" which is a much shorter version of the same sound. The
	 volume of the hit depends on the hardness parameter. It has a fadeout
	 similar to the bass drum.
      */

      uint32_t n, f0;
      if (m_frame < m_decay_start) {
	uint32_t n = m_decay_start;
	uint32_t f0 = m_frame;
	if (n - m_frame > to - from)
	  n = m_frame + to - from;
	for ( ; m_frame < n; ++m_frame) {
	  float v = m_buf1[m_frame] + m_h * m_buf2[m_frame];
	  v = (fabs(v + 1) - fabs(v - 1)) * 0.5;
	  m_last_sample = 0.3 * m_v * v;
	  p(r_output_mix)[from + m_frame - f0] += m_last_sample;
	}
	from += n - f0;
      }
      n = m_end;
      f0 = m_frame;
      if (n - m_frame > to - from)
	n = m_frame + to - from;
      for ( ; m_frame < n; ++m_frame) {
	float v = m_buf1[m_frame] + m_h * m_buf2[m_frame];
	v = (fabs(v + 1) - fabs(v - 1)) * 0.5;
	float s = 1 - (m_frame - m_decay_start) / float(m_end - m_decay_start);
	m_last_sample = s * 0.3 * m_v * v;
	p(r_output_mix)[from + m_frame - f0] += m_last_sample;
      }

      if (m_frame >= m_end) {
	m_frame = 0;
	m_key = LV2::INVALID_KEY;
      }
      break;
    }
      
    }
    
  }
  

protected:
  
  unsigned char m_key;
  unsigned m_frame;
  unsigned m_decay_start;
  unsigned m_end;
  float m_last_sample;
  unsigned m_quickfade;
  unsigned m_quickfade_max;
  
  VoiceType m_type;
  int m_number;
  float* m_buf1;
  float* m_buf2;
  float* m_buf3;
  unsigned m_len1;
  unsigned m_len2;
  unsigned m_len3;
  float m_l;
  float m_h;
  float m_v;
  
};


class Rudolf556 : public LV2::Synth<Voice, Rudolf556> {
public:


  Rudolf556(double rate) 
    : LV2::Synth<Voice, Rudolf556>(r_n_ports, r_midi_input),
      m_frame(0) {
    
    m_bass_h00_info.format = 0;
    SNDFILE* snd1 = sf_open((string(bundle_path()) + "bass_h00.flac").c_str(), 
			   SFM_READ, &m_bass_h00_info);
    m_bass_h05_info.format = 0;
    SNDFILE* snd2 = sf_open((string(bundle_path()) + "bass_h05.flac").c_str(), 
			   SFM_READ, &m_bass_h05_info);
    m_bass_h10_info.format = 0;
    SNDFILE* snd3 = sf_open((string(bundle_path()) + "bass_h10.flac").c_str(), 
			   SFM_READ, &m_bass_h10_info);
    m_snare_bonk_info.format = 0;
    SNDFILE* snd4 = sf_open((string(bundle_path()) + "snare_bonk.flac").c_str(), 
			   SFM_READ, &m_snare_bonk_info);
    m_snare_noise_info.format = 0;
    SNDFILE* snd5 = sf_open((string(bundle_path()) +"snare_noise.flac").c_str(), 
			    SFM_READ, &m_snare_noise_info);
    m_hihat_base_info.format = 0;
    SNDFILE* snd6 = sf_open((string(bundle_path()) + "hihat_base.flac").c_str(), 
			    SFM_READ, &m_hihat_base_info);
    m_hihat_hit_info.format = 0;
    SNDFILE* snd7 = sf_open((string(bundle_path()) + "hihat_hit.flac").c_str(), 
			    SFM_READ, &m_hihat_hit_info);

    if (!(snd1 && snd2 && snd3 && snd4 && snd5 && snd6 && snd7)) {
      set_ok(false);
      return;
    }
    
    m_bass_h00 = new float[m_bass_h00_info.frames];
    sf_read_float(snd1, m_bass_h00, m_bass_h00_info.frames);
    m_bass_h05 = new float[m_bass_h05_info.frames];
    sf_read_float(snd2, m_bass_h05, m_bass_h05_info.frames);
    m_bass_h10 = new float[m_bass_h10_info.frames];
    sf_read_float(snd3, m_bass_h10, m_bass_h10_info.frames);
    m_snare_bonk = new float[m_snare_bonk_info.frames];
    sf_read_float(snd4, m_snare_bonk, m_snare_bonk_info.frames);
    m_snare_noise = new float[m_snare_noise_info.frames];
    sf_read_float(snd5, m_snare_noise, m_snare_noise_info.frames);
    m_hihat_base = new float[m_hihat_base_info.frames];
    sf_read_float(snd6, m_hihat_base, m_hihat_base_info.frames);
    m_hihat_hit = new float[m_hihat_hit_info.frames];
    sf_read_float(snd7, m_hihat_hit, m_hihat_hit_info.frames);
    
    add_voices(new Voice(Voice::Bass, 1, 64,
			 m_bass_h00, m_bass_h00_info.frames,
			 m_bass_h05, m_bass_h05_info.frames,
			 m_bass_h10, m_bass_h10_info.frames),
	       new Voice(Voice::Bass, 2, 64,
			 m_bass_h00, m_bass_h00_info.frames,
			 m_bass_h05, m_bass_h05_info.frames,
			 m_bass_h10, m_bass_h10_info.frames),
	       new Voice(Voice::Snare, 1, 64,
			 m_snare_bonk, m_snare_bonk_info.frames,
			 m_snare_noise, m_snare_noise_info.frames),
	       new Voice(Voice::Snare, 2, 64,
			 m_snare_bonk, m_snare_bonk_info.frames,
			 m_snare_noise, m_snare_noise_info.frames),
	       new Voice(Voice::Hihat, 1, 64,
			 m_hihat_base, m_hihat_base_info.frames,
			 m_hihat_hit, m_hihat_hit_info.frames),
	       new Voice(Voice::Hihat, 2, 64,
			 m_hihat_base, m_hihat_base_info.frames,
			 m_hihat_hit, m_hihat_hit_info.frames));

    add_audio_outputs(r_output_mix);
  }

  unsigned find_free_voice(unsigned char key, unsigned char velocity) {
    static unsigned NO = 1024;
    static unsigned voice_map[] = { 0, NO, 1, NO, 2, 3, NO, 4, NO, 5, NO, NO };
    return voice_map[key % 12];
  }
  
protected:
  
  float* m_bass_h00;
  float* m_bass_h05;
  float* m_bass_h10;
  float* m_snare_bonk;
  float* m_snare_noise;
  float* m_hihat_base;
  float* m_hihat_hit;
  
  SF_INFO m_bass_h00_info;
  SF_INFO m_bass_h05_info;
  SF_INFO m_bass_h10_info;
  SF_INFO m_snare_bonk_info;
  SF_INFO m_snare_noise_info;
  SF_INFO m_hihat_base_info;
  SF_INFO m_hihat_hit_info;
  
  uint32_t m_frame;
  
};


static unsigned _ = Rudolf556::register_class(r_uri);
