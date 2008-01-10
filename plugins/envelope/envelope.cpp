/****************************************************************************
    
    envelope.cpp - envelope generator plugin
    
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

#include <cmath>
#include <cstring>
#include <iostream>

#include "envelope.hpp"
#include "lv2plugin.hpp"
#include "envelope.peg"


class EnvelopePlugin : 
  public LV2::Plugin<EnvelopePlugin, LV2::CommandExt<true> > {
public:
  
  EnvelopePlugin(double rate) 
    : LV2::Plugin<EnvelopePlugin, LV2::CommandExt<true> >(e_n_ports),
      m_eg(uint32_t(rate)) {
    
  }
  
  
  void run(uint32_t nframes) {
    for (uint32_t i = 0; i < nframes; ++i)
      p(e_output)[i] = m_eg.run(*p(e_attack), *p(e_decay), 
				*p(e_sustain), *p(e_release));
  }
  
  char* command(uint32_t argc, const char* const* argv) {
    if (argc == 2 && !strcmp("envelope", argv[0])) {
      m_eg.set_string(argv[1]);
      feedback(2, argv);
      return 0;
    }
    return strdup("Unknown command");
  }
  
protected:
  
  Envelope m_eg;
  
};


static unsigned _ = EnvelopePlugin::register_class(e_uri);
