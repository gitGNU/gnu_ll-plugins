/****************************************************************************

    actiontrigger.hpp

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


#ifndef ACTIONTRIGGER_HPP
#define ACTIONTRIGGER_HPP

#include <vector>

#include "lv2-midiport.h"


class Action;
class Chunk;
class Mixer;


class ActionTrigger {
public:
  
  /** Create a new ActionTrigger object that uses the given @c mixer to play
      sound and run its actions. */
  ActionTrigger(Mixer& mixer);
  
  /** Read MIDI events from the given @c midi_port and execute actions. */
  void run(const LV2_MIDI* midi_port, uint32_t nframes);
  
  /** Add a new action. It should be allocated on the heap, and this 
      ActionTrigger object will take over ownership of the @c action object. */
  bool add_action(Action* action);
  
  /** Return the list of actions. */
  std::vector<Action*> get_actions();
  
  /** Map an action to a given MIDI key. To unmap an action, use key number 
      255. */
  bool map_action(Action* action, unsigned char key);
  
  /** Remove an action from the list and delete it. */
  bool remove_action(Action* action);
  
  /** Remove all actions that use a given chunk. */
  bool remove_actions_for_chunk(const Chunk* chunk);
  
protected:
  
  /** Used internally to hold information about a mapped action. */
  struct ActionInfo {
    ActionInfo() : action(0), running(false) { }
    Action* action;
    bool running;
  };
  
  std::vector<ActionInfo> m_key_table;
  std::vector<Action*> m_actions;
  Mixer& m_mixer;
};


#endif
