/****************************************************************************

    actiontriggermodel.cpp

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

#include "actiontriggermodel.hpp"


using namespace sigc;
using namespace std;


ActionTriggerModel::ActionTriggerModel() {

}


ActionTriggerModel::~ActionTriggerModel() {
  map<string, ActionModel*>::iterator iter;
  for (iter = m_actions.begin(); iter != m_actions.end(); ++iter)
    delete iter->second;
}

  
bool ActionTriggerModel::add_action(const std::string& name, 
				    const std::string& sample, 
				    const std::string& chunk) {
  map<string, ActionModel*>::iterator iter = m_actions.find(name);
  if (iter != m_actions.end()) {
    delete iter->second;
    m_actions.erase(iter);
    signal_action_removed(name);
  }
  m_actions[name] = new ActionModel(sample, chunk);
  signal_action_added(name);
  return true;
}

  
bool ActionTriggerModel::map_action(const std::string& name, 
				    unsigned char from, unsigned char to) {
  map<string, ActionModel*>::iterator iter = m_actions.find(name);
  if (iter != m_actions.end() && to >= from && to < 128) {
    iter->second->first = from;
    iter->second->last = to;
    signal_action_mapped(name, from, to);
    return true;
  }
  return false;
}
  

const std::map<std::string, ActionTriggerModel::ActionModel*>& 
ActionTriggerModel::get_actions() const {
  return m_actions;
}
  

bool ActionTriggerModel::remove_action(const std::string& name) {
  map<string, ActionModel*>::iterator iter = m_actions.find(name);
  if (iter != m_actions.end()) {
    delete iter->second;
    m_actions.erase(iter);
    signal_action_removed(name);
    return true;
  }
  return false;
}
  

bool ActionTriggerModel::remove_actions_for_chunk(const std::string& sample, 
						  const std::string& chunk) {
  map<string, ActionModel*>::iterator iter;
  for (iter = m_actions.begin(); iter != m_actions.end(); ) {
    if (iter->second->sample == sample && iter->second->chunk == chunk) {
      map<string, ActionModel*>::iterator iter2 = iter;
      ++iter;
      string name = iter2->first;
      delete iter2->second;
      m_actions.erase(iter2);
      signal_action_removed(name);
    }
    else
      ++iter;
  }
}
