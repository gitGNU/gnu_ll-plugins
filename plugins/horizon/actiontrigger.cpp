#include "action.hpp"
#include "actiontrigger.hpp"
#include "lv2-midifunctions.h"
#include "mixer.hpp"

  
ActionTrigger::ActionTrigger(Mixer& mixer)
  : m_key_table(128),
    m_mixer(mixer) {

}


void ActionTrigger::run(const LV2_MIDI* midi_port, uint32_t nframes) {
  
  static unsigned char keymask = ~(1<<7);

  LV2_MIDIState state = { const_cast<LV2_MIDI*>(midi_port), nframes, 0 };
  
  double timestamp;
  uint32_t size;
  unsigned char* data;
  uint32_t pos = 0;
  
  do {
    
    lv2midi_get_event(&state, &timestamp, &size, &data);
    lv2midi_step(&state);
    
    // do the audio processing for (timestamp - pos) frames here
    m_mixer.run(uint32_t(timestamp - pos));
    
    if (timestamp < nframes && size == 3) {
      unsigned char type = data[0] & 0xF0;
      unsigned char key = data[1] & keymask;
      if (type == 0x90 && m_key_table[key].action) {
	m_mixer.play_chunk(&m_key_table[key].action->get_chunk());
	m_key_table[key].running = true;
	// Note On, start the action!
      }
      else if (type == 0x80 && m_key_table[key].running) {
	m_mixer.stop();
	m_key_table[key].running = false;
	// Note Off, stop the action!
      }
    }
    
    pos = uint32_t(timestamp);
    
  } while (timestamp < nframes);
  
}


bool ActionTrigger::add_action(Action* action) {
  m_actions.push_back(action);
  return true;
}
  

std::vector<Action*> ActionTrigger::get_actions() {
  return m_actions;
}

  
bool ActionTrigger::map_action(Action* action, unsigned char key) {
  m_key_table[key].action = action;
  m_key_table[key].running = false;
  return true;
}

  
