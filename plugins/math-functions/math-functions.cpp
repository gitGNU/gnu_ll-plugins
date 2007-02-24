/****************************************************************************
    
    math-functions.cpp - LV2 plugins implementing some common functions
                         from the C math library
    
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


using namespace std;


namespace {
  
  /** A pointer to a function that takes a float parameter and returns 
      a float. */
  typedef float (*unary_f)(float);
  
  /** A pointer to a function that takes two float parameters and returns
      a float. */
  typedef float (*binary_f)(float, float);
  
  // We need all these as variables instead of immediate values since
  // we use them as template parameters - floats are not as allowed as 
  // template parameters but float references are
  float neg1 = -1;
  float pos1 = 1;
  float zero = 0;
  float epsilon = 0.00001;
  
}


template <unary_f F, bool A>
class Unary : public LV2Plugin {
public:
  Unary(uint32_t, const char*, const LV2_Host_Feature* const*) 
    : LV2Plugin(2) {
    
  }
  void run(uint32_t sample_count) {
    float* input = static_cast<float*>(m_ports[0]);
    float* output = static_cast<float*>(m_ports[1]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i)
      output[i] = F(input[i]);
  }
};


template <unary_f F, bool A>
class UnaryGuard : public LV2Plugin {
public:
  UnaryGuard(uint32_t, const char*, const LV2_Host_Feature* const*) 
    : LV2Plugin(2) {
    
  }
  void run(uint32_t sample_count) {
    float* input = static_cast<float*>(m_ports[0]);
    float* output = static_cast<float*>(m_ports[1]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i) {
      output[i] = F(input[i]);
      if (!isnormal(output[i]))
        output[i] = 0;
    }
  }
};


template <unary_f F, bool A, float& MIN, float& MAX>
class UnaryRange : public LV2Plugin {
public:
  UnaryRange(uint32_t, const char*, const LV2_Host_Feature* const*) 
    : LV2Plugin(2) {
    
  }
  void run(uint32_t sample_count) {
    float* input = static_cast<float*>(m_ports[0]);
    float* output = static_cast<float*>(m_ports[1]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i) {
      float this_input = input[i] < MIN ? MIN : input[i];
      this_input = this_input > MAX ? MAX : this_input;
      output[i] = F(this_input);
    }
  }
};


template <unary_f F, bool A, float& MIN>
class UnaryMin : public LV2Plugin {
public:
  UnaryMin(uint32_t, const char*, const LV2_Host_Feature* const*) 
    : LV2Plugin(2) {
    
  }
  void run(uint32_t sample_count) {
    float* input = static_cast<float*>(m_ports[0]);
    float* output = static_cast<float*>(m_ports[1]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i) {
      float this_input = input[i] < MIN ? MIN : input[i];
      output[i] = F(this_input);
    }
  }
};


template <binary_f F, bool A>
class Binary : public LV2Plugin {
public:
  Binary(uint32_t, const char*, const LV2_Host_Feature* const*) 
    : LV2Plugin(3) {
    
  }
  void run(uint32_t sample_count) {
    float* input1 = static_cast<float*>(m_ports[0]);
    float* input2 = static_cast<float*>(m_ports[1]);
    float* output = static_cast<float*>(m_ports[2]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i)
      output[i] = F(input1[i], input2[i]);
  }
};


template <binary_f F, bool A>
class BinaryGuard : public LV2Plugin {
public:
  BinaryGuard(uint32_t, const char*, const LV2_Host_Feature* const*) 
    : LV2Plugin(3) {
    
  }
  void run(uint32_t sample_count) {
    float* input1 = static_cast<float*>(m_ports[0]);
    float* input2 = static_cast<float*>(m_ports[1]);
    float* output = static_cast<float*>(m_ports[2]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i) {
      output[i] = F(input1[i], input2[i]);
      if (!isnormal(output[i]))
        output[i] = 0;
    }
  }
};


template <bool A>
class Modf : public LV2Plugin {
public:
  Modf(uint32_t, const char*, const LV2_Host_Feature* const*) 
    : LV2Plugin(3) {
    
  }
  void run(uint32_t sample_count) {
    float* input = static_cast<float*>(m_ports[0]);
    float* output1 = static_cast<float*>(m_ports[1]);
    float* output2 = static_cast<float*>(m_ports[2]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i)
      output2[i] = modf(input[i], output1 + i);
  }
};


#define LL_PREFIX "http://ll-plugins.nongnu.org/lv2/dev/math-function-"


