/****************************************************************************
    
    lv2instrument.hpp - support file for writing LV2 plugins in C++
                        using the Instrument extension
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef LV2INSTRUMENT_HPP
#define LV2INSTRUMENT_HPP

#include <unistd.h>

#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "lv2plugin.hpp"
#include "lv2-instrument.h"


class LV2Instrument : public LV2Plugin {
public:
  
  /** This constructor is needed to initialise the port vector with the
      correct number of ports. */
  LV2Instrument(uint32_t ports) : LV2Plugin(ports) { }
  
  /** We need a virtual destructor since we have virtual member functions. */
  virtual ~LV2Instrument() { }
  
  /** Arbitrary configuration function without RT constraints. */
  virtual char* configure(const char* key, const char* value) { return 0; }
  
  /** Function for loading data from external files. */
  virtual char* set_file(const char* key, const char* filename) { return 0; }
  
protected:
  
};


/* These functions are C wrappers for the C++ member functions. You should not
   use them directly. */
namespace LV2SupportFunctions {
  
  
  
  
  
  char* configure(LV2_Handle instance, const char* key, const char* value);
  
  unsigned int list_used_files(LV2_Handle instance, 
                               char*** keys, char*** filepaths);
  
  void* lv2_instrument_descriptor(const char* URI);
  
  
  /* This template function creates an instance of a plugin. It is used as
     the instantiate() callback in the LV2 descriptor. You should not use
     it directly. It will return 0 if the host does not provide the 
     Instrument extension. */
  template <class T>
  LV2_Handle create_instrument_instance(const LV2_Descriptor* descriptor,
                                        double sample_rate,
                                        const char* bundle_path,
                                        const LV2_Host_Feature* const* host_features) {
    // check that the host supports the Instrument extension
    size_t i;
    for (i = 0; host_features[i]; ++i) {
      if (!std::strcmp(host_features[i]->URI, 
                       "<http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext>"))
        break;
    }
    if (!host_features[i])
      return 0;
    
    // create and return an instance of the plugin
    T* t = new T(sample_rate, bundle_path, host_features);
    return reinterpret_cast<LV2_Handle>(t);
  }

  
  /* This template function creates an instance of a plugin. It is used as
     the instantiate() callback in the LV2 descriptor. You should not use
     it directly. This function will return a non-NULL handle even if the host
     does not provide the Instrument extension. */
  template <class T>
  LV2_Handle create_optional_instrument_instance(const LV2_Descriptor* descriptor,
                                        double sample_rate,
                                        const char* bundle_path,
                                        const LV2_Host_Feature* const* host_features) {
    // create and return an instance of the plugin
    T* t = new T(sample_rate, bundle_path, host_features);
    return reinterpret_cast<LV2_Handle>(t);
  }
  
  
}  
  
/** This is the function you should use to register your LV2 instrument class.
    It should be called when the library is loaded, so you can write an
    initialisation function with the constructor attribute and put it there. 
    Since this is a template function but the template type isn't one of 
    the parameters, you need to give it explicitly like this:
    
    @code
    register_lv2_inst<MyInstrumentClass>("http://ll-plugins.sf.net/MyPlugin");
    @endcode
    
    @param uri The unique LV2 URI for this plugin.
*/
template <class T>
size_t register_lv2_inst(const std::string& uri, bool optional = false) {
  using namespace LV2SupportFunctions;
  
  LV2_Descriptor desc;
  std::memset(&desc, 0, sizeof(LV2_Descriptor));
  char* c_uri = new char[uri.size() + 1];
  std::memcpy(c_uri, uri.c_str(), uri.size() + 1);
  desc.URI = c_uri;
  if (!optional)
    desc.instantiate = &create_instrument_instance<T>;
  else
    desc.instantiate = &create_optional_instrument_instance<T>;
  desc.connect_port = &connect_port;
  desc.activate = &activate;
  desc.run = &run;
  desc.deactivate = &deactivate;
  desc.cleanup = &delete_plugin_instance;
  desc.extension_data = &lv2_instrument_descriptor;
  get_lv2_descriptors().push_back(desc);
  
  return get_lv2_descriptors().size() - 1;
}


#endif
