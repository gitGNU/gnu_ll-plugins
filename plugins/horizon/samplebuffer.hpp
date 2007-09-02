/****************************************************************************

    samplebuffer.hpp

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

#ifndef SAMPLEBUFFER_HPP
#define SAMPLEBUFFER_HPP

#include <string>


class SampleBuffer {
public:
  
  SampleBuffer(const std::string& filename);
  SampleBuffer(const SampleBuffer& buf);
  ~SampleBuffer();
  
  bool is_valid() const;
  
  const float* get_data(size_t channel) const;
  float* get_data(size_t channel);
  size_t get_length() const;
  float get_rate() const;
  size_t get_channels() const;
  const std::string& get_shm_name(size_t channel) const;
  
protected:
  
  float* shm_alloc(size_t channel, size_t floats);
  void shm_free(size_t channel);
  
  size_t m_length;
  float m_rate;
  size_t m_channels;
  float** m_data;
  std::string* m_shm_names;
  
};


#endif
