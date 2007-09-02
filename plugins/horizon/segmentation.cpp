/****************************************************************************

    segmentation.cpp

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

#include "segmentation.hpp"


Segmentation::Segmentation(size_t length) {
  m_segments.push_back(0);
  if (length > 0)
    m_segments.push_back(length);
}


const std::vector<size_t>& Segmentation::get_segments() const {
  return m_segments;
}


bool Segmentation::add_splitpoint(size_t frame) {
  if (frame >= m_segments[m_segments.size() - 1])
    return false;
  size_t i;
  for (i = 0; i < m_segments.size(); ++i) {
    if (m_segments[i] == frame)
      return false;
    else if (m_segments[i] > frame)
      break;
  }
  m_segments.insert(m_segments.begin() + i, frame);
  return true;
}


bool Segmentation::remove_splitpoint(size_t frame) {
  if (frame >= m_segments[m_segments.size() - 1] || frame == 0)
    return false;
  size_t i;
  for (i = 0; i < m_segments.size(); ++i) {
    if (m_segments[i] == frame)
      break;
    else if (m_segments[i] > frame)
      return false;
  }
  m_segments.erase(m_segments.begin() + i);
  return true;
}


bool Segmentation::move_splitpoint(size_t frame, size_t newframe) {
  if (frame >= m_segments[m_segments.size() - 1] || frame == 0)
    return false;
  size_t i;
  for (i = 1; i < m_segments.size() - 1; ++i) {
    if (m_segments[i] == frame)
      break;
    else if (m_segments[i] > frame)
      return false;
  }
  newframe = newframe <= m_segments[i - 1] ? m_segments[i - 1] + 1 : newframe;
  newframe = newframe >= m_segments[i + 1] ? m_segments[i + 1] - 1 : newframe;
  m_segments[i] = newframe;
  return true;
}

