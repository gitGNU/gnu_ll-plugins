/****************************************************************************
    
    lv2plugin.cpp - support file for writing LV2 plugins in C++
    
    Copyright (C) 2006  Lars Luthman <larsl@users.sourceforge.net>
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#include <cassert>

#include "lv2plugin.hpp"


using namespace std;


namespace LV2SupportFunctions {

  void connect_port(LV2_Handle instance, unsigned long port, 
                    void* data_location) {
    reinterpret_cast<LV2Plugin*>(instance)->connect_port(port, data_location);
  }


  void activate(LV2_Handle instance) {
    reinterpret_cast<LV2Plugin*>(instance)->activate();
  }


  void run(LV2_Handle instance, unsigned long sample_count) {
    reinterpret_cast<LV2Plugin*>(instance)->run(sample_count);
  }
  
  
  void deactivate(LV2_Handle instance) {
    reinterpret_cast<LV2Plugin*>(instance)->deactivate();
  }

  DescList& get_lv2_descriptors() {
    static vector<pair<LV2_Descriptor, unsigned long> > descriptors;
    return descriptors;
  }

  void delete_plugin_instance(LV2_Handle instance) {
    delete reinterpret_cast<LV2Plugin*>(instance);
  }
  
}
  
extern "C" {
  const LV2_Descriptor* lv2_descriptor(unsigned long index) {
    using namespace LV2SupportFunctions;
    if (index < get_lv2_descriptors().size())
      return &get_lv2_descriptors()[index].first;
    return NULL;
  }
}


