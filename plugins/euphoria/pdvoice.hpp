#ifndef PDVOICE_HPP
#define PDVOICE_HPP

#include <string>

#include "pdoscillator.hpp"
#include "envelope.hpp"


class PDVoice {
public:
  
  PDVoice(uint32_t rate);
  ~PDVoice();
  
  void run(float* left, float* right, uint32_t nframes);
  
  void on(unsigned char key, unsigned char velocity);
  
  void off();
  
  void fast_off();
  
  void reset();
  
  void set_phase(const std::string& str);
  
  void set_dist_env(const std::string& str);
  
  void set_gain_env(const std::string& str);
  
private:
  
  PDOscillator m_pdosc;
  Envelope m_pd_dist_env;
  Envelope m_pd_amp_env;
  
};


#endif
