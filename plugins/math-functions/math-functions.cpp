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
class Unary : public LV2::Plugin< Unary<F, A> > {
public:

  typedef LV2::Plugin< Unary<F, A> > Parent;

  Unary(double) : Parent(2) {
    
  }
  void run(uint32_t sample_count) {
    float* input = static_cast<float*>(Parent::m_ports[0]);
    float* output = static_cast<float*>(Parent::m_ports[1]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i)
      output[i] = F(input[i]);
  }
};


template <unary_f F, bool A>
class UnaryGuard : public LV2::Plugin< UnaryGuard<F, A> > {
public:
  
  typedef LV2::Plugin< UnaryGuard<F, A> > Parent;
  
  UnaryGuard(double) : Parent(2) {
    
  }
  void run(uint32_t sample_count) {
    float* input = static_cast<float*>(Parent::m_ports[0]);
    float* output = static_cast<float*>(Parent::m_ports[1]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i) {
      output[i] = F(input[i]);
      if (!isnormal(output[i]))
        output[i] = 0;
    }
  }
};


template <unary_f F, bool A, float& MIN, float& MAX>
class UnaryRange : public LV2::Plugin< UnaryRange<F, A, MIN, MAX> > {
public:

  typedef LV2::Plugin< UnaryRange<F, A, MIN, MAX> > Parent;

  UnaryRange(double) : Parent(2) {
    
  }
  void run(uint32_t sample_count) {
    float* input = static_cast<float*>(Parent::m_ports[0]);
    float* output = static_cast<float*>(Parent::m_ports[1]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i) {
      float this_input = input[i] < MIN ? MIN : input[i];
      this_input = this_input > MAX ? MAX : this_input;
      output[i] = F(this_input);
    }
  }
};


template <unary_f F, bool A, float& MIN>
class UnaryMin : public LV2::Plugin< UnaryMin<F, A, MIN> > {
public:

  typedef LV2::Plugin< UnaryMin<F, A, MIN> > Parent;

  UnaryMin(double) : Parent(2) {
    
  }
  void run(uint32_t sample_count) {
    float* input = static_cast<float*>(Parent::m_ports[0]);
    float* output = static_cast<float*>(Parent::m_ports[1]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i) {
      float this_input = input[i] < MIN ? MIN : input[i];
      output[i] = F(this_input);
    }
  }
};


template <binary_f F, bool A>
class Binary : public LV2::Plugin< Binary<F, A> > {
public:
  
  typedef LV2::Plugin< Binary<F, A> > Parent;
  
  Binary(double) : Parent(3) {
    
  }
  void run(uint32_t sample_count) {
    float* input1 = static_cast<float*>(Parent::m_ports[0]);
    float* input2 = static_cast<float*>(Parent::m_ports[1]);
    float* output = static_cast<float*>(Parent::m_ports[2]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i)
      output[i] = F(input1[i], input2[i]);
  }
};


template <binary_f F, bool A>
class BinaryGuard : public LV2::Plugin< BinaryGuard<F, A> > {
public:

  typedef LV2::Plugin< BinaryGuard<F, A> > Parent;

  BinaryGuard(double) : Parent(3) {
    
  }
  void run(uint32_t sample_count) {
    float* input1 = static_cast<float*>(Parent::m_ports[0]);
    float* input2 = static_cast<float*>(Parent::m_ports[1]);
    float* output = static_cast<float*>(Parent::m_ports[2]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i) {
      output[i] = F(input1[i], input2[i]);
      if (!isnormal(output[i]))
        output[i] = 0;
    }
  }
};


template <bool A>
class Modf : public LV2::Plugin< Modf<A> > {
public:

  typedef LV2::Plugin< Modf<A> > Parent;

  Modf(double) : Parent(3) {
    
  }
  void run(uint32_t sample_count) {
    float* input = static_cast<float*>(Parent::m_ports[0]);
    float* output1 = static_cast<float*>(Parent::m_ports[1]);
    float* output2 = static_cast<float*>(Parent::m_ports[2]);
    for (uint32_t i = 0; i < (A ? sample_count : 1); ++i)
      output2[i] = modf(input[i], output1 + i);
  }
};


#define LL_PREFIX "http://ll-plugins.nongnu.org/lv2/dev/math-function-"


