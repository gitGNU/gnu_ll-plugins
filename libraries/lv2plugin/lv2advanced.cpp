/****************************************************************************
    
    lv2advanced.cpp - support file for writing LV2 advanceds in C++
    
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

#include "lv2advanced.hpp"


using namespace std;


char* LV2Advanced::command(LV2_Handle instance, 
			       uint32_t argc, const char* const * argv) {
  return reinterpret_cast<LV2Advanced*>(instance)->command(argc, argv);
}

void* LV2Advanced::lv2_advanced_descriptor(const char* URI) {
  if (!std::strcmp(URI, "<http://ll-plugins.nongnu.org/lv2/namespace#dont-use-this-extension>")) {
    using namespace LV2SupportFunctions;
    static LV2_CommandDescriptor cdesc = {
      &LV2Advanced::command
    };
    return &cdesc;
  }
  
  return 0;
}

  

