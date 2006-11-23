/************************************************************************

   This file is partly based on dssi.h from DSSI 0.9, which is
   Copyright (C) 2004 Chris Cannam, Steve Harris and Sean Bolton

   Modifications (C) 2006 Lars Luthman <lars.luthman@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA.

*************************************************************************/

#ifndef LV2_INSTRUMENT_H
#define LV2_INSTRUMENT_H

#include <lv2.h>

#ifdef __cplusplus
extern "C" {
#endif


/* This header defines data structures used by LV2 hosts and plugins that
   support the LV2 Instrument extension with URI 
   <http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext>.
   
   This extension adds two things to LV2 plugins:
   
   * A configure() callback that can be used by the host to pass arbitrary
     keys and values to the plugin, that the plugin can use to do anything
     that is hard to do with control rate input ports or that require some 
     non-realtime safe computations
     
   * A set_file() callback that can be used by the host to pass filenames
     associated with keys to the plugin
     
   The data member in the LV2_Host_Feature struct for this extension, 
   passed to the plugin's instantiate() function, should be set to NULL.

   A plugin that supports this extension must have a non-NULL extension_data()
   function pointer in its descriptor, that returns a pointer to an 
   LV2_InstrumentDescriptor, defined below, when called with the URI 
   "http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext". This 
   LV2_InstrumentDescriptor object contains function pointers that 
   implement the "instrument" part of the plugin.
*/



/** This struct contains all callbacks used in this extension. */
typedef struct {

  
  /** This member is a function pointer that sends a piece of
      configuration data to the plugin. The key argument specifies
      some aspect of the synth's configuration that is to be changed,
      and the value argument specifies a new value for it.  A plugin
      that does not require this facility at all may set this member
      to NULL.
      
      This call is intended to set some session-scoped aspect of a
      plugin's behaviour that can not be done conveniently by changing
      port values, or requires non-realtime safe computations when set.
      The plugin should act immediately on the request. The call should
      return NULL on success, or an error string that may be shown to 
      the user.  The host will free the returned value after use if it 
      is non-NULL. If the plugin encounters an error and returns a non-NULL
      value it should not change its behaviour (e.g. by continuing 
      to use any previous value associated to that key).
      
      Calls to configure() are not automated as timed events.
      Instead, a host should remember the last value associated with
      each key passed to configure() during a given session for a
      given plugin instance, and should call configure() with the
      correct value for each key the next time it instantiates the
      "same" plugin instance, for example on reloading a project in
      which the plugin was used before. If multiple keys are used, 
      the order in which their values are set should not matter to the
      plugin. Only successful calls to configure() (i.e. ones returning
      NULL) should be remembered by the host.
      
      This mechanism should NOT be used for loading external resources from
      files, such as samples or patches. If a plugin needs to load external
      data it should use the set_file() member.
      
      The plugin may be told to stop using a value associated with a key
      by calling this function with that key and an empty string as the
      value parameter. The host may then forget about that key altogether.
      
      This function does not need to be realtime safe. All parameters must
      be non-NULL.
  */
  char* (*configure)(LV2_Handle instance, 
                     const char* key, 
                     const char* value);
  
  
  /** This member is a function pointer that sends a filename to the plugin.
      The key parameter specifies what the file should be used for (using some
      protocol defined by the plugin author) and the filename parameter is
      the name of the file. A plugin that does not need to load external data
      files may set this member to NULL.
      
      The filename parameter should be the name of a file that can be opened
      in read-only mode using the standard C library function fopen(), for
      example 'fopen(filename, "rb")'. The plugin is NOT allowed to write to
      the file or even open it for writing.
      
      A call to this function should return NULL on success, or an error 
      string that may be shown to the user. The host will free the returned 
      value after use if it is non-NULL. If the plugin encounters an error 
      and returns a non-NULL value it should not change its behaviour (e.g. 
      by continuing to use any previous file associated to that key).
      
      Calls to set_file() are not automated as timed events. Instead, a
      host should remember the last filename associated with each key 
      passed to set_file() during a given session for a given plugin 
      instance, and should call set_file() with the correct filename for 
      each key the next time it instantiates the "same" plugin instance, 
      for example on reloading a project in which the plugin was used 
      before. If multiple keys are used, the order in which their filenames 
      are set should not matter to the plugin. Only successful calls to 
      set_file() (i.e. ones returning NULL) should be remembered by the host.
      
      A plugin can be told to stop using a file associated to a key by
      calling this function with that key and an empty string as the filename
      parameter. The host may then forget about that key altogether.
      
      This function does not need to be realtime safe. All parameters must 
      be non-NULL.
  */
  char* (*set_file)(LV2_Handle instance,
                    const char* key,
                    const char* filename);


} LV2_InstrumentDescriptor;


#ifdef __cplusplus
}
#endif


#endif
