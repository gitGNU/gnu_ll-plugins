/************************************************************************
 *
 * GTK2 in-process UI extension for LV2
 *
 * Copyright (C) 2006 Lars Luthman <lars.luthman@gmail.com>
 * 
 * Based on lv2.h, which is 
 *
 * Copyright (C) 2000-2002 Richard W.E. Furse, Paul Barton-Davis, Stefan
 * Westerfeld
 * Copyright (C) 2006 Steve Harris, Dave Robillard.
 *
 * This header is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This header is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
 * USA.
 *
 ***********************************************************************/

/** This extension defines an interface that can be used in LV2 plugins and
    hosts to create GTK2 GUIs for plugins. The GUIs will be plugins that
    will reside in shared object files in the plugin bundle directory and
    be referenced in the RDF file using the triple
    
    <http://my.plugin> <http://ll-plugins.nongnu.org/lv2/namespace#gtk2Gui> <mygui.so>
    
    where <http://my.plugin> is the URI of the plugin and <mygui.so> is the 
    relative URI to the shared object file. While it is possible to have the
    plugin GUI and the plugin in the same shared object file it is probably
    a good idea to keep them separate so that hosts that don't want GUIs
    don't have to load the GUI code.
    
    Note that the process that loads the shared object file containing the GUI
    code and the process that loads the shared object file containing the 
    actual plugin implementation does not have to be the same. There are many
    valid reasons for having the plugin and the GUI in different processes, or
    even on different machines. This means that you can _not_ use singletons
    and global variables and expect them to refer to the same objects in the
    GUI and the actual plugin. The function callback interface defined in this
    header is all you can expect to work.
    
    Since the LV2 specification itself allows for extensions that may add 
    new types of data and configuration parameters that plugin authors may 
    want to control with a GUI, this extension allows for meta-extensions that
    can extend the interface between the GUI and the host. See the
    extension_data() callback pointers for more details.
    
    Note that this extension is NOT a Host Feature. There is no way for a plugin
    to know whether the host that loads it supports GUIs or not, and the plugin
    must ALWAYS work without the GUI (although it may be rather useless unless
    it has been configured using the GUI in a previous session).
*/

#ifndef LV2_GTK2GUI_H
#define LV2_GTK2GUI_H

#include <lv2.h>
#include <gtk/gtkwidget.h>


#ifdef __cplusplus
extern "C" {
#endif



typedef void* LV2UI_Controller;


/** This struct contains pointers to functions provided by the program which is
    loading the GUI plugin. None of the members are allowed to be NULL. 
    An object of this type will be created by the host and a pointer to it
    will be passed to the instantiate() function in the LV2UI_UIDescriptor
    struct. The GUI plugin can then use the function pointers in that object
    to control the LV2 plugin instance through the host. For example, a GUI
    that provides a single slider for a "Gain" parameter with port number
    1 will probably want to call 
    <code>set_control(host_controller, 1, slider_value)</code> when the user
    moves the slider, where @c host_controller is the @c LV2UI_Controller given
    by the host and @c slider_value is the new value of the slider. 
*/
typedef struct {

  /** Change the value of one of the plugin's control rate float ports. If
      @c port is not an index for a valid control rate float input port nothing
      should happen. 
  */
  void (*set_control)(LV2UI_Controller   controller,
                      uint32_t           port, 
                      float              value);
  
  /** Similar to the extension_data() callback in the @c LV2_Descriptor 
      struct, this returns a data structure associated with an extension URI,
      for example a struct containing additional function pointers. Avoid 
      returning function pointers directly since standard C++ has no valid 
      way of casting a void* to a function pointer. 
  */
  void* (*extension_data)(LV2UI_Controller  controller,
                          const char*       URI);

} LV2UI_ControllerDescriptor;



typedef void* LV2UI_Handle;


typedef struct {
  
  /** Create a new GUI object and return a handle to it. A handle to the
      controller that this GUI is going to be used with is passed as a 
      parameter, as well as a pointer to a GtkWidget pointer. This should
      be set to point to the widget that will act as the GTK+ GUI for the 
      plugin instance. The GUI is not allowed to set this pointer to NULL 
      unless it also returns NULL from this function. 
  */
  LV2UI_Handle (*instantiate)(LV2UI_ControllerDescriptor*   descriptor,
                              LV2UI_Controller              controller,
                              const char*                   URI,
                              const char*                   bundle_path,
                              GtkWidget**                   widget);
  
  /** Destroy the GUI object and the associated widget. 
   */
  void (*cleanup)(LV2UI_Handle  gui);
  
  /** Tell the GUI that a control port value has changed. This member may be
      set to NULL if the GUI is not interested in control port changes. 
  */
  void (*set_control)(LV2UI_Handle   gui,
                      uint32_t       port,
                      float          value);
  
  /** Returns a data structure associated with an extension URI, for example
      a struct containing additional function pointers. Avoid returning
      function pointers directly since standard C++ has no valid way of
      casting a void* to a function pointer. This member may be set to NULL
      if the GUI is not interested in supporting any extensions. 
  */
  void* (*extension_data)(LV2UI_Handle    gui,
                          const char*     URI);

} LV2UI_UIDescriptor;



/** Accessing a plugin GUI:
    
    A plugin programmer must include a function called "lv2ui_descriptor"
    with the following function prototype within the shared object
    file. This function will have C-style linkage (if you are using
    C++ this is taken care of by the 'extern "C"' clause at the top of
    the file). This function will be accessed by the GUI host using the 
    @c dlopen() function and called to get a LV2UI_UIDescriptor for the
    wanted plugin.
    
    This function differs from the lv2_descriptor() function in lv2.h in the
    parameter it takes - lv2_descriptor() takes an index and the host calls
    the function with indices from 0 and upwards until it finds the plugin
    descriptor it is looking for. lv2ui_descriptor() takes the plugin URI
    as parameter and returns the LV2UI_UIDescriptor for that plugin, or 0
    if it does not know anything about that plugin. */
const LV2UI_UIDescriptor* lv2ui_descriptor(const char* URI);


typedef const LV2UI_UIDescriptor* 
(*LV2UI_UIDescriptorFunction)(const char* URI);



#ifdef __cplusplus
}
#endif


#endif
