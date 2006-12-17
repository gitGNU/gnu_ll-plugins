/****************************************************************************
    
    trilobyte.cpp - The plugin part for a MIDI keyboard plugin
    
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

#include "lv2plugin.hpp"
#include "trilobyte.peg"
#include "lv2-midifunctions.h"


class Trilobyte : public LV2Plugin {
public:

  Trilobyte(uint32_t rate, const char* bundle, const LV2_Host_Feature** f)
    : LV2Plugin(k_n_ports) {
    
  }
  
  
  void run(uint32_t nframes) {

  }
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2<Trilobyte>(k_uri);
}
