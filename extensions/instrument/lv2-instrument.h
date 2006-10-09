/************************************************************************

   This file is based on dssi.h from DSSI 0.9, which is
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


/* This header defines data structures and preprocessor macros used by
   LV2 hosts and plugins that support the LV2 Instrument extension with
   URI <http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext>.
   
   This extension adds two things to LV2 plugins:
   
   * "Programs", named presets that can be listed and selected by the host
   
   * A configure() callback that can be used by the host to pass arbitrary
     keys and values to the plugin, that the plugin can use to e.g. load
     sample files, edit programs,  or do anything else that is hard to do
     with control rate input ports or that require some non-RT safe 
     computations, and a list_used_files() callback that can be used by the
     host to find out which files it needs to copy into a project directory 
     or session archive in order to restore the plugin correctly
     
   Some terminology: a 'MIDI port' is an LV2 input port with the datatype 
   <http://ll-plugins.nongnu.org/lv2/namespace#miditype>. An LV2 plugin may
   also have output ports with this type but they do not have any special
   meaning for this extension.
   
   A plugin that supports this extension may have one 'default MIDI port',
   specified by the RDF triple
   
   PLUGIN <http://ll-plugins.nongnu.org/lv2/namespace#defaultMidiPort> INDEX
   
   where PLUGIN is the URI for the plugin and INDEX is an integer literal 
   specifying the index for a MIDI port. If the plugin has no MIDI ports
   it will not have a default MIDI port either, and if it has MIDI ports
   but no specified default MIDI port the MIDI port with the lowest index
   will be assumed to be the default one. MIDI events sent to the default
   MIDI port may be subject to filtering by the host as specified in the 
   comments for select_program() below.
   
   Plugins that support this extension do not _have_ to have MIDI ports - 
   program selection may be useful even if the plugin itself doesn't do 
   anything with MIDI events.
   
   The data member in the LV2_Host_Feature struct for this extension, 
   passed to the plugin's instantiate() function, should be a function pointer
   of the type LV2_InstrumentFunctionCallback, specified below. It should
   point to a function in the host that takes a LV2_InstrumentFunction (also
   specified below) as argument. LV2_InstrumentFunction is a pointer to a 
   function that takes no parameters and returns a pointer to a
   LV2_InstrumentDescriptor. So the way the host accesses the plugin's 
   LV2_InstrumentDescriptor is something like this:
   
   1. Host loads plugin and retrieves a pointer to its LV2_Descriptor.
   2. Host calls instantiate() in the plugin, passing the LV2_Host_Feature
      {"<http://ll-plugins.nongnu.org/lv2/namespace#instrument-ext>",
       &hostInstrumentFunctionCallback }.
   3. In its instantiate() function, the plugin calls 
      hostInstrumentFunctionCallback(&pluginInstrumentFunction);
   4. In its hostInstrumentFunctionCallback() function, the host calls
      pluginInstrumentFunction() and stores the return value as a pointer
      to the LV2_InstrumentDescriptor for this plugin.
   
*/


/** A struct that contains information about a single program. Used in the
    return value of the get_program() callback. 
*/
typedef struct {

  /** Number of this program. Should not exceed 2097151, since that is the
      largest number that can be obtained using MIDI Bank Select MSB/LSB and
      Program Change messages, and many hosts will probably want to map MIDI
      program changes to LV2 program changes. Hosts that use MIDI to select
      programs should use the macros at the end of this header file to
      transform MIDI bank and program numbers to plain program numbers.
  */
  uint32_t number;
  
  /** Name of the program. 
   */
  const char* name;
  
} LV2_ProgramDescriptor;


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
      which the plugin was used before.
      
      Calling configure() completely invalidates all program information 
      obtained from the plugin. If the host wants to keep the list of 
      available programs for the plugin up to date, it needs to retrieve
      it again after each configure() call, and if it wants a particular
      program to be selected it needs to call select_program() again after
      each configure() call.
      
      This function does not need to be realtime safe.
  */
  char* (*configure)(LV2_Handle instance, 
                     const char* key, 
                     const char* value);


  /** This member is a function pointer that the host can use to get
      a list of the data files currently used by the plugins and the
      configuration keys they are associated with. The host may need
      this information if it wants to save the state of the plugin into
      some kind of project or session archive (e.g. if it wants to make
      a project archive that should be transferrable to another computer
      it would have to copy the used data files into that archive).
      
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
  
  
  /** This member is a function pointer that provides a description
      of a program (named preset) available on this synth.  A
      plugin that does not support programs at all should set this
      member to NULL.
      
      The index argument is an index into the plugin's list of
      programs, not a program number as represented by the number
      field of the LV2_ProgramDescriptor (this distinction is
      needed to support synths that use non-contiguous program
      numbers). The host should attach no meaning at all to the index.
      
      This function returns a LV2_ProgramDescriptor pointer that is
      guaranteed to be valid only until the next call to get_program(),
      deactivate(), or configure(), on the same plugin instance. This
      function must return NULL if passed an index argument out of
      range, so that the host can use it to query the number of
      programs as well as their properties.
      
      This function does not need to be realtime safe.
  */
  const LV2_ProgramDescriptor *(*get_program)(LV2_Handle instance, 
                                              uint32_t index);
  
  
  /** This member is a function pointer that selects a new program
      for this synth. The program change should take effect
      immediately at the start of the next run() call. (This means 
      that a host providing the capability of changing programs
      between any two notes on a track must vary the block size so as
      to place the program change at the right place. A host that
      wanted to avoid this would probably just instantiate a plugin
      for each program.)
      
      A plugin that does not support programs at all should set this
      member to NULL. Plugins should ignore a select_program() call
      with an invalid program.
      
      A plugin is not required to select any particular default
      program on activate(), it's the host's duty to set a program
      explicitly. The current program is invalidated by any call to
      configure() or deactivate().
      
      A plugin is permitted to re-write the values of its input
      control ports when select_program() is called. The host should
      re-read the input control port values and update its own
      records appropriately (this is the only circumstance in which 
      a LV2 plugin with this extension is allowed to modify its own 
      input ports).
      
      Regardless of whether this callback is NULL or not, a host
      should never deliver MIDI Program Change or Bank Select (MSB or LSB)
      messages to the default MIDI port.
      
      If the plugin supports the hardRtCapable property, this function
      needs to be realtime safe.
  */
  void (*select_program)(LV2_Handle instance, uint32_t program);
  
  
} LV2_InstrumentDescriptor;


/** This is the type of the plugin's callback that returns a pointer to
    its LV2_InstrumentDescriptor.
*/
typedef const LV2_InstrumentDescriptor* (*LV2_InstrumentFunction)();


/** This is the type of the host's callback that is passed as the data in
    the host feature struct. 
*/
typedef void (*LV2_InstrumentFunctionCallback)(LV2_InstrumentFunction);


/*
  Macros to create LV2_Instrument program numbers from MIDI bank and 
  program numbers, and the other way around. 
*/

#define LV2INST_PROGRAM(B, P)          (((B) << 7) + (P))
#define LV2INST_MIDIBANK(L)            ((L) >> 7)
#define LV2INST_MIDIPROGRAM(L)         ((L) & 0x7F)


#endif
