/****************************************************************************
    
    pdoscillator.cpp - A phase distortion oscillator
    
    Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA

****************************************************************************/

#ifndef PDOSCILLATOR_HPP
#define PDOSCILLATOR_HPP

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include <stdint.h>


class PDOscillator {
public:
  
  inline PDOscillator(uint32_t rate) 
    : m_inv_rate(1.0 / rate),
      m_phase(0) {
    
    m_points.push_back(Point(0, 0, 0));
    m_points.push_back(Point(0.5, 0.1, 0.5));
    m_points.push_back(Point(1, 1, 1));
  }
  
  
  inline float run(float freq, float dist) {
    
    m_phase += m_inv_rate * freq;
    if (m_phase > 1)
      m_phase -= 1;
    
    float real_phase;
    for (int s = 1; s < m_points.size(); ++s) {
      const Point& p = m_points[s];
      float xa = p.o * (1 - dist) + p.x * dist;
      if (m_phase < xa) {
        const Point& p0 = m_points[s - 1];
        float ya = p.o * (1 - dist) + p.y * dist;
        float y0a = p0.o * (1 - dist) + p0.y * dist;
        float x0a = p0.o * (1 - dist) + p0.x * dist;
        real_phase = y0a + (m_phase - x0a) * (ya - y0a) / (xa - x0a);
        return -std::cos(real_phase * 2 * M_PI);
      }
    }
    
    return 0;
  }
  
  
  inline bool set_string(const std::string& str) {
    std::istringstream iss(str);
    std::vector<Point> new_points;
    
    while (iss.good()) {
      double o, x, y;
      iss>>o>>x>>y>>std::ws;
      new_points.push_back(Point(o, x, y));
    }
    
    if (new_points.size() > 1 && new_points[0].o == 0 && 
        new_points[0].x == 0 && new_points[0].y == 0 && 
        new_points[new_points.size() - 1].o == 1 && 
        new_points[new_points.size() - 1].x == 1 && 
        new_points[new_points.size() - 1].y == 1) {
      m_points = new_points;
      return true;
    }
    
    return false;
  }
  
  
protected:
  
  struct Point {
    Point(float _o, float _x, float _y) : o(_o), x(_x), y(_y) { }
    float o;
    float x;
    float y;
  };
  
  
  std::vector<Point> m_points;
  
  float m_inv_rate;
  float m_phase;
  
};


#endif
