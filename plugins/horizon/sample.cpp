/****************************************************************************

    sample.cpp

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

#include "chunk.hpp"
#include "sample.hpp"
#include "samplebuffer.hpp"
#include "segmentation.hpp"


Sample::Sample(const std::string& filename)
  : m_orig_sample(filename),
    m_seg(m_orig_sample.get_length()),
    m_proc_sample(0) {
  
  if (is_valid()) {
    for (unsigned i = filename.length() - 1; i >= 0; --i) {
      if (filename[i] == '/') {
	m_name = filename.substr(i + 1);
	break;
      }
    }
  }
  
}


Sample::~Sample() {
  if (m_proc_sample) {
    delete m_proc_sample;
  }
}


bool Sample::is_valid() const {
  return m_orig_sample.is_valid();
}
  

const Segmentation& Sample::get_segmentation() const {
  return m_seg;
}


const SampleBuffer& Sample::get_processed_buffer() const {
  if (m_proc_sample)
    return *m_proc_sample;
  return m_orig_sample;
}


const EffectStack& Sample::get_effect_stack() const {
  return m_static_fx;
}


const std::vector<Chunk*>& Sample::get_chunks() const {
  return m_chunks;
}
  

const std::string& Sample::get_name() const {
  return m_name;
}


const Chunk* Sample::find_chunk(const std::string& name) const {
  for (unsigned i = 0; i < m_chunks.size(); ++i) {
    if (m_chunks[i]->get_name() == name)
      return m_chunks[i];
  }
  return 0;
}


void Sample::set_name(const std::string& name) {
  m_name = name;
}


bool Sample::add_splitpoint(size_t frame) {
  return m_seg.add_splitpoint(frame);
}


bool Sample::remove_splitpoint(size_t frame) {
  return m_seg.remove_splitpoint(frame);
}


bool Sample::move_splitpoint(size_t frame, size_t newframe) {
  return m_seg.move_splitpoint(frame, newframe);
}


const Effect* Sample::add_static_effect(size_t pos, 
					const std::string& effect_uri) {
  const Effect* e = 0;
  if ((e = m_static_fx.add_effect(effect_uri, pos))) {
    apply_effect_stack();
    return e;
  }
  return 0;
}


bool Sample::remove_static_effect(size_t pos) {
  if (m_static_fx.remove_effect(pos)) {
    apply_effect_stack();
    return true;
  }
  return false;
}


bool Sample::bypass_static_effect(size_t pos, bool bypass) {
  if (m_static_fx.bypass_effect(pos, bypass)) {
    apply_effect_stack();
    return true;
  }
  return false;
}


bool Sample::add_chunk(size_t first, size_t last, const std::string& name) {
  if (first <= last && last + 1 < m_seg.get_segments().size()) {
    m_chunks.push_back(new Chunk(*this, first, last + 1, name));
    return true;
  }
  return false;
}


void Sample::apply_effect_stack() {
  SampleBuffer* new_buf = new SampleBuffer(m_orig_sample);
  m_static_fx.process(*new_buf, *new_buf);
  // XXX can't do this when we're playing the buffer
  delete m_proc_sample;
  m_proc_sample = new_buf;
}

