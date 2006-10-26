#include "lv2plugin.hpp"
#include "klaviatur.peg"
#include "lv2-midifunctions.h"


class Klaviatur : public LV2Plugin {
public:

  Klaviatur(uint32_t rate, const char* bundle, const LV2_Host_Feature** f)
    : LV2Plugin(k_n_ports) {
    
  }
  
  
  void run(uint32_t nframes) {

    LV2_MIDIState in = { p<LV2_MIDI>(k_midi_input), nframes, 0 };
    LV2_MIDIState out = { p<LV2_MIDI>(k_midi_output), nframes, 0 };
    
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
  register_lv2<Klaviatur>("http://ll-plugins.nongnu.org/lv2/dev/klaviatur/0.0.0");
}
