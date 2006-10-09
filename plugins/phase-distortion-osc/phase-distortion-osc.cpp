#include "lv2plugin.hpp"
#include "pdosc.hpp"


class PhaseDistortionOscillator : public LV2Plugin {
public:
  
  PhaseDistortionOscillator(unsigned long rate, const char*, 
                            const LV2_Host_Feature**) 
    : LV2Plugin(4),
      m_osc(rate) {

  }
  
  
  void activate() {
    m_osc.reset();
  }

  
  void run(unsigned long nframes) {
    float* f = static_cast<float*>(m_ports[0]);
    float shape = *static_cast<float*>(m_ports[1]);
    float amount = *static_cast<float*>(m_ports[2]);
    float* output = static_cast<float*>(m_ports[3]);
    
    int ishape = int(shape);
    ishape = (ishape < 0 ? 0 : ishape);
    ishape = (ishape > 5 ? 5 : ishape);
    amount = (amount < 0 ? 0 : amount);
    amount = (amount > 0.999 ? 0.999 : amount);
    
    for (unsigned long i = 0; i < nframes; ++i)
      output[i] = m_osc.run(440, ishape, amount);
  }
  

protected:
  
  PDOsc m_osc;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2<PhaseDistortionOscillator>("http://ll-plugins.nongnu.org/lv2/dev/phase-distortion-osc/0.0.0");
}
