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

#ifndef LV2_PROGRAM_GTK2GUI_H
#define LV2_PROGRAM_GTK2GUI_H

#include <lv2-gtk2gui.h>

#ifdef __cplusplus
extern "C" {
#endif


/** A pointer to an object of this type should be returned by the 
    extension_data() function in the LV2UI_ControllerDescriptor when it is 
    called with the URI <http://ll-plugins.nongnu.org/lv2/namespace#program>. */
typedef struct {
  
  /** Tell the host to start using the plugin program with the given number. */
  void (*set_program)(LV2UI_Controller, unsigned char number);

} LV2_ProgramControllerDescriptor;


/** A pointer to an object of this type should be returned by the 
    extension_data() function in the LV2UI_UIDescriptor when it is called 
    with the URI <http://ll-plugins.nongnu.org/lv2/namespace#program>. */
typedef struct {
  
  /** Tell the GUI that a program has been added for the plugin instance. */
  void (*add_program)(LV2UI_Handle handle, 
                      unsigned char number,
                      const char* name);
  
  /** Tell the GUI that a program has been removed for the plugin instance. */
  void (*remove_program)(LV2UI_Handle handle,
                         unsigned char number);
  
  /** Tell the GUI that all programs have been removed for the plugin 
      instance. */
  void (*clear_programs)(LV2UI_Handle handle);
  
  /**Tell the GUI that the given program is now being used for the plugin. */
  void (*set_program)(LV2UI_Handle handle, unsigned char number);

} LV2_ProgramUIDescriptor;


#ifdef __cplusplus
}
#endif


#endif
