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
  
  Voice(VoiceType type, int number, float* buf1, unsigned len1, 
	float* buf2, unsigned len2, float* buf3 = 0, unsigned len3 = 0)
    : m_key(LV2::INVALID_KEY),
      m_frame(0),
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
    m_key = key;
    m_frame = 0;
    m_l = *p(1 + 6 * m_type + 3 * m_number + 0);
    m_l = m_l < 0 ? 0 : m_l;
    m_l = m_l > 1 ? 1 : m_l;
    m_h = *p(1 + 6 * m_type + 3 * m_number + 1);
    m_h = m_h < 0 ? 0 : m_h;
    m_h = m_h > 1 ? 1 : m_h;
    m_v = *p(1 + 6 * m_type + 3 * m_number + 2);
    m_v = m_v < 0 ? 0 : m_v;
    m_v = m_v > 1 ? 1 : m_v;

  }
  
  unsigned char get_key() const {
    return m_key;
  }
  
  void render(uint32_t from, uint32_t to) {
    
    if (m_key == LV2::INVALID_KEY)
      return;
    
    switch (m_type) {

    case Bass: {
      float h;
      uint32_t n = m_len1 - m_frame;
      n = n < to - from ? n : to - from;
      if (m_h < 0.5) {
	h = 2 * m_h;
	for (uint32_t i = 0; i < n; ++i) {
	  p(r_output_mix)[from + i] += 
	    0.6 * m_v * ((1-h) * m_buf1[m_frame + i] + h * m_buf2[m_frame + i]);
	}
      }
      else {
	h = 2 * m_h - 1;
	for (uint32_t i = 0; i < n; ++i) {
	  p(r_output_mix)[from + i] += 
	    0.6 * m_v * ((1-h) * m_buf2[m_frame + i] + h * m_buf3[m_frame + i]);
	}
      }
      m_frame += n;
      if (m_frame >= m_len1) {
	m_frame = 0;
	m_key = LV2::INVALID_KEY;
      }
      break;
    }
      
    case Snare: {
      uint32_t n = m_len1 - m_frame;
      n = n < to - from ? n : to - from;
      for (uint32_t i = 0; i < n; ++i) {
	p(r_output_mix)[from + i] += 
	  0.6 * m_v * (m_buf1[m_frame + i] + m_h * 0.2 * m_buf2[m_frame + i]);
      }
      m_frame += n;
      if (m_frame >= m_len1) {
	m_frame = 0;
	m_key = LV2::INVALID_KEY;
      }
      break;
    }
      
    case Hihat: {
      uint32_t n = m_len1 - m_frame;
      n = n < to - from ? n : to - from;
      for (uint32_t i = 0; i < n; ++i) {
	float v = m_buf1[m_frame + i] + m_h * m_buf2[m_frame + i];
	v = (fabs(v + 1) - fabs(v - 1)) * 0.5;
	p(r_output_mix)[from + i] += 0.3 * m_v * v;
      }
      m_frame += n;
      if (m_frame >= m_len1) {
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
    SNDFILE* snd1 = sf_open((string(bundle_path()) + "bass_h00.wav").c_str(), 
			   SFM_READ, &m_bass_h00_info);
    m_bass_h05_info.format = 0;
    SNDFILE* snd2 = sf_open((string(bundle_path()) + "bass_h05.wav").c_str(), 
			   SFM_READ, &m_bass_h05_info);
    m_bass_h10_info.format = 0;
    SNDFILE* snd3 = sf_open((string(bundle_path()) + "bass_h10.wav").c_str(), 
			   SFM_READ, &m_bass_h10_info);
    m_snare_bonk_info.format = 0;
    SNDFILE* snd4 = sf_open((string(bundle_path()) + "snare_bonk.wav").c_str(), 
			   SFM_READ, &m_snare_bonk_info);
    m_snare_noise_info.format = 0;
    SNDFILE* snd5 = sf_open((string(bundle_path()) +"snare_noise.wav").c_str(), 
			    SFM_READ, &m_snare_noise_info);
    m_hihat_base_info.format = 0;
    SNDFILE* snd6 = sf_open((string(bundle_path()) + "hihat_base.wav").c_str(), 
			    SFM_READ, &m_hihat_base_info);
    m_hihat_hit_info.format = 0;
    SNDFILE* snd7 = sf_open((string(bundle_path()) + "hihat_hit.wav").c_str(), 
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
    
    add_voices(new Voice(Voice::Bass, 1,
			 m_bass_h00, m_bass_h00_info.frames,
			 m_bass_h05, m_bass_h05_info.frames,
			 m_bass_h10, m_bass_h10_info.frames),
	       new Voice(Voice::Bass, 2,
			 m_bass_h00, m_bass_h00_info.frames,
			 m_bass_h05, m_bass_h05_info.frames,
			 m_bass_h10, m_bass_h10_info.frames),
	       new Voice(Voice::Snare, 1,
			 m_snare_bonk, m_snare_bonk_info.frames,
			 m_snare_noise, m_snare_noise_info.frames),
	       new Voice(Voice::Snare, 2,
			 m_snare_bonk, m_snare_bonk_info.frames,
			 m_snare_noise, m_snare_noise_info.frames),
	       new Voice(Voice::Hihat, 1,
			 m_hihat_base, m_hihat_base_info.frames,
			 m_hihat_hit, m_hihat_hit_info.frames),
	       new Voice(Voice::Hihat, 2,
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