static unsigned _ = (Unary<&atan, true>::register_class(LL_PREFIX "atan/0.0.0"),
		     Unary<&atan, false>::register_class(LL_PREFIX "atan-ctrl/0.0.0"),
		     Unary<&ceil, true>::register_class(LL_PREFIX "ceil/0.0.0"),
		     Unary<&ceil, false>::register_class(LL_PREFIX "ceil-ctrl/0.0.0"),
		     Unary<&cos, true>::register_class(LL_PREFIX "cos/0.0.0"),
		     Unary<&cos, false>::register_class(LL_PREFIX "cos-ctrl/0.0.0"),
		     Unary<&cosh, true>::register_class(LL_PREFIX "cosh/0.0.0"),
		     Unary<&cosh, false>::register_class(LL_PREFIX "cosh-ctrl/0.0.0"),
		     Unary<&exp, true>::register_class(LL_PREFIX "exp/0.0.0"),
		     Unary<&exp, false>::register_class(LL_PREFIX "exp-ctrl/0.0.0"),
		     Unary<&abs, true>::register_class(LL_PREFIX "abs/0.0.0"),
		     Unary<&abs, false>::register_class(LL_PREFIX "abs-ctrl/0.0.0"),
		     Unary<&floor, true>::register_class(LL_PREFIX "floor/0.0.0"),
		     Unary<&floor, false>::register_class(LL_PREFIX "floor-ctrl/0.0.0"),
		     Unary<&sin, true>::register_class(LL_PREFIX "sin/0.0.0"),
		     Unary<&sin, false>::register_class(LL_PREFIX "sin-ctrl/0.0.0"),
		     Unary<&sinh, true>::register_class(LL_PREFIX "sinh/0.0.0"),
		     Unary<&sinh, false>::register_class(LL_PREFIX "sinh-ctrl/0.0.0"),
		     UnaryMin<&log, true, epsilon>::register_class(LL_PREFIX "log/0.0.0"),
		     UnaryMin<&log, false, epsilon>::register_class(LL_PREFIX "log-ctrl/0.0.0"),
		     UnaryMin<&log10, true, epsilon>::register_class(LL_PREFIX "log10/0.0.0"),
		     UnaryMin<&log10, false,epsilon>::register_class(LL_PREFIX "log10-ctrl/0.0.0"),
		     UnaryMin<&sqrt, true, zero>::register_class(LL_PREFIX "sqrt/0.0.0"),
		     UnaryMin<&sqrt, false, zero>::register_class(LL_PREFIX "sqrt-ctrl/0.0.0"),
		     UnaryRange<&acos, true, neg1, pos1>::register_class(LL_PREFIX "acos/0.0.0"),
		     UnaryRange<&acos,false,neg1,pos1>::register_class(LL_PREFIX"acos-ctrl/0.0.0"),
		     UnaryRange<&asin, true, neg1, pos1>::register_class(LL_PREFIX "asin/0.0.0"),
		     UnaryRange<&asin,false,neg1,pos1>::register_class(LL_PREFIX"asin-ctrl/0.0.0"),
		     UnaryGuard<&tan, true>::register_class(LL_PREFIX "tan/0.0.0"),
		     UnaryGuard<&tan, false>::register_class(LL_PREFIX "tan-ctrl/0.0.0"),
		     UnaryGuard<&tanh, true>::register_class(LL_PREFIX "tanh/0.0.0"),
		     UnaryGuard<&tanh, false>::register_class(LL_PREFIX "tanh-ctrl/0.0.0"),
		     Binary<&atan2, true>::register_class(LL_PREFIX "atan2/0.0.0"),
		     Binary<&atan2, false>::register_class(LL_PREFIX "atan2-ctrl/0.0.0"),
		     BinaryGuard<&fmod, true>::register_class(LL_PREFIX "fmod/0.0.0"),
		     BinaryGuard<&fmod, false>::register_class(LL_PREFIX "fmod-ctrl/0.0.0"),
		     BinaryGuard<&pow, true>::register_class(LL_PREFIX "pow/0.0.0"),
		     BinaryGuard<&pow, false>::register_class(LL_PREFIX "pow-ctrl/0.0.0"),
		     Modf<true>::register_class(LL_PREFIX "modf/0.0.0"),
		     Modf<false>::register_class(LL_PREFIX "modf-ctrl/0.0.0"));
