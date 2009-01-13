/****************************************************************************

    sample.hpp

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

#ifndef SAMPLE_HPP
#define SAMPLE_HPP

#include <string>
#include <vector>

#include "samplebuffer.hpp"
#include "segmentation.hpp"
#include "effectstack.hpp"


class Chunk;


class Sample {
public:
  
  Sample(const std::string& filename);
  ~Sample();
  
  bool is_valid() const;
  
  const Segmentation& get_segmentation() const;
  const SampleBuffer& get_processed_buffer() const;
  const EffectStack& get_effect_stack() const;
  const std::vector<Chunk*>& get_chunks() const;
  const std::string& get_name() const;
  const Chunk* find_chunk(const std::string& name) const;
  
  void set_name(const std::string& name);
  bool add_splitpoint(size_t frame);
  bool remove_splitpoint(size_t frame);
  bool move_splitpoint(size_t frame, size_t newframe);
  const Effect* add_static_effect(size_t pos, const std::string& effect_uri);
  bool remove_static_effect(size_t pos);
  bool bypass_static_effect(size_t pos, bool bypass); 
  bool add_chunk(size_t first, size_t last, const std::string& name);
  
protected:
  
  void apply_effect_stack();
  
  SampleBuffer m_orig_sample;
  Segmentation m_seg;
  EffectStack m_static_fx;
  SampleBuffer* m_proc_sample;
  std::vector<Chunk*> m_chunks;
  std::string m_name;
  
};


#endif
