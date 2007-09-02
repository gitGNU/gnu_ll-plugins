/****************************************************************************

    actiontrigger.cpp

    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#include "action.hpp"
#include "actiontrigger.hpp"
#include "lv2-midifunctions.h"
#include "mixer.hpp"

  
ActionTrigger::ActionTrigger(Mixer& mixer)
  : m_key_table(128),
    m_mixer(mixer) {

}


void ActionTrigger::run(const LV2_MIDI* midi_port, uint32_t nframes) {
  
  static unsigned char keymask(~(1<<7));

  LV2_MIDIState state = { const_cast<LV2_MIDI*>(midi_port), nframes, 0 };
  
  double timestamp;
  uint32_t size;
  unsigned char* data;
  uint32_t pos = 0;
  
  do {
    
    lv2midi_get_event(&state, &timestamp, &size, &data);
    lv2midi_step(&state);
    
    // do the audio processing for (timestamp - pos) frames
    m_mixer.run(uint32_t(timestamp - pos));
    
    if (timestamp < nframes && size == 3) {
      unsigned char type = data[0] & 0xF0;
      unsigned char key = data[1] & keymask;
      if (type == 0x90 && m_key_table[key].action) {
	for (unsigned i = 0; i < m_key_table.size(); ++i)
	  m_key_table[i].running = false;
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


bool ActionTrigger::remove_action(Action* action) {
  for (unsigned i = 0; i < 128; ++i) {
    if (m_key_table[i].action == action) {
      m_key_table[i].action = 0;
      // XXX probably have to do something here if the action is running
      m_key_table[i].running = false;
    }
  }
  for (unsigned i = 0; i < m_actions.size(); ++i) {
    if (m_actions[i] == action) {
      delete action;
      m_actions.erase(m_actions.begin() + i);
      return true;
    }
  }
  
  return false;
}
  

bool ActionTrigger::remove_actions_for_chunk(const Chunk* chunk) {
  
  bool something_removed = false;
  
  for (unsigned i = 0; i < 128; ++i) {
    if (m_key_table[i].action && &m_key_table[i].action->get_chunk() == chunk) {
      m_key_table[i].action = 0;
      // XXX here too
      m_key_table[i].running = false;
    }
  }
  
  for (long i = 0; i < (long)m_actions.size(); ++i) {
    if (&m_actions[i]->get_chunk() == chunk) {
      delete m_actions[i];
      m_actions.erase(m_actions.begin() + i);
      --i;
      something_removed = true;
    }
  }
  
  return something_removed;
}

