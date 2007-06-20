#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <semaphore.h>

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
    
    sem_init(&m_lock, 0, 1);
    
    load_sample("/home/ll/div/loops/tomsphases.wav");
  }
  
  
  ~Horizon() {
    sem_destroy(&m_lock);
    for (unsigned i = 0; i < m_samples.size(); ++i)
      delete m_samples[i];
  }
  
  
  void run(uint32_t nframes) {
    if (!sem_trywait(&m_lock)) {
      m_mixer.set_buffers(p(h_left), p(h_right));
      m_trigger.run(p<LV2_MIDI>(h_midi_input), nframes);
      sem_post(&m_lock);
    }
    else {
      memset(p(h_left), 0, sizeof(float) * nframes);
      memset(p(h_right), 0, sizeof(float) * nframes);
    }
  }


  char* tell_plugin(uint32_t argc, const char*const* argv) {
    
    // load a new sample
    if (argc == 2 && !strcmp(argv[0], "load_sample")) {
      if (load_sample(argv[1])) {
	cerr<<"Loaded sample from "<<argv[1]<<endl;
	return 0;
      }
      else
	return strdup((string("Failed to load ") + argv[1]).c_str());
    }
    
    // delete a loaded sample
    else if (argc == 2 && !strcmp(argv[0], "delete_sample")) {
      if (delete_sample(argv[1])) {
	cerr<<"Deleted sample "<<argv[1]<<endl;
	return 0;
      }
      else
	return strdup((string("Failed to delete ") + argv[1]).c_str());
    }
    
    // rename a loaded sample
    else if (argc == 3 && !strcmp(argv[0], "rename_sample")) {
      if (rename_sample(argv[1], argv[2])) {
	cerr<<"Renamed sample "<<argv[1]<<" to "<<argv[2]<<endl;
	return 0;
      }
      else
	return strdup((string("Failed to rename ") + argv[1]).c_str());
    }
    
    // add a splitpoint
    else if (argc == 3 && !strcmp(argv[0], "add_splitpoint")) {
      if (add_splitpoint(argv[1], atol(argv[2]))) {
	cerr<<"Added splitpoint "<<argv[2]<<" in sample "<<argv[1]<<endl;
	return 0;
      }
      else
	return strdup((string("Failed to add splitpoint ") + argv[2] + 
		       " in sample " + argv[1]).c_str());
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
    
    const SampleBuffer& buf = sample->get_processed_buffer();
    if (buf.get_channels() == 1)
      tell_host("ssifs", "sample_loaded", sample->get_name().c_str(), 
		long(buf.get_length()), buf.get_rate(), 
		buf.get_shm_name(0).c_str());
    else if (buf.get_channels() == 2)
      tell_host("ssifss", "sample_loaded", sample->get_name().c_str(), 
		long(buf.get_length()), buf.get_rate(), 
		buf.get_shm_name(0).c_str(), buf.get_shm_name(1).c_str());
    
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
  

  bool delete_sample(const std::string& name) {
    for (unsigned i = 0; i < m_samples.size(); ++i) {
      if (m_samples[i]->get_name() == name) {
	sem_wait(&m_lock);
	m_mixer.stop();
	const std::vector<Chunk*>& chunks = m_samples[i]->get_chunks();
	for (unsigned j = 0; j < chunks.size(); ++j)
	  m_trigger.remove_actions_for_chunk(chunks[j]);
	sem_post(&m_lock);
	delete m_samples[i];
	m_samples.erase(m_samples.begin() + i);
	tell_host("ss", "sample_deleted", name.c_str());
	return true;
      }
    }
    return false;
  }
  
  
  bool rename_sample(const std::string& old_name, const std::string& new_name) {
    
    // check that the new name isn't used already
    for (unsigned i = 0; i < m_samples.size(); ++i) {
      if (m_samples[i]->get_name() == new_name)
	return false;
    }
    
    for (unsigned i = 0; i < m_samples.size(); ++i) {
      if (m_samples[i]->get_name() == old_name) {
	m_samples[i]->set_name(new_name);
	tell_host("sss", "sample_renamed", old_name.c_str(), new_name.c_str());
	return true;
      }
    }
    
    return false;
  }
  
  
  bool add_splitpoint(const std::string& name, size_t frame) {
    for (unsigned i = 0; i < m_samples.size(); ++i) {
      if (m_samples[i]->get_name() == name) {
	bool success = false;
	sem_wait(&m_lock);
	m_mixer.stop();
	if (m_samples[i]->add_splitpoint(frame))
	  success = true;
	sem_post(&m_lock);
	if (success)
	  tell_host("ssi", "splitpoint_added", name.c_str(), frame);
	return success;
      }
    }
    return false;
  }
  
  
  Mixer m_mixer;
  ActionTrigger m_trigger;
  vector<Sample*> m_samples;
  
  // XXX this should be more fine-grained
  sem_t m_lock;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2_adv<Horizon>(h_uri);
}
