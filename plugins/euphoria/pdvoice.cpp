#include "pdvoice.hpp"

  
PDVoice::PDVoice(uint32_t rate)
  : m_pdosc(rate),
    m_pd_dist_env(rate),
    m_pd_amp_env(rate) {

}


PDVoice::~PDVoice() {

}


void PDVoice::run(float* left, float* right, uint32_t nframes) {

}

  
void PDVoice::on(unsigned char key, unsigned char velocity) {
  m_pd_dist_env.on();
  m_pd_amp_env.on();
}

  
void PDVoice::off() {
  m_pd_dist_env.off();
  m_pd_amp_env.off();
}

  
void PDVoice::fast_off() {
  m_pd_dist_env.fast_off();
  m_pd_amp_env.fast_off();
}

  
void PDVoice::reset() {

}


void PDVoice::set_phase(const std::string& str) {
  m_pdosc.set_string(str);
}

  
void PDVoice::set_dist_env(const std::string& str) {
  m_pd_dist_env.set_string(str);
}

  
void PDVoice::set_gain_env(const std::string& str) {
  m_pd_amp_env.set_string(str);
}

  

