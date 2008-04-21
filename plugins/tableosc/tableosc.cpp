/****************************************************************************
    
    Table oscillator LV2 plugin
    
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

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <lv2plugin.hpp>
#include <samplerate.h>
#include <sndfile.h>

#include "tableosc.peg"


using namespace std;
using namespace LV2;


class TableOsc : public Plugin<TableOsc, SaveRestore<false> > {
public:
  
  static const uint32_t TABLE_SIZE = 4096;
  
  TableOsc(double rate)
    : Plugin<TableOsc, SaveRestore<false> >(t_n_ports),
      m_pos(0),
      m_factor(TABLE_SIZE / rate) {
    for (uint32_t i = 0; i < TABLE_SIZE; ++i)
      m_data[i] = static_cast<float>(i) / TABLE_SIZE;
  }
  
  
  void run(uint32_t nframes) {
    float inc = *p(t_freq) * m_factor;
    for (uint32_t i = 0; i < nframes; ++i) {
      float a = m_data[static_cast<uint32_t>(m_pos) % TABLE_SIZE];
      float b = m_data[(static_cast<uint32_t>(m_pos) + 1) % TABLE_SIZE];
      float tmp;
      float r = modff(m_pos, &tmp);
      if (r == 0)
	p(t_output)[i] = a;
      else
	p(t_output)[i] = a * (1 - r) + b * r;
      m_pos += inc;
      if (m_pos >= TABLE_SIZE)
	m_pos -= TABLE_SIZE;
    }
  }
  
  
  char* save(const char* directory, LV2SR_File*** files) {
    if (m_filename.size() > 0) {
      LV2SR_File* file = static_cast<LV2SR_File*>(malloc(sizeof(LV2SR_File)));
      file->name = strdup("table");
      file->path = strdup(m_filename.c_str());
      file->must_copy = 0;
      *files = static_cast<LV2SR_File**>(calloc(2, sizeof(LV2SR_File*)));
      (*files)[0] = file;
    }
    else {
      *files = static_cast<LV2SR_File**>(calloc(1, sizeof(LV2SR_File*)));
    }
    return 0;
  }
  
  
  char* restore(const LV2SR_File** files) {
    
    if (*files) {
      
      if (!strcmp((*files)->name, "table")) {
	
	// open the file
	SF_INFO s_info;
	SNDFILE* s_file = sf_open((*files)->path, SFM_READ, &s_info);
	if (!s_file)
	  return strdup("Could not open data file");
	
	// check that it is sane
	if (s_info.frames == 0 || s_info.samplerate == 0 ||
	    s_info.channels != 1) {
	  sf_close(s_file);
	  return strdup("Could not load data file");
	}
	
	// allocate temporary buffer and read data into it
	vector<float> data(s_info.frames);
	if (sf_readf_float(s_file, &data[0], s_info.frames) != s_info.frames) {
	  sf_close(s_file);
	  return strdup("Reading data failed");
	}
	sf_close(s_file);
	
	// resample the data into the internal table
	if (s_info.frames == TABLE_SIZE)
	  memcpy(m_data, &data[0], sizeof(float) * TABLE_SIZE);
	else {
	  SRC_DATA src = { &data[0], m_data, s_info.frames, TABLE_SIZE, 
			   0, 0, 0, double(TABLE_SIZE) / s_info.frames };
	  int err;
	  if ((err = src_simple(&src, SRC_SINC_BEST_QUALITY, 1)))
	    return strdup(src_strerror(err));
	}
	
      }
      
      else
	return strdup("Unknown file name");
    }
    
    // if there are no files, fill the oscillator table with 0
    else {
      for (uint32_t i = 0; i < TABLE_SIZE; ++i)
	m_data[i] = 0;
    }
    
    return 0;
  }

  
protected:
  
  float m_data[TABLE_SIZE];
  float m_pos;
  float m_factor;
  string m_filename;
  
};


static int _ = TableOsc::register_class(t_uri);
