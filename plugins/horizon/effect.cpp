/****************************************************************************

    effect.cpp

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

#include "effect.hpp"


using namespace std;


Effect::Effect(const std::string& name)
  : m_name(name) {
  
}
  

const vector<float>& Effect::get_parameters() const {
  return m_params;
}


const string& Effect::get_name() const {
  return m_name;
}


bool Effect::set_parameter(size_t index, float value) {
  if (index >= m_params.size())
    return false;
  m_params[index] = value;
  return true;
}


void Effect::set_name(const std::string& name) {
  m_name = name;
}


