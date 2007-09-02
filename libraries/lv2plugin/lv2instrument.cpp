/****************************************************************************
    
    lv2instrument.cpp - support file for writing LV2 instruments in C++
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#include "lv2instrument.hpp"


using namespace std;


namespace LV2SupportFunctions {
  
  
  char* configure(LV2_Handle instance, const char* key, const char* value) {
    return reinterpret_cast<LV2Instrument*>(instance)->configure(key, value);
  }
  
  char* set_file(LV2_Handle instance, const char* key, const char* filename) {
    return reinterpret_cast<LV2Instrument*>(instance)->set_file(key, filename);
  }

  void* lv2_instrument_descriptor(const char* URI) {
    if (!std::strcmp(URI, "<http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext>")) {
      using namespace LV2SupportFunctions;
      static LV2_InstrumentDescriptor idesc = {
        &configure,
        &set_file,
      };
      return &idesc;
    }
    
    return 0;
  }
  
}
  

