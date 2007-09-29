/****************************************************************************

    chunk.hpp

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

#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <string>
#include <vector>

#include <stdint.h>


class Envelope;
class EffectStack;
class Sample;


class Chunk {
public:
  
  Chunk(const Sample& sample, uint32_t start, uint32_t end, 
	const std::string& name);
  
  uint32_t get_start() const;
  
  uint32_t get_end() const;
  
  const Sample& get_sample() const;  

  const std::string& get_name() const;
  
protected:
  
  std::vector<Envelope*> m_envelopes;
  EffectStack* m_dyn_fx;
  const Sample& m_sample;
  uint32_t m_start;
  uint32_t m_end;
  std::string m_name;
};


#endif
