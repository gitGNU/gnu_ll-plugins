/****************************************************************************
    
    lv2instrument.cpp - support file for writing LV2 instruments in C++
    
    Copyright (C) 2006  Lars Luthman <larsl@users.sourceforge.net>
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#include <cassert>

#include "lv2instrument.hpp"


using namespace std;


namespace LV2SupportFunctions {
  
  
  char* configure(LV2_Handle instance, const char* key, const char* value) {
    return reinterpret_cast<LV2Instrument*>(instance)->configure(key, value);
  }
  
  
  unsigned int list_used_files(LV2_Handle instance, 
                               char*** keys, char*** filepaths) {
    return reinterpret_cast<LV2Instrument*>(instance)->
      list_used_files(keys, filepaths);
  }
  
  
  const LV2_ProgramDescriptor* get_program(LV2_Handle instance, 
                                           unsigned long index) {
    return reinterpret_cast<LV2Instrument*>(instance)->get_program(index);
  }
  
  
  void select_program(LV2_Handle instance, unsigned long program) {
    reinterpret_cast<LV2Instrument*>(instance)->select_program(program);
  }


  const LV2_InstrumentDescriptor* get_instrument_descriptor() {
    static LV2_InstrumentDescriptor desc = {
      &configure,
      &list_used_files,
      &get_program,
      &select_program,
    };
    return &desc;
  }
  
  
}
  

