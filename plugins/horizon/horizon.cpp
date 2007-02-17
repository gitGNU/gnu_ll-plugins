#include <iostream>

#include "lv2instrument.hpp"
#include "horizon.peg"
#include "sample.hpp"
#include "actiontrigger.hpp"
#include "mixer.hpp"


using namespace std;


class Horizon : public LV2Instrument {
public:
  
  
  Horizon(uint32_t rate, const char* bundle_path, const LV2_Host_Feature** f)
    : LV2Instrument(h_n_ports),
      m_trigger(m_mixer),
      m_sample("/home/ll/div/loops/us10phase.wav"),
      m_pos(0) {
    
  }
  
  
  void run(uint32_t nframes) {
    m_mixer.set_buffers(p(h_left), p(h_right));
    m_trigger.run(p<LV2_MIDI>(h_midi_input), nframes);
  }
  
  
protected:
  
  Mixer m_mixer;
  ActionTrigger m_trigger;
  Sample m_sample;
  unsigned m_pos;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2_inst<Horizon>(h_uri, true);
}
