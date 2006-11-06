#include <iostream>
#include <sstream>

#include <gsl/gsl_chebyshev.h>

#include "shaper.hpp"


using namespace std;


Shaper::Shaper(unsigned int rate) 
  : m_rate(rate) {
  for (int i = 0; i < 10; ++i)
    m_tables[i] = new float[WAVETABLE_SIZE];
}


Shaper::~Shaper() {
  for (int i = 0; i < 10; ++i)
    delete [] m_tables[i];
}
  

float Shaper::run(float input, float max_freq) {
  float table;
  if (max_freq > 0) {
    float ratio = m_rate / (2 * max_freq);
    table = log2(ratio) - 0.5;
    //cerr<<m_rate<<" "<<max_freq<<" "<<ratio<<" "<<table<<endl;
  }
  else
    table = 9;
  if (table < 0)
    return 0;
  int itable = int(table);
  int index = int((input + 1) * (WAVETABLE_SIZE - 1) / 2);
  if (table >= 9)
    return m_tables[9][index];
  float r = table - itable;
  return m_tables[itable][index] * (1 - r) + m_tables[itable + 1][index] * r;
}
 
 
bool Shaper::set_string(const std::string& str) {

  istringstream iss(str);
  vector<float> points;
  
  while (iss.good()) {
    float x;
    iss>>x>>ws;
    points.push_back(x);
  }

  if (points.size() < 4 || points[0] != -1 || 
      points[points.size() - 2] != 1)
    return false;
  
  gsl_function func;
  func.function = &Shaper::function;
  func.params = &points;
  gsl_cheb_series* cheb = gsl_cheb_alloc(int(pow(2, 10 - 1)));
  gsl_cheb_init(cheb, &func, -1, 1);
  int c = 1;
  for (int t = 0; t < 10; ++t) {
    for (int i = 0; i < WAVETABLE_SIZE; ++i)
      m_tables[t][i] = gsl_cheb_eval_n(cheb, c,
                                       -1.0 + 2.0 * i / (WAVETABLE_SIZE - 1));
    c *= 2;
  }
  gsl_cheb_free(cheb);
  
  return true;
}


double Shaper::function(double x, void* params) {
  vector<float>& v = *static_cast<vector<float>*>(params);
  if (x < -1)
    return v[1];
  for (int i = 1; i < v.size() / 2; ++i) {
    if (x < v[2 * i])
      return v[2 * i - 1] + (x - v[2 * (i - 1)]) * 
        (v[2 * i + 1] - v[2 * i - 1]) / (v[2 * i] - v[2 * (i - 1)]);
  }
  return v[v.size() - 1];
}

