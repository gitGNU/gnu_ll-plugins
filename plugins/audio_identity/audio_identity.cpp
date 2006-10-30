#include <iostream>

#include <cstring>

#include "lv2plugin.hpp"


using namespace std;


/** This is the class that contains all the code and data for the audio
    identity plugin. */
class AudioIdentity : public LV2Plugin {
public:
  
  AudioIdentity(uint32_t, const char*, const LV2_Host_Feature**) 
    : LV2Plugin(2) {
    
  }
  
  
  void run(uint32_t nframes) {
    std::memcpy(p(1), p(0), sizeof(float) * nframes);
  }
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2<AudioIdentity>("http://ll-plugins.nongnu.org/lv2/dev/audio_identity/0.0.0");
}
