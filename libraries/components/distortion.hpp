/****************************************************************************
    
    distortion.hpp - A distortion effect
    
    Copyright (C) 2007  Lars Luthman <lars.luthman@gmail.com>

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

#ifndef DISTORTION_HPP
#define DISTORTION_HPP

#include <cmath>

#include <stdint.h>


namespace {


  class Distortion {
  public:
  
    Distortion(uint32_t rate);
    ~Distortion();
  
    void run(float* data, uint32_t nframes, float toggle,
	     float drive, float set, float tone, float mix);
  
  private:
  
  };


  class StereoDistortion {
  public:
  
    StereoDistortion(uint32_t rate);
    ~StereoDistortion();
  
    void run(float* left, float* right, uint32_t nframes, float toggle,
	     float drive, float set, float tone, float mix);
  
  private:
  
  };

  
  Distortion::Distortion(uint32_t rate) {

  }


  Distortion::~Distortion() {

  }


  void Distortion::run(float* data, uint32_t nframes, float toggle, 
		       float drive, float set, float tone, float mix) {

  }



  StereoDistortion::StereoDistortion(uint32_t rate) {

  }


  StereoDistortion::~StereoDistortion() {

  }


  void StereoDistortion::run(float* left, float* right, uint32_t nframes, 
			     float toggle, float drive, float set, 
			     float tone, float mix) {

  }

}


#endif
