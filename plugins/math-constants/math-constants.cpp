/****************************************************************************
    
    math-constants.cpp - LV2 plugins producing constant output for some common
                         constants from the C math library
    
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
#include <iostream>

#include "lv2plugin.hpp"


namespace {
  
  // This is needed because floats can't be used as template parameters,
  // so we need to use references
  float e = M_E;
  float log2e = M_LOG2E;
  float log10e = M_LOG10E;
  float ln2 = M_LN2;
  float ln10 = M_LN10;
  float pi = M_PI;
  float pi_2 = M_PI_2;
  float pi_4 = M_PI_4;
  float _1_pi = M_1_PI;
  float _2_pi = M_2_PI;
  float _2_sqrtpi = M_2_SQRTPI;
  float sqrt2 = M_SQRT2;
  float sqrt1_2 = M_SQRT1_2;
}

/** This is a template plugin class with a single output port that returns 
    a constant value. */
template <float& Output>
class Constant : public LV2Plugin {
public:
  Constant(uint32_t, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(1) {
    
  }
  void run(uint32_t sample_count) {
    *static_cast<float*>(m_ports[0]) = Output;
  }
};


#define LL_PREFIX "http://ll-plugins.nongnu.org/lv2/dev/math-constant-"


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2< Constant<e> >(LL_PREFIX "e/0.0.0");
  register_lv2< Constant<log2e> >(LL_PREFIX "log2e/0.0.0");
  register_lv2< Constant<log10e> >(LL_PREFIX "log10e/0.0.0");
  register_lv2< Constant<ln2> >(LL_PREFIX "ln2/0.0.0");
  register_lv2< Constant<ln10> >(LL_PREFIX "ln10/0.0.0");
  register_lv2< Constant<pi> >(LL_PREFIX "pi/0.0.0");
  register_lv2< Constant<pi_2> >(LL_PREFIX "pi_2/0.0.0");
  register_lv2< Constant<pi_4> >(LL_PREFIX "pi_4/0.0.0");
  register_lv2< Constant<_1_pi> >(LL_PREFIX "1_pi/0.0.0");
  register_lv2< Constant<_2_pi> >(LL_PREFIX "2_pi/0.0.0");
  register_lv2< Constant<_2_sqrtpi> >(LL_PREFIX"2_sqrtpi/0.0.0");
  register_lv2< Constant<sqrt2> >(LL_PREFIX "sqrt2/0.0.0");
  register_lv2< Constant<sqrt1_2> >(LL_PREFIX "sqrt1_2/0.0.0");
}
