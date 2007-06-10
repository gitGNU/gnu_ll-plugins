#ifndef ACTIONTRIGGER_HPP
#define ACTIONTRIGGER_HPP

#include <vector>

#include "lv2-midiport.h"


class Action;
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
