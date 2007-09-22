/****************************************************************************

    mixer.hpp

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

#ifndef MIXER_HPP
#define MIXER_HPP

#include "sample.hpp"
#include "voice.hpp"


/** The Mixer class handles all mixing of voices and writing of audio
    to the output buffers. */
class Mixer {
public:
  
  /** Create a new Mixer object. */
  Mixer();
  
  /** Set the buffers to write to. */
  void set_buffers(float* left, float* right);
  
  /** Write @c nframes of audio to the output buffers (do not call this
      without first calling set_buffers() with pointers to buffers
      that are large enough to hold @c nframes frames of audio). */
  void run(uint32_t nframes);
  
  void play_chunk(const Chunk* chunk);
  
  void play_preview(const SampleBuffer& buffer, uint32_t start, uint32_t end);
  
  void stop();

protected:
  
  float* m_left;
  float* m_right;
  float m_gain;
  
  Voice m_voice;
};


#endif
