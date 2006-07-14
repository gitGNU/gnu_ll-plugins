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

  float neg1 = -1;
  
  float pos1 = 1;
  
  float zero = 0;
  
  float epsilon = 0.00001;
  
}


template <unary_f F>
class UnaryFunction : public LV2Plugin {
public:
  UnaryFunction(unsigned long, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(2) {
    
  }
  void run(unsigned long sample_count) {
    float* input = static_cast<float*>(m_ports[0]);
    float* output = static_cast<float*>(m_ports[1]);
    for (unsigned long i = 0; i < sample_count; ++i)
      output[i] = F(input[i]);
  }
};


template <unary_f F>
class UnaryFunctionGuard : public LV2Plugin {
public:
  UnaryFunctionGuard(unsigned long, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(2) {
    
  }
  void run(unsigned long sample_count) {
    float* input = static_cast<float*>(m_ports[0]);
    float* output = static_cast<float*>(m_ports[1]);
    for (unsigned long i = 0; i < sample_count; ++i) {
      output[i] = F(input[i]);
      if (!isnormal(output[i]))
        output[i] = 0;
    }
  }
};


template <unary_f F, float& MIN, float& MAX>
class UnaryFunctionRange : public LV2Plugin {
public:
  UnaryFunctionRange(unsigned long, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(2) {
    
  }
  void run(unsigned long sample_count) {
    float* input = static_cast<float*>(m_ports[0]);
    float* output = static_cast<float*>(m_ports[1]);
    for (unsigned long i = 0; i < sample_count; ++i) {
      float this_input = input[i] < MIN ? MIN : input[i];
      this_input = this_input > MAX ? MAX : this_input;
      output[i] = F(this_input);
    }
  }
};


template <unary_f F, float& MIN>
class UnaryFunctionMin : public LV2Plugin {
public:
  UnaryFunctionMin(unsigned long, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(2) {
    
  }
  void run(unsigned long sample_count) {
    float* input = static_cast<float*>(m_ports[0]);
    float* output = static_cast<float*>(m_ports[1]);
    for (unsigned long i = 0; i < sample_count; ++i) {
      float this_input = input[i] < MIN ? MIN : input[i];
      output[i] = F(this_input);
    }
  }
};


template <binary_f F>
class BinaryFunction : public LV2Plugin {
public:
  BinaryFunction(unsigned long, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(3) {
    
  }
  void run(unsigned long sample_count) {
    float* input1 = static_cast<float*>(m_ports[0]);
    float* input2 = static_cast<float*>(m_ports[1]);
    float* output = static_cast<float*>(m_ports[2]);
    for (unsigned long i = 0; i < sample_count; ++i)
      output[i] = F(input1[i], input2[i]);
  }
};


template <binary_f F>
class BinaryFunctionGuard : public LV2Plugin {
public:
  BinaryFunctionGuard(unsigned long, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(3) {
    
  }
  void run(unsigned long sample_count) {
    float* input1 = static_cast<float*>(m_ports[0]);
    float* input2 = static_cast<float*>(m_ports[1]);
    float* output = static_cast<float*>(m_ports[2]);
    for (unsigned long i = 0; i < sample_count; ++i) {
      output[i] = F(input1[i], input2[i]);
      if (!isnormal(output[i]))
        output[i] = 0;
    }
  }
};


class ModfFunction : public LV2Plugin {
public:
  ModfFunction(unsigned long, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(3) {
    
  }
  void run(unsigned long sample_count) {
    float* input = static_cast<float*>(m_ports[0]);
    float* output1 = static_cast<float*>(m_ports[1]);
    float* output2 = static_cast<float*>(m_ports[2]);
    for (unsigned long i = 0; i < sample_count; ++i)
      output2[i] = modf(input[i], output1 + i);
  }
};


#define LL_PREFIX "http://ll-plugins.nongnu.org/lv2/dev/math-function-"


void initialise() __attribute__((constructor));
void initialise() {
  
  register_lv2<UnaryFunctionRange<&acos, neg1, pos1> >(LL_PREFIX "acos/0.0.0");
  register_lv2<UnaryFunctionRange<&asin, neg1, pos1> >(LL_PREFIX "asin/0.0.0");
  register_lv2<UnaryFunction<&atan> >(LL_PREFIX "atan/0.0.0");
  register_lv2<UnaryFunction<&ceil> >(LL_PREFIX "ceil/0.0.0");
  register_lv2<UnaryFunction<&cos> >(LL_PREFIX "cos/0.0.0");
  register_lv2<UnaryFunction<&cosh> >(LL_PREFIX "cosh/0.0.0");
  register_lv2<UnaryFunction<&exp> >(LL_PREFIX "exp/0.0.0");
  register_lv2<UnaryFunction<&abs> >(LL_PREFIX "abs/0.0.0");
  register_lv2<UnaryFunction<&floor> >(LL_PREFIX "floor/0.0.0");
  register_lv2<UnaryFunctionMin<&log, epsilon> >(LL_PREFIX "log/0.0.0");
  register_lv2<UnaryFunctionMin<&log10, epsilon> >(LL_PREFIX "log10/0.0.0");
  register_lv2<UnaryFunction<&sin> >(LL_PREFIX "sin/0.0.0");
  register_lv2<UnaryFunction<&sinh> >(LL_PREFIX "sinh/0.0.0");
  register_lv2<UnaryFunctionMin<&sqrt, zero> >(LL_PREFIX "sqrt/0.0.0");
  register_lv2<UnaryFunctionGuard<&tan> >(LL_PREFIX "tan/0.0.0");
  register_lv2<UnaryFunctionGuard<&tanh> >(LL_PREFIX "tanh/0.0.0");

  register_lv2<BinaryFunction<&atan2> >(LL_PREFIX "atan2/0.0.0");
  register_lv2<BinaryFunctionGuard<&fmod> >(LL_PREFIX "fmod/0.0.0");
  register_lv2<BinaryFunctionGuard<&pow> >(LL_PREFIX "pow/0.0.0");

  register_lv2<ModfFunction >(LL_PREFIX "modf/0.0.0");
}
