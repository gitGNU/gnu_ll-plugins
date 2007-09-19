/****************************************************************************
    
    audio_identity.cpp - trivial plugin
    
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

#include <cstring>

#include "lv2plugin.hpp"


using namespace std;


/** This is the class that contains all the code and data for the audio
    identity plugin. */
class AudioIdentity : public LV2::Plugin {
public:
  
  AudioIdentity(double, const char*, const LV2_Host_Feature* const*) 
    : LV2::Plugin(2) {
    
  }
  
  
  void run(uint32_t nframes) {
    std::memcpy(p(1), p(0), sizeof(float) * nframes);
  }
  
};


static LV2::Register<AudioIdentity> 
reg("http://ll-plugins.nongnu.org/lv2/dev/audio_identity/0.0.0");
