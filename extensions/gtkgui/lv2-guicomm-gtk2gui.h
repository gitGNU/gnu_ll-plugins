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

#ifndef LV2_GUICOMM_GTK2GUI_H
#define LV2_GUICOMM_GTK2GUI_H

#include <lv2-gtk2gui.h>

#ifdef __cplusplus
extern "C" {
#endif


/** A pointer to an object of this type should be returned by the 
    extension_data() function in the LV2UI_ControllerDescriptor when it is 
    called with the URI <http://ll-plugins.nongnu.org/lv2/ext/gtk2gui/comm>. */
typedef struct {
  
  /** Send a command to the plugin. */
  void (*tell_plugin)(LV2UI_Controller     controller, 
		      uint32_t             argc,
		      const char* const*   argv);

} LV2_GUICommControllerDescriptor;


/** A pointer to an object of this type should be returned by the 
    extension_data() function in the LV2UI_UIDescriptor when it is called 
    with the URI <http://ll-plugins.nongnu.org/lv2/ext/gtk2gui/comm>. */
typedef struct {
  
  /** Send feedback to the GUI. */
  void (*tell_gui)(LV2UI_Handle         handle, 
		   uint32_t             argc,
		   const char* const*   argv);
  
} LV2_GUICommUIDescriptor;


#ifdef __cplusplus
}
#endif


#endif
