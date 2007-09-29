/****************************************************************************

    actiontriggermodel.hpp

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


#ifndef ACTIONTRIGGERMODEL_HPP
#define ACTIONTRIGGERMODEL_HPP

#include <string>
#include <map>

#include <sigc++/signal.h>


class ActionTriggerModel {
public:
  
  struct ActionModel {
    ActionModel(const std::string& s, const std::string& c) 
      : sample(s), chunk(c), first(255), last(255) {

    }
    std::string sample;
    std::string chunk;
    unsigned char first;
    unsigned char last;
  };
  
  
  /** Create a new ActionTriggerModel object. */
  ActionTriggerModel();
  
  ~ActionTriggerModel();
  
  /** Add a new action. */
  bool add_action(const std::string& name, const std::string& sample, 
		  const std::string& chunk);
  
  /** Map an action to a key range. */
  bool map_action(const std::string& name, 
		  unsigned char from, unsigned char to);
  
  /** Return the list of actions. */
  const std::map<std::string, ActionModel*>& get_actions() const;
  
  /** Remove an action. */
  bool remove_action(const std::string& name);
  
  /** Remove all actions that use a given chunk. */
  bool remove_actions_for_chunk(const std::string& sample, 
				const std::string& chunk);
  
  sigc::signal<void, const std::string&> signal_action_added;
  
  sigc::signal<void, const std::string&, unsigned char, unsigned char>
  signal_action_mapped;
  
  sigc::signal<void, const std::string&> signal_action_removed;
  
protected:
  
  std::map<std::string, ActionModel*> m_actions;
  
};


#endif
