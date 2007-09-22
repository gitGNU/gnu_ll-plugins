/****************************************************************************

    mixer.cpp

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
#include <iostream>

#include "mixer.hpp"


using namespace std;


Mixer::Mixer()
  : m_left(0),
    m_right(0),
    m_gain(0) {
  
}


void Mixer::set_buffers(float* left, float* right) {
  m_left = left;
  m_right = right;
}


void Mixer::run(uint32_t nframes) {
  memset(m_left, 0, nframes * sizeof(float));
  memset(m_right, 0, nframes * sizeof(float));
  m_voice.render_to(m_left, m_right, nframes, m_gain);
  m_left += nframes;
  m_right += nframes;
}


void Mixer::play_chunk(const Chunk* chunk) {
  m_voice.set_chunk(chunk);
  m_gain = 1;
}


void Mixer::play_preview(const SampleBuffer& buffer, 
			 uint32_t start, uint32_t end) {
  m_voice.set_region(buffer, start, end);
  m_gain = 0.5;
}


void Mixer::stop() {
  m_voice.set_chunk(0);
  m_gain = 0;
}
