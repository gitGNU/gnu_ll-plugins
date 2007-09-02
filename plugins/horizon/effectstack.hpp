/****************************************************************************

    effectstack.hpp

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

#ifndef EFFECTSTACK_HPP
#define EFFECTSTACK_HPP

#include <vector>


class Effect;
class SampleBuffer;

class EffectStack {
public:
  
  struct EffectInfo {

    EffectInfo(Effect* e)
      : effect(e),
	bypassed(false) {

    }
    
    Effect* effect;
    bool bypassed;
  };
  
  const std::vector<EffectInfo>& get_effects() const;
  
  std::vector<EffectInfo>& get_effects();
  
  const Effect* add_effect(const std::string& uri, size_t index);
  bool remove_effect(size_t index);
  bool bypass_effect(size_t index, bool bypass);
  
  void process(const float* input, float* output, size_t nframes);
  void process(const SampleBuffer& input, SampleBuffer& output);
  
protected:
  
  std::vector<EffectInfo> m_effects;
  
};


#endif
