/****************************************************************************
    
    math-functions.cpp - LV2 plugins implementing some common functions
                         from the C math library
    
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
class Unary : public LV2::Plugin {
public:
  Unary(double, const char*, const LV2_Feature* const*) 
    : LV2::Plugin(2) {
    
  }
  void run(uint32_t sample_count) {
    float* input = static_cast<float*>(m_ports[0]);
    float* output = static_cast<float*>(m_ports[1]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i)
      output[i] = F(input[i]);
  }
};


template <unary_f F, bool A>
class UnaryGuard : public LV2::Plugin {
public:
  UnaryGuard(double, const char*, const LV2_Feature* const*) 
    : LV2::Plugin(2) {
    
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
class UnaryRange : public LV2::Plugin {
public:
  UnaryRange(double, const char*, const LV2_Feature* const*) 
    : LV2::Plugin(2) {
    
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
class UnaryMin : public LV2::Plugin {
public:
  UnaryMin(double, const char*, const LV2_Feature* const*) 
    : LV2::Plugin(2) {
    
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
class Binary : public LV2::Plugin {
public:
  Binary(double, const char*, const LV2_Feature* const*) 
    : LV2::Plugin(3) {
    
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
class BinaryGuard : public LV2::Plugin {
public:
  BinaryGuard(double, const char*, const LV2_Feature* const*) 
    : LV2::Plugin(3) {
    
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
class Modf : public LV2::Plugin {
public:
  Modf(double, const char*, const LV2_Feature* const*) 
    : LV2::Plugin(3) {
    
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


static LV2::Register<Unary<&atan, true> > 
reg01(LL_PREFIX "atan/0.0.0");
static LV2::Register<Unary<&atan, false> > 
reg02(LL_PREFIX "atan-ctrl/0.0.0");
static LV2::Register<Unary<&ceil, true> > 
reg03(LL_PREFIX "ceil/0.0.0");
static LV2::Register<Unary<&ceil, false> > 
reg04(LL_PREFIX "ceil-ctrl/0.0.0");
static LV2::Register<Unary<&cos, true> > 
reg05(LL_PREFIX "cos/0.0.0");
static LV2::Register<Unary<&cos, false> > 
reg06(LL_PREFIX "cos-ctrl/0.0.0");
static LV2::Register<Unary<&cosh, true> > 
reg07(LL_PREFIX "cosh/0.0.0");
static LV2::Register<Unary<&cosh, false> > 
reg08(LL_PREFIX "cosh-ctrl/0.0.0");
static LV2::Register<Unary<&exp, true> > 
reg09(LL_PREFIX "exp/0.0.0");
static LV2::Register<Unary<&exp, false> > 
reg10(LL_PREFIX "exp-ctrl/0.0.0");
static LV2::Register<Unary<&abs, true> > 
reg11(LL_PREFIX "abs/0.0.0");
static LV2::Register<Unary<&abs, false> > 
reg12(LL_PREFIX "abs-ctrl/0.0.0");
static LV2::Register<Unary<&floor, true> > 
reg13(LL_PREFIX "floor/0.0.0");
static LV2::Register<Unary<&floor, false> > 
reg14(LL_PREFIX "floor-ctrl/0.0.0");
static LV2::Register<Unary<&sin, true> > 
reg15(LL_PREFIX "sin/0.0.0");
static LV2::Register<Unary<&sin, false> > 
reg16(LL_PREFIX "sin-ctrl/0.0.0");
static LV2::Register<Unary<&sinh, true> > 
reg17(LL_PREFIX "sinh/0.0.0");
static LV2::Register<Unary<&sinh, false> > 
reg18(LL_PREFIX "sinh-ctrl/0.0.0");
static LV2::Register<UnaryMin<&log, true, epsilon> > 
reg19(LL_PREFIX "log/0.0.0");
static LV2::Register<UnaryMin<&log, false, epsilon> > 
reg20(LL_PREFIX "log-ctrl/0.0.0");
static LV2::Register<UnaryMin<&log10, true, epsilon> > 
reg21(LL_PREFIX "log10/0.0.0");
static LV2::Register<UnaryMin<&log10, false,epsilon> > 
reg22(LL_PREFIX "log10-ctrl/0.0.0");
static LV2::Register<UnaryMin<&sqrt, true, zero> > 
reg23(LL_PREFIX "sqrt/0.0.0");
static LV2::Register<UnaryMin<&sqrt, false, zero> > 
reg24(LL_PREFIX "sqrt-ctrl/0.0.0");
static LV2::Register<UnaryRange<&acos, true, neg1, pos1> > 
reg25(LL_PREFIX "acos/0.0.0");
static LV2::Register<UnaryRange<&acos,false,neg1,pos1> > 
reg26(LL_PREFIX"acos-ctrl/0.0.0");
static LV2::Register<UnaryRange<&asin, true, neg1, pos1> > 
reg27(LL_PREFIX "asin/0.0.0");
static LV2::Register<UnaryRange<&asin,false,neg1,pos1> > 
reg28(LL_PREFIX"asin-ctrl/0.0.0");
static LV2::Register<UnaryGuard<&tan, true> > 
reg29(LL_PREFIX "tan/0.0.0");
static LV2::Register<UnaryGuard<&tan, false> > 
reg30(LL_PREFIX "tan-ctrl/0.0.0");
static LV2::Register<UnaryGuard<&tanh, true> > 
reg31(LL_PREFIX "tanh/0.0.0");
static LV2::Register<UnaryGuard<&tanh, false> > 
reg32(LL_PREFIX "tanh-ctrl/0.0.0");
static LV2::Register<Binary<&atan2, true> > 
reg33(LL_PREFIX "atan2/0.0.0");
static LV2::Register<Binary<&atan2, false> > 
reg34(LL_PREFIX "atan2-ctrl/0.0.0");
static LV2::Register<BinaryGuard<&fmod, true> > 
reg35(LL_PREFIX "fmod/0.0.0");
static LV2::Register<BinaryGuard<&fmod, false> > 
reg36(LL_PREFIX "fmod-ctrl/0.0.0");
static LV2::Register<BinaryGuard<&pow, true> > 
reg37(LL_PREFIX "pow/0.0.0");
static LV2::Register<BinaryGuard<&pow, false> > 
reg38(LL_PREFIX "pow-ctrl/0.0.0");
static LV2::Register<Modf<true> > 
reg39(LL_PREFIX "modf/0.0.0");
static LV2::Register<Modf<false> > 
reg40(LL_PREFIX "modf-ctrl/0.0.0");

