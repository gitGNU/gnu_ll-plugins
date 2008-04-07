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
#include <lv2_osc.h>

#include "beep.peg"


using namespace std;
using namespace LV2;


struct BeepVoice : Voice {
  
  BeepVoice(double rate)
    : m_rate(rate),
      m_phase(0),
      m_freq(0), 
      m_key(INVALID_KEY) {
  
  }
  
  void on(unsigned char key, unsigned char velocity) {
    m_key = key;
    m_freq = key2hz(key);
  }
  
  void off(unsigned char velocity) {
    m_key = INVALID_KEY;
  }
  
  unsigned char get_key() const {
    return m_key;
  }
  
  void render(uint32_t from, uint32_t to) {
    if (m_key != INVALID_KEY) {
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


class Beep : public Synth<BeepVoice, Beep, SaveRestore<true>, MsgContext<true> > {
public:
  
  Beep(double rate)
    : Synth<BeepVoice, Beep, SaveRestore<true>, MsgContext<true> >(b_n_ports, b_midi) {
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
  
  bool blocking_run(uint8_t* outputs_written) { 
    LV2_Event_Iterator iter;
    lv2_event_begin(&iter, p<LV2_Event_Buffer>(b_msg_input));
    cerr<<"Message context woken up with these events:"<<endl;
    while (lv2_event_is_valid(&iter)) {
      uint8_t* data;
      LV2_Event* ev = lv2_event_get(&iter, &data);
      if (ev->type == 1) {
	cerr<<" MIDI:"<<hex;
	for (uint32_t i = 0; i < ev->size; ++i)
	  cerr<<" 0x"<<int(data[i]);
	cerr<<dec<<endl;
      }
      else if (ev->type == 2) {
	const LV2_OSC_Event* osc = reinterpret_cast<LV2_OSC_Event*>(data);
	const char* types = lv2_osc_get_types(osc);
	const char* path = lv2_osc_get_path(osc);
	cerr<<"  OSC: \""<<path<<"\" "<<types;
	for (uint32_t i = 0; i < strlen(types); ++i) {
	  switch (types[i]) {
	  case 'i': cerr<<" "<<lv2_osc_get_argument(osc, i)->i; break;
	  case 'f': cerr<<" "<<lv2_osc_get_argument(osc, i)->f; break;
	  case 's': cerr<<" \""<<&lv2_osc_get_argument(osc, i)->s<<"\""; break;
	  case 'b': cerr<<" <BLOB>"; break;
	  default: cerr<<" <\?\?\?>";
	  }
	}
	cerr<<endl;
      }
      lv2_event_increment(&iter);
    }
    return false;
  }

  
protected:
  
  int m_state;
  
};


static unsigned _ = Beep::register_class(b_uri);
