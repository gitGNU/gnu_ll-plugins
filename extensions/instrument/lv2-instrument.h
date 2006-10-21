/************************************************************************

   This file is partly based on dssi.h from DSSI 0.9, which is
   Copyright (C) 2004 Chris Cannam, Steve Harris and Sean Bolton

   Modifications made by Lars Luthman in 2006

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
     keys and values to the plugin, that the plugin can use to e.g. load
     sample files, edit programs,  or do anything else that is hard to do
     with control rate input ports or that require some non-RT safe 
     computations
   * A list_used_files() callback that can be used by the host to find 
     out which files it needs to copy into a project directory or session
     archive in order to restore the plugin correctly
     
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
      plugin's behaviour, for example to tell the plugin to load
      sample data from a particular file.  The plugin should act
      immediately on the request.  The call should return NULL on
      success, or an error string that may be shown to the user.  The
      host will free the returned value after use if it is non-NULL.
      
      Calls to configure() are not automated as timed events.
      Instead, a host should remember the last value associated with
      each key passed to configure() during a given session for a
      given plugin instance, and should call configure() with the
      correct value for each key the next time it instantiates the
      "same" plugin instance, for example on reloading a project in
      which the plugin was used before. If multiple keys are used, 
      the order in which their values are set should not matter to the
      plugin.
      
      This function does not need to be realtime safe.
  */
  char* (*configure)(LV2_Handle instance, 
                     const char* key, 
                     const char* value);


  /** This member is a function pointer that the host can use to get
      a list of the data files currently used by the plugin and the
      configuration keys they are associated with. The host may need
      this information if it wants to save the state of the plugin into
      some kind of project or session archive (e.g. if it wants to make
      a project archive that should be transferrable to another computer
      it would have to copy all used data files into that archive).
      
      If the plugin is using any datafiles it should allocate two arrays
      of char* elements and change the pointers *keys and *filepaths to
      point to those arrays. The array pointed to by *filepaths
      should contain the paths to the used data files, and the array
      pointed to by *keys should contain the corresponding keys that the
      host should use with configure() when it restores the current state
      of the plugin. Any previous values for the keys in the key list
      should be forgotten by the host. The elements of the *filepaths array
      may also be NULL, in which case the host should forget about the 
      corresponding configuration key altogether.
      
      The function should return the sizes of the two arrays pointed to by
      the keys and filepaths parameters, which should be equal. If the return
      value is larger than 0, the host is responsible for deallocating the 
      memory used by the two arrays pointed to by keys and filepaths as well 
      as all key strings and all non-NULL filepaths. The plugin must allocate
      the arrays in such a way that it is possible to deallocate them using 
      free() (i.e. C++ plugins should use malloc() instead of the 'new' 
      operator).
      
      A plugin that does not use data files may set this member to
      NULL.

      This function does not need to be realtime safe.
  */
  unsigned int (*list_used_files)(LV2_Handle instance,
                                  char*** keys,
                                  char*** filepaths);  
  
} LV2_InstrumentDescriptor;


#ifdef __cplusplus
}
#endif


#endif
