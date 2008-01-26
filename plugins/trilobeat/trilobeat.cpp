/****************************************************************************
    
    trilobeat.cpp - The plugin part for a MIDI keyboard plugin
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
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

#include <cmath>
#include <cstring>
#include <sstream>
#include <vector>
#include <iostream>

#include "lv2plugin.hpp"
#include "trilobeat.peg"
#include "lv2-midifunctions.h"


using namespace std;


class Trilobeat : public LV2::Plugin<Trilobeat, LV2::CommandExt<true> > {
public:

  Trilobeat(double rate)
    : LV2::Plugin<Trilobeat, LV2::CommandExt<true> >(k_n_ports),
      m_seq(128, std::vector<unsigned char>(s_steps)),
      m_invrate(1.0 / rate),
      m_rate(rate),
      m_step(0),
      m_frame_offset(0),
      m_keyinfo(128) {
    
  }
  
  
  void run(uint32_t nframes) {
    
    unsigned char on[] = { 0x90, 0x00, 0x64 };
    unsigned char off[] = { 0x80, 0x00, 0x00 };
    
    // steps per minute (4 steps per beat)
    double spm = (*p(k_bpm) < 1 ? 1 : *p(k_bpm)) * 4;
    
    // frames per step
    double fps = 60 * m_rate / spm;
    
    // reset MIDI port
    LV2_MIDIState out = { p<LV2_MIDI>(k_midi_out), nframes, 0 };
    out.midi->size = 0;
    out.midi->event_count = 0;
    
    while (m_frame_offset < nframes) {
      ++m_step;
      m_step %= 32;
      for (unsigned char key = 0; key < 128; ++key) {
	if (m_seq[key][m_step]) {
	  off[1] = key;
	  lv2midi_put_event(&out, m_frame_offset, 3, off);
	  on[1] = key;
	  on[2] = m_seq[key][m_step];
	  lv2midi_put_event(&out, m_frame_offset, 3, on);
	}
      }
      m_frame_offset += fps;
    }
    m_frame_offset -= nframes;
  }
  
  
  char* command(uint32_t argc, char const* const* argv) {

    if (argc == 4 && !strncmp(argv[0], "set", 3)) {
      int key = atoi(argv[1]);
      int step = atoi(argv[2]);
      int velocity = atoi(argv[3]);
      if (key < 0 || key > 127)
        return strdup("Invalid key number");
      if (step < 0 || step > s_steps)
        return strdup("Invalid step number");
      if (velocity < 0 || velocity > 127)
        return strdup("Invalid velocity");
      m_seq[key][step] = velocity;
      feedback(argc, argv);
      return 0;
    }
    
    else if (argc == 2 && !strncmp(argv[0], "new_names", 9)) {
      istringstream iss(argv[1]);
      iss>>ws;
      while (!iss.eof()) {
	int key;
	iss>>key;
	string name;
	getline(iss, name);
	iss>>ws;
	if (key >= 0 && key < 128) {
	  // .active is read by the run() thread, but it's just a boolean
	  // and nothing catastrophic will happen if it is misread - it should
	  // be OK to set it without locking
	  m_keyinfo[key].active = true;
	  m_keyinfo[key].name = name;
	  char tmp[4];
	  sprintf(tmp, "%d", key);
	  char const* cmd[] = { "keyname", tmp, name.c_str() };
	  feedback(3, cmd);
	}
	else
	  break;
      }
      return 0;
    }
    
    else if (argc == 4 && !strncmp(argv[0], "velocity", 8)) {
      int key = atoi(argv[1]);
      int step = atoi(argv[2]);
      int velocity = atoi(argv[3]);
      if (key < 0 || key >= 128)
	return strdup("Invalid key number");
      if (step < 0 || step >= s_steps)
	return strdup("Invalid step number");
      if (velocity >= 128)
	return strdup("Invalid velocity number");
      bool changed = m_seq[key][step] != velocity;
      // this is read by the run() thread, but it's just a boolean and 
      // nothing catastrophic will happen if it is misread - it should
      // be OK to set it without locking
      m_seq[key][step] = velocity;
      if (changed)
	feedback(4, argv);
      return 0;
    }
      
    return strdup("Unknown command");
  }
  
protected:
  
  struct KeyInfo {
    KeyInfo(bool a = false, std::string const& n = "") : active(a), name(n) { }
    bool active;
    std::string name;
  };
  
  static unsigned s_steps;
  
  std::vector< std::vector<unsigned char> > m_seq;
  float m_invrate;
  float m_rate;
  unsigned m_step;
  double m_frame_offset;
  std::vector<KeyInfo> m_keyinfo;
};


unsigned Trilobeat::s_steps = 32;


static int _ = Trilobeat::register_class(k_uri);
