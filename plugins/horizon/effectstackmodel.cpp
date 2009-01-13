/****************************************************************************

    effectstackmodel.cpp

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

#include "effectstackmodel.hpp"


EffectStackModel::EffectStackModel() {
  
}


bool EffectStackModel::insert_effect(size_t pos, const std::string& name) {
  if (pos > m_effects.size())
    return false;
  m_effects.insert(m_effects.begin() + pos, EffectModel(name));
  return true;
}
  

bool EffectStackModel::remove_effect(size_t pos) {
  if (pos >= m_effects.size())
    return false;
  m_effects.erase(m_effects.begin() + pos);
  return true;
}


bool EffectStackModel::bypass_effect(size_t pos, bool bypass) {
  if (pos >= m_effects.size())
    return false;
  m_effects[pos].set_bypassed(bypass);
  return true;
}


const std::vector<EffectModel>& EffectStackModel::get_effects() const {
  return m_effects;
}
