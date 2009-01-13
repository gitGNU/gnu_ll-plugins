/****************************************************************************

    effectmodel.cpp

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

#include "effectmodel.hpp"


EffectModel::EffectModel(const std::string& name)
  : m_name(name),
    m_bypassed(false) {

}
  

const std::string& EffectModel::get_name() const {
  return m_name;
}


bool EffectModel::get_bypassed() const {
  return m_bypassed;
}
  

void EffectModel::set_name(const std::string& name) {
  m_name = name;
}


void EffectModel::set_bypassed(bool bypass) {
  m_bypassed = bypass;
}
