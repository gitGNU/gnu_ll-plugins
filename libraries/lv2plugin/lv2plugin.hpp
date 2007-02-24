/****************************************************************************
    
    lv2plugin.hpp - support file for writing LV2 plugins in C++
    
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef LV2PLUGIN_HPP
#define LV2PLUGIN_HPP

#include <unistd.h>

#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include <lv2.h>


/** This is a base class for LV2 plugins. It has default implementations for
    all functions, so you only have to implement the functions that you need
    (for example run()). All subclasses must have a constructor that takes
    a single <code>uint32_t</code> as parameter, otherwise it will 
    not work with the template function register_lv2(). The host will use 
    this parameter to pass the sample rate when it creates a new instance of
    the plugin. 

    Here's an example of a plugin that simply copies the input to the output:

    @code
#include "lv2plugin.hpp"


class TestLV2 : public LV2Plugin {
public:
  TestLV2(uint32_t, const char*, const LV2_Host_Feature**) { }
  void run(uint32_t sample_count) {
    memcpy(m_ports[1], m_ports[0], sample_count * sizeof(float));
  }
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2<TestLV2>("http://ll-plugins.sf.net/plugins/TestLV2#0.0.0", 2);
}
    @endcode

    If the above code is compiled and linked with @c -llv2_plugin into a 
    shared module, it should be a fully functional (but not very useful) LV2 
    plugin.
    
    The line <code>void initialise() __attribute__((constructor));</code> tells
    GCC that the function @c initialise() should be executed when the plugin
    is loaded. You should use this method instead of naming the function
    @c _init(), which is deprecated. For other compilers you may have to do
    something different.
    
    If you want to be less compiler dependent you could do something like
    
    @code
static struct Init {
  Init() {
    register_lv2<TestLV2>("http://ll-plugins.sf.net/plugins/TestLV2#0.0.0", 2);
  }
} init;
    @endcode
*/
class LV2Plugin {
public:
  
  /** This constructor is needed to initialise the port vector with the
      correct number of ports. */
  LV2Plugin(uint32_t ports) : m_ports(ports, 0) { }
  
  /** We need a virtual destructor since we have virtual member functions. */
  virtual ~LV2Plugin() { }
  
  /** Connects the ports. You shouldn't have to override this, just use
      p(port) to access the port buffers. */
  virtual void connect_port(uint32_t port, void* data_location) {
    m_ports[port] = data_location;
  }
  
  /** Override this function if you need to do anything on activation. 
      This is always called before the host starts using the run() function.
      You should reset your plugin to it's initial state here. */
  virtual void activate() { }
  
  /** This is the process callback which should fill all output port buffers. 
      You most likely want to override it. */
  virtual void run(uint32_t sample_count) = 0;
  
  /** Override this function if you need to do anything on deactivation. 
      The host calls this when it does not plan to make any more calls to 
      run() (unless it calls activate() again). */
  virtual void deactivate() { }
  
protected:
  
  /** Use this as a shorthand to access and cast port buffers. */
  template <typename T> inline T* p(uint32_t port) {
    return reinterpret_cast<T*>(m_ports[port]);
  }
  
  /** This is needed because default template parameters aren't allowed for
      template functions. */
  inline float* p(uint32_t port) {
    return reinterpret_cast<float*>(m_ports[port]);
  }
  
  /** This vector contains pointers to all port buffers. You don't need to
      access it directly, use the p() function instead. */
  std::vector<void*> m_ports;
  
};


/* These functions are C wrappers for the C++ member functions. You should not
   use them directly. That's why they are intentionally documented without the
   Doxygen comment syntax. */
namespace LV2SupportFunctions {
  
  typedef std::vector<LV2_Descriptor> DescList;
  
  void connect_port(LV2_Handle instance, uint32_t port, 
                    void* data_location);
  void activate(LV2_Handle instance);
  void run(LV2_Handle instance, uint32_t sample_count);
  void deactivate(LV2_Handle instance);
  
  /* This function returns the list of LV2 descriptors. It should only be 
     used internally. */
  DescList& get_lv2_descriptors();

  /* This template function creates an instance of a plugin. It is used as
     the instantiate() callback in the LV2 descriptor. You should not use
     it directly. */
  template <class T>
  LV2_Handle create_plugin_instance(const LV2_Descriptor* descriptor,
                                    uint32_t sample_rate,
                                    const char* bundle_path,
                                    const LV2_Host_Feature* const* host_features) {
    T* t = new T(sample_rate, bundle_path, host_features);
    return reinterpret_cast<LV2_Handle>(t);
  }
  
  /* This function destroys an instance of a plugin. It is used as the
     cleanup() callback in the LV2 descriptor. You should not use it
     directly. */
  void delete_plugin_instance(LV2_Handle instance);
  
}  
  
/** This is the function you should use to register your LV2 plugin class.
    It should be called when the library is loaded, so you can write an
    initialisation function with the @c constructor attribute and put it there. 
    Since this is a template function but the template type isn't one of 
    the parameters, you need to give it explicitly, like this:
    
    @code
    register_lv2<MyPluginClass>("http://ll-plugins.sf.net/MyPlugin");
    @endcode
    
    @param uri The unique LV2 URI for this plugin.
*/
template <class T>
size_t register_lv2(const std::string& uri) {
  using namespace LV2SupportFunctions;
  LV2_Descriptor desc;
  std::memset(&desc, 0, sizeof(LV2_Descriptor));
  char* c_uri = new char[uri.size() + 1];
  std::memcpy(c_uri, uri.c_str(), uri.size() + 1);
  desc.URI = c_uri;
  desc.instantiate = &create_plugin_instance<T>;
  desc.connect_port = &connect_port;
  desc.activate = &activate;
  desc.run = &run;
  desc.deactivate = &deactivate;
  desc.cleanup = &delete_plugin_instance;
  get_lv2_descriptors().push_back(desc);
  return get_lv2_descriptors().size() - 1;
}


#endif