void initialise() __attribute__((constructor));
void initialise() {
  
  register_lv2<Unary<&atan, true> >(LL_PREFIX "atan/0.0.0");
  register_lv2<Unary<&atan, false> >(LL_PREFIX "atan-ctrl/0.0.0");
  register_lv2<Unary<&ceil, true> >(LL_PREFIX "ceil/0.0.0");
  register_lv2<Unary<&ceil, false> >(LL_PREFIX "ceil-ctrl/0.0.0");
  register_lv2<Unary<&cos, true> >(LL_PREFIX "cos/0.0.0");
  register_lv2<Unary<&cos, false> >(LL_PREFIX "cos-ctrl/0.0.0");
  register_lv2<Unary<&cosh, true> >(LL_PREFIX "cosh/0.0.0");
  register_lv2<Unary<&cosh, false> >(LL_PREFIX "cosh-ctrl/0.0.0");
  register_lv2<Unary<&exp, true> >(LL_PREFIX "exp/0.0.0");
  register_lv2<Unary<&exp, false> >(LL_PREFIX "exp-ctrl/0.0.0");
  register_lv2<Unary<&abs, true> >(LL_PREFIX "abs/0.0.0");
  register_lv2<Unary<&abs, false> >(LL_PREFIX "abs-ctrl/0.0.0");
  register_lv2<Unary<&floor, true> >(LL_PREFIX "floor/0.0.0");
  register_lv2<Unary<&floor, false> >(LL_PREFIX "floor-ctrl/0.0.0");
  register_lv2<Unary<&sin, true> >(LL_PREFIX "sin/0.0.0");
  register_lv2<Unary<&sin, false> >(LL_PREFIX "sin-ctrl/0.0.0");
  register_lv2<Unary<&sinh, true> >(LL_PREFIX "sinh/0.0.0");
  register_lv2<Unary<&sinh, false> >(LL_PREFIX "sinh-ctrl/0.0.0");
  register_lv2<UnaryMin<&log, true, epsilon> >(LL_PREFIX "log/0.0.0");
  register_lv2<UnaryMin<&log, false, epsilon> >(LL_PREFIX "log-ctrl/0.0.0");
  register_lv2<UnaryMin<&log10, true, epsilon> >(LL_PREFIX "log10/0.0.0");
  register_lv2<UnaryMin<&log10, false,epsilon> >(LL_PREFIX "log10-ctrl/0.0.0");
  register_lv2<UnaryMin<&sqrt, true, zero> >(LL_PREFIX "sqrt/0.0.0");
  register_lv2<UnaryMin<&sqrt, false, zero> >(LL_PREFIX "sqrt-ctrl/0.0.0");
  register_lv2<UnaryRange<&acos, true, neg1, pos1> >(LL_PREFIX "acos/0.0.0");
  register_lv2<UnaryRange<&acos,false,neg1,pos1> >(LL_PREFIX"acos-ctrl/0.0.0");
  register_lv2<UnaryRange<&asin, true, neg1, pos1> >(LL_PREFIX "asin/0.0.0");
  register_lv2<UnaryRange<&asin,false,neg1,pos1> >(LL_PREFIX"asin-ctrl/0.0.0");
  register_lv2<UnaryGuard<&tan, true> >(LL_PREFIX "tan/0.0.0");
  register_lv2<UnaryGuard<&tan, false> >(LL_PREFIX "tan-ctrl/0.0.0");
  register_lv2<UnaryGuard<&tanh, true> >(LL_PREFIX "tanh/0.0.0");
  register_lv2<UnaryGuard<&tanh, false> >(LL_PREFIX "tanh-ctrl/0.0.0");

  register_lv2<Binary<&atan2, true> >(LL_PREFIX "atan2/0.0.0");
  register_lv2<Binary<&atan2, false> >(LL_PREFIX "atan2-ctrl/0.0.0");
  register_lv2<BinaryGuard<&fmod, true> >(LL_PREFIX "fmod/0.0.0");
  register_lv2<BinaryGuard<&fmod, false> >(LL_PREFIX "fmod-ctrl/0.0.0");
  register_lv2<BinaryGuard<&pow, true> >(LL_PREFIX "pow/0.0.0");
  register_lv2<BinaryGuard<&pow, false> >(LL_PREFIX "pow-ctrl/0.0.0");

  register_lv2<Modf<true> >(LL_PREFIX "modf/0.0.0");
  register_lv2<Modf<false> >(LL_PREFIX "modf-ctrl/0.0.0");
}
