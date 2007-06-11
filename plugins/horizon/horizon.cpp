#include <iostream>
#include <string>
#include <vector>

#include "action.hpp"
#include "lv2advanced.hpp"
#include "horizon.peg"
#include "sample.hpp"
#include "actiontrigger.hpp"
#include "mixer.hpp"


using namespace std;


class Horizon : public LV2Advanced {
public:
  
  
  Horizon(uint32_t rate, const char* bundle_path, 
	  const LV2_Host_Feature* const* f)
    : LV2Advanced(h_n_ports),
      m_trigger(m_mixer) {
    
    load_sample("/home/ll/div/loops/tomsphases.wav");
  }
  
  
  ~Horizon() {
    for (unsigned i = 0; i < m_samples.size(); ++i)
      delete m_samples[i];
  }
  
  
  void run(uint32_t nframes) {
    m_mixer.set_buffers(p(h_left), p(h_right));
    m_trigger.run(p<LV2_MIDI>(h_midi_input), nframes);
  }


  char* tell_plugin(uint32_t argc, const char*const* argv) {
    if (argc == 2 && !strcmp(argv[0], "load_sample")) {
      if (load_sample(argv[1])) {
	cerr<<"Loaded sample from "<<argv[1]<<endl;
	return 0;
      }
      else
	return strdup((string("Failed to load ") + argv[1]).c_str());
    }
    
    return strdup("Unknown command!");
  }
  
protected:
  
  bool load_sample(const std::string& filename) {
    
    Sample* sample = new Sample(filename);
    if (!sample->is_valid()) {
      delete sample;
      return false;
    }
    
    m_samples.push_back(sample);
    int n = m_samples.size() - 1;
    Action* a1 = new Action(*m_samples[n]->get_chunks()[0]);
    Action* a2 = new Action(*m_samples[n]->get_chunks()[1]);
    Action* a3 = new Action(*m_samples[n]->get_chunks()[2]);
    Action* a4 = new Action(*m_samples[n]->get_chunks()[3]);
    m_trigger.add_action(a1);
    m_trigger.map_action(a1, 60 + n * 4);
    m_trigger.add_action(a2);
    m_trigger.map_action(a2, 61 + n * 4);
    m_trigger.add_action(a3);
    m_trigger.map_action(a3, 62 + n * 4);
    m_trigger.add_action(a4);
    m_trigger.map_action(a4, 63 + n * 4);

    return true;
  }
  
  
  Mixer m_mixer;
  ActionTrigger m_trigger;
  vector<Sample*> m_samples;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2_adv<Horizon>(h_uri);
}
