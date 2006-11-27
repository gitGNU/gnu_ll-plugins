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

#ifndef LV2_INSTRUMENT_GTK2GUI_H
#define LV2_INSTRUMENT_GTK2GUI_H

#include <lv2-gtk2gui.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {

  void (*configure)(LV2UI_Controller controller, 
                    const char* key, 
                    const char* value);

  void (*set_file)(LV2UI_Controller controller,
                   const char* key,
                   const char* filename);

} LV2_InstrumentControllerDescriptor;


typedef struct {

  void (*configure)(LV2UI_Handle handle, 
                    const char* key, 
                    const char* value);

  void (*set_file)(LV2UI_Handle handle,
                   const char* key,
                   const char* filename);

} LV2_InstrumentUIDescriptor;


#ifdef __cplusplus
}
#endif


#endif
