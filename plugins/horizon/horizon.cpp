#include <iostream>

#include "action.hpp"
#include "lv2instrument.hpp"
#include "horizon.peg"
#include "sample.hpp"
#include "actiontrigger.hpp"
#include "mixer.hpp"


using namespace std;


class Horizon : public LV2Instrument {
public:
  
  
  Horizon(uint32_t rate, const char* bundle_path, 
	  const LV2_Host_Feature* const* f)
    : LV2Instrument(h_n_ports),
      m_trigger(m_mixer),
      m_sample("/home/ll/div/loops/mbs.wav"),
      m_pos(0) {
    
    Action* a1 = new Action(*m_sample.get_chunks()[0]);
    Action* a2 = new Action(*m_sample.get_chunks()[1]);
    Action* a3 = new Action(*m_sample.get_chunks()[2]);
    Action* a4 = new Action(*m_sample.get_chunks()[3]);
    m_trigger.add_action(a1);
    m_trigger.map_action(a1, 60);
    m_trigger.add_action(a2);
    m_trigger.map_action(a2, 61);
    m_trigger.add_action(a3);
    m_trigger.map_action(a3, 62);
    m_trigger.add_action(a4);
    m_trigger.map_action(a4, 63);
    
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
