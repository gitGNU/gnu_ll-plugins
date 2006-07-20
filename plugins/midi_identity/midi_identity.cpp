#include <cstring>

#include "lv2plugin.hpp"
#include "lv2-miditype.h"


/** This is the class that contains all the code and data for the MIDI 
    identity plugin. */
class MIDIIdentity : public LV2Plugin {
public:
  
  MIDIIdentity(unsigned long, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(2) {
    
  }
  
  
  void run(unsigned long sample_count) {
    
    LV2_MIDI* input = static_cast<LV2_MIDI*>(m_ports[0]);
    LV2_MIDI* output = static_cast<LV2_MIDI*>(m_ports[1]);
    
    if (input->size <= output->capacity) {
      output->event_count = input->event_count;
      output->size = input->size;
      std::memcpy(output->data, input->data, input->size);
    }
    else {
      output->event_count = 0;
      output->size = 0;
    }
    
  }
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2<MIDIIdentity>("http://ll-plugins.nongnu.org/lv2/dev/midi_identity/0.0.0");
}
