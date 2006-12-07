/****************************************************************************
    
    lv2-midifunctions.h - support file for using MIDI in LV2 plugins
    
    Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    
    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef LV2_MIDIFUNCTIONS
#define LV2_MIDIFUNCTIONS

#include <string.h>

#include "lv2-miditype.h"


/** A sort of "iterator" for LV2 MIDI ports. For normal use you will probably 
    want to initialise it like this:
    @code
    LV2_MIDIState state = { (LV2_MIDI*)port_buffer, nframes, 0 };
    @endcode
    where @c port_buffer is the buffer for your MIDI port and @c nframes is 
    the number of audio frames that you are computing in the current run() 
    cycle. */
typedef struct {
  
  /** The MIDI port buffer. */
  LV2_MIDI* midi;
  
  /** The number of frames we are computing in the current run() cycle, i.e.
      the upper limit for the event timestamps. */
  const uint32_t frame_count;
  
  /** The current position in the data buffer. */
  uint32_t position;
  
} LV2_MIDIState;


/** Read a MIDI event from the port buffer. If there buffer contains more 
    events the timestamp (offset from the cycle start in frames) for next 
    event will be returned, and the parameters @c timestamp, @c size, and
    @c data will be set to point to the timestamp, data size, and raw data
    for the MIDI event. If there are no more events in the buffer, the 
    @c frame_count member of @c state will be returned.
    
    This function does not increase the read/write position, for that you will
    need lv2midi_step(). This code will iterate over all events in a buffer
    and do something with them:
    @code
    LV2_MIDIState reader = { (LV2_MIDI*)port_buffer, nframes, 0 };
    double timestamp;
    uint32_t size;
    unsigned char* data;
    while (lv2midi_get_event(&reader, &timestamp, &size, &data) < nframes) {
      do_something_with_midi(timestamp, size, data);
      lv2midi_step(reader);
    }
    @endcode
*/
static double lv2midi_get_event(LV2_MIDIState* state,
                                double* timestamp, 
                                uint32_t* size, 
                                unsigned char** data) {
  
  if (state->position >= state->midi->size) {
    state->position = state->midi->size;
    *timestamp = state->frame_count;
    *size = 0;
    *data = NULL;
    return *timestamp;
  }
  
  *timestamp = *(double*)(state->midi->data + state->position);
  *size = *(size_t*)(state->midi->data + state->position + sizeof(double));
  *data = state->midi->data + state->position + 
    sizeof(double) + sizeof(size_t);
  return *timestamp;
}


/** Move the read/write position to next event in the port buffer and
    return the timestamp for that event, or the @c frame_count member
    of @c state if there are no more events. */
static double lv2midi_step(LV2_MIDIState* state) {

  if (state->position + sizeof(double) + sizeof(size_t) >= state->midi->size) {
    state->position = state->midi->size;
    return state->frame_count;
  }
  
  state->position += sizeof(double);
  size_t size = *(size_t*)(state->midi->data + state->position);
  state->position += sizeof(size_t);
  state->position += size;
  
  if (state->position >= state->midi->size)
    return state->frame_count;
  
  return *(double*)(state->midi->data + state->position);
}


/** Write one event to the port buffer and increase the read/write position.
    This example writes one note on at frame offset 0 followed by a note off
    at frame offset 100:
    @code
    LV2_MIDIState writer = { (LV2_MIDI*)port_buffer, nframes, 0 };
    unsigned char note_on[] = { 0x90, 0x60, 0x70 };
    unsigned char note_off[] = { 0x80, 0x60, 0x64 };
    lv2midi_put_event(&writer, 0, 3, note_on);
    lv2midi_put_event(&writer, 0, 100, note_off);
    @endcode
*/
static int lv2midi_put_event(LV2_MIDIState* state, 
                             double timestamp,
                             uint32_t size,
                             const unsigned char* data) {

  if (state->midi->capacity - state->midi->size < 
      sizeof(double) + sizeof(size_t) + size)
    return -1;
  
  *(double*)(state->midi->data + state->midi->size) = timestamp;
  state->midi->size += sizeof(double);
  *(size_t*)(state->midi->data + state->midi->size) = size;
  state->midi->size += sizeof(size_t);
  memcpy(state->midi->data + state->midi->size, data, size);
  state->midi->size += size;
  
  ++state->midi->event_count;
  
  return 0;
}

#endif

