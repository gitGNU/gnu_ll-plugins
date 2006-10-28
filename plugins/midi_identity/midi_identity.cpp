#include <cstring>

#include "lv2plugin.hpp"
#include "lv2-midifunctions.h"


/** This is the class that contains all the code and data for the MIDI 
    identity plugin. */
class MIDIIdentity : public LV2Plugin {
public:
  
  MIDIIdentity(unsigned long, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(2) {
    
  }
  
  
  void run(unsigned long nframes) {
    
    LV2_MIDIState in = { static_cast<LV2_MIDI*>(m_ports[0]), nframes, 0 };
    LV2_MIDIState out = { static_cast<LV2_MIDI*>(m_ports[1]), nframes, 0 };
    
    out.midi->size = 0;
    out.midi->event_count = 0;
    
    double event_time;
    uint32_t event_size;
    unsigned char* event;

    while (lv2midi_get_event(&in, &event_time, &event_size, &event) < nframes){
      lv2midi_put_event(&out, event_time, event_size, event);
      lv2midi_step(&in);
    }
    
  }
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2<MIDIIdentity>("http://ll-plugins.nongnu.org/lv2/dev/midi_identity/0.0.0");
}
