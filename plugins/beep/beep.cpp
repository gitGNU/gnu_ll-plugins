/****************************************************************************
    
    beep.cpp - A simple DSSI synth plugin
    
    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>
    
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

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>

#include <lv2synth.hpp>

#include "beep.peg"


using namespace std;


struct BeepVoice : LV2::Voice {
  
  BeepVoice(double rate)
    : m_rate(rate),
      m_phase(0),
      m_freq(0), 
      m_key(LV2::INVALID_KEY) {
  
  }
  
  void on(unsigned char key, unsigned char velocity) {
    m_key = key;
    m_freq = LV2::key2hz(key);
  }
  
  void off(unsigned char velocity) {
    m_key = LV2::INVALID_KEY;
  }
  
  unsigned char get_key() const {
    return m_key;
  }
  
  void render(uint32_t from, uint32_t to) {
    if (m_key != LV2::INVALID_KEY) {
      for (uint32_t i = from; i < to; ++i) {
	m_phase = m_phase > 0.3 ? -0.3 : m_phase;
	p(b_output)[i] += m_phase;
	m_phase += m_freq / m_rate;
      }
    }
  }
  
protected:
  
  float m_rate;
  float m_phase;
  float m_freq;
  unsigned char m_key;

};


class Beep : public LV2::Synth<BeepVoice, Beep, LV2::SaveRestoreExt<true> > {
public:
  
  Beep(double rate)
    : LV2::Synth<BeepVoice, Beep, LV2::SaveRestoreExt<true> >(b_n_ports, b_midi) {
    add_voices(new BeepVoice(rate),
	       new BeepVoice(rate),
	       new BeepVoice(rate),
	       new BeepVoice(rate),
	       new BeepVoice(rate),
	       new BeepVoice(rate));
    add_audio_outputs(b_output);
    
    srand(time(NULL));
    m_state = rand();
    cerr<<"Generated random state: "<<m_state<<endl;
    
  }
  
  
  char* save(const char* directory, LV2SR_File*** files) {
    string str(directory);
    str += "/beep-XXXXXX";
    char* str2 = strdup(str.c_str());
    int fd = mkstemp(str2);
    if (fd < 0) {
      free(str2);
      return strdup("Could not create file");
    }
    FILE* f = fdopen(fd, "w");
    fprintf(f, "%d\n", m_state);
    fclose(f);
    cerr<<"Wrote state "<<m_state<<" to file"<<endl;
    LV2SR_File* data = static_cast<LV2SR_File*>(malloc(sizeof(LV2SR_File)));
    data->name = strdup("state");
    data->path = str2;
    data->must_copy = 1;
    *files = static_cast<LV2SR_File**>(calloc(2, sizeof(LV2SR_File*)));
    (*files)[0] = data;
    (*files)[1] = 0;
    return 0;
  }
  
  char* restore(const LV2SR_File** files) {
    cerr<<__PRETTY_FUNCTION__<<endl;
    if (*files) {
      if (!strcmp((*files)->name, "state")) {
	ifstream ifs((*files)->path);
	ifs>>m_state;
	cerr<<"Read state "<<m_state<<" from file"<<endl;
	return 0;
      }
      return strdup("Unknown file name");
    }
    return strdup("Empty file list");
  }
  
protected:
  
  int m_state;
  
};


static unsigned _ = Beep::register_class(b_uri);
