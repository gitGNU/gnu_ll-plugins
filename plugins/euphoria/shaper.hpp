/****************************************************************************
    
    shaper.hpp - A waveshaper
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
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

#ifndef SHAPER_HPP
#define SHAPER_HPP

#include <string>
#include <vector>


class Shaper {
public:
  
  Shaper(unsigned int rate);
  ~Shaper();
  
  float run(float input, float max_freq);
  
  bool set_string(const std::string& str);
  
protected:
  
  static const int WAVETABLE_SIZE = 65536;
  
  static double function(double x, void* params);
  
  typedef float *float_ptr;
  float_ptr m_tables[10];
  unsigned m_rate;
  
};


#endif
