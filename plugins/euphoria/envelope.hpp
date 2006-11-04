#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include <sstream>
#include <string>
#include <vector>

#include <pthread.h>
#include <stdint.h>


class Envelope {
public:
  
  Envelope(uint32_t rate);
  ~Envelope();

  float run(float a, float d, float s, float r);
  
  void on();
  void off();
  void fast_off();
  
  bool set_string(const std::string& str);
  
protected:
  
  enum StageType {
    Constant,
    Attack,
    Decay,
    Release,
    NumTypes
  };
  
  enum CurveType {
    Linear,
    Exponential,
    Exponential2,
    NumCurves
  };
  
  struct Stage {
    Stage(float _start, float _p, float _ss, StageType _type, CurveType _curve)
      : start(_start), p(_p), ss(_ss), type(_type), curve(_curve) { }
    Stage() { }
    float start;
    float p;
    float ss;
    StageType type;
    CurveType curve;
  };
  
  std::vector<Stage> m_stages;
  
  float m_value;
  int m_stage;
  float m_phase;
  uint32_t m_rate;
  pthread_mutex_t m_mutex;
};


Envelope::Envelope(uint32_t rate) 
  : m_value(0),
    m_stage(-1),
    m_phase(0),
    m_rate(rate) {

  m_stages.push_back(Stage(0, 1, 0, Attack, Linear));
  m_stages.push_back(Stage(1, 1, 0, Decay, Exponential));
  m_stages.push_back(Stage(0.5, 1, 1, Constant, Linear));
  m_stages.push_back(Stage(0.5, 1, 1, Release, Exponential));
  
  pthread_mutex_init(&m_mutex, 0);
}


Envelope::~Envelope() {
  pthread_mutex_destroy(&m_mutex);
}


float Envelope::run(float a, float d, float s, float r) {
  
  if (pthread_mutex_trylock(&m_mutex))
    return m_value;
  
  switch (m_stage) {

  case -2:
    m_value -= 1.0 / (0.003 * m_rate);
    if (m_value <= 0) {
      m_value = 0;
      m_stage = -1;
    }
    pthread_mutex_unlock(&m_mutex);
    return m_value;
    
  case -1:
    pthread_mutex_unlock(&m_mutex);
    return m_value;
    
  default: {
    float phi = 1.0 / (m_rate);
    if (m_stages[m_stage].type == Attack)
      phi /= a;
    else if (m_stages[m_stage].type == Decay)
      phi /= d;
    else if (m_stages[m_stage].type == Release)
      phi /= r;
    m_phase += phi;
    float a = m_stages[m_stage].start;
    float b = m_stages[(m_stage + 1) % m_stages.size()].start;
    if (m_phase >= 1) {
      ++m_stage;
      m_phase = 0;
      if (m_stage >= m_stages.size())
        m_stage = -1;
    }
    m_value = a + (b - a) * m_phase;
    pthread_mutex_unlock(&m_mutex);
    return m_value;
  }
    
  }
  
}
 
 
void Envelope::on() {
  m_stage = 0;
  m_phase = 0;
}


void Envelope::off() {
  //m_stage = -2;
}


void Envelope::fast_off() {
  m_stage = -2;
  m_phase = 0;
}

  
bool Envelope::set_string(const std::string& str) {
  std::istringstream iss(str);
  std::vector<Stage> new_segments;
  while (iss.good()) {
    Stage s;
    iss>>s.start>>s.p>>s.ss;
    std::string tmp;
    iss>>tmp;
    if (tmp == "c")
      s.type = Constant;
    else if (tmp == "a")
      s.type = Attack;
    else if (tmp == "d")
      s.type = Decay;
    else if (tmp == "r")
      s.type = Release;
    iss>>tmp;
    if (tmp == "l")
      s.curve = Linear;
    else if (tmp == "e")
      s.curve = Exponential;
    else if (tmp == "e2")
      s.curve = Exponential2;
    new_segments.push_back(s);
  }
  
  if (new_segments.size() == 0 || new_segments[0].start != 0)
    return false;
  
  m_stages = new_segments;
  
  return true;
}


#endif
