/****************************************************************************
    
    Convolution reverb plugin, based on Fons Adriaensen's JConv
    
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

#include "convolver.peg"
#include "zita-convolver.h"


using namespace std;
using namespace LV2;


class Convolver : 
  public Plugin<Convolver, FixedP2BufSize<true>, SaveRestore<false> > {
public:
  
  Convolver(double rate)
    : Plugin<Convolver, FixedP2BufSize<true>, SaveRestore<false> >(c_n_ports) {
    
  }
  
  
  void run(uint32_t nframes) {
    memset(p(c_left_output), 0, nframes * sizeof(float));
    memset(p(c_right_output), 0, nframes * sizeof(float));
  }
  
  
  char* save(const char* directory, LV2SR_File*** files) {
    return strdup("save() not implemented yet");
  }
  
  
  char* restore(const LV2SR_File** files) {
    return strdup("restore() not implemented yet");
  }

  
protected:

  Convproc m_conv;
  
};


static int _ = Convolver::register_class(c_uri);
