/****************************************************************************

    effectstack.cpp

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

#include <cstring>

#include "normaliseeffect.hpp"
#include "reverseeffect.hpp"
#include "effectstack.hpp"
#include "samplebuffer.hpp"


const std::vector<EffectStack::EffectInfo>& EffectStack::get_effects() const {
  return m_effects;
}

  
std::vector<EffectStack::EffectInfo>& EffectStack::get_effects() {
  return m_effects;
}

  
const Effect* EffectStack::add_effect(const std::string& uri, size_t index) {
  if (index > m_effects.size())
    return 0;

  if (uri == "reverse") {
    m_effects.insert(m_effects.begin() + index, 
		     EffectInfo(new ReverseEffect));
    return m_effects[index].effect;
  }
  
  else if (uri == "normalise") {
    m_effects.insert(m_effects.begin() + index,
		     EffectInfo(new NormaliseEffect));
    return m_effects[index].effect;
  }
  
  return 0;
}


bool EffectStack::remove_effect(size_t index) {
  if (index >= m_effects.size())
    return false;
  delete m_effects[index].effect;
  m_effects.erase(m_effects.begin() + index);
  return true;
}


bool EffectStack::bypass_effect(size_t index, bool bypass) {
  if (index >= m_effects.size())
    return false;
  m_effects[index].bypassed = bypass;
  return true;
}

  
void EffectStack::process(const float* input, float* output, size_t nframes) {
  const float* data = input;
  for (size_t i = 0; i < m_effects.size(); ++i) {
    if (!m_effects[i].bypassed) {
      m_effects[i].effect->process(data, output, nframes);
      data = output;
    }
  }
}


void EffectStack::process(const SampleBuffer& input, SampleBuffer& output) {
  process(input.get_data(0), output.get_data(0), input.get_length());
  if (input.get_channels() > 1 && output.get_channels() > 1)
    process(input.get_data(1), output.get_data(1), input.get_length());
}


