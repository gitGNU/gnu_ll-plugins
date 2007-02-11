/****************************************************************************
    
    trilobeat.cpp - The plugin part for a MIDI keyboard plugin
    
    Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
    
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

#include <cmath>
#include <cstring>
#include <sstream>
#include <vector>
#include <iostream>

#include "lv2instrument.hpp"
#include "trilobeat.peg"
#include "lv2-midifunctions.h"
#include "monostep.hpp"
#include "frequencytable.hpp"


using namespace std;


class Trilobeat : public LV2Instrument {
public:

  Trilobeat(uint32_t rate, const char* bundle, const LV2_Host_Feature** f)
    : LV2Instrument(k_n_ports),
      m_seq(32),
      m_invrate(1.0 / rate),
      m_rate(rate),
      m_step(0),
      m_playing(128, false) {
    
  }
  
  
  void run(uint32_t nframes) {
    
    unsigned char on[] = { 0x90, 0x00, 0x64 };
    unsigned char off[] = { 0x80, 0x00, 0x64 };
    
    double spm = (*p(k_bpm) < 1 ? 1 : *p(k_bpm)) * 8;
    double fpb = 60 * m_rate / spm;
    double ipart, fpart;
    fpart = modf(m_step, &ipart);
    int step = int(ipart);
    step -= (fpart == 0 ? 1 : 0);
    double next_event = fpart * fpb;
    LV2_MIDIState out = { p<LV2_MIDI>(k_midi_out), nframes, 0 };
    out.midi->size = 0;
    out.midi->event_count = 0;
    
    while (next_event < nframes) {
      ++step;
      step %= 32;
      
      // write events here

      next_event += fpb;
    }
    
    m_step += nframes / fpb;
    m_step -= (m_step >= 32 ? 32 : 0);
  }
  
  
  char* configure(const char* key, const char* value) {
    
    if (!strncmp(key, "seq", 3)) {
      int s = atoi(key + 3);
      if (s < 0 || s > 127)
        return strdup("INCORRECT SEQUENCE NUMBER");
      istringstream iss(value);
      int n;
      iss>>n;
      if (n != 32)
        return strdup("INCORRECT SEQUENCE LENGTH");
      for (int i = 0; i < n; ++i) {
        int on, note, slide, velocity;
        iss>>on>>note>>slide>>velocity;
        m_seq[i].on = on;
        m_seq[i].note = note;
        m_seq[i].slide = slide;
        m_seq[i].velocity = velocity;
      }
      return 0;
    }
    
    return strdup("UNKNOWN KEY");
  }
  
protected:
  
  std::vector<MonoStep> m_seq;
  float m_invrate;
  float m_rate;
  double m_step;
  std::vector<bool> m_playing;
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2_inst<Trilobeat>(k_uri);
}
