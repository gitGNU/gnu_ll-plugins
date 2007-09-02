/****************************************************************************
    
    eventqueue.hpp - Simple event queue for Elven
    
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA

****************************************************************************/

#ifndef EVENTQUEUE_HPP
#define EVENTQUEUE_HPP

#include <semaphore.h>

#include "ringbuffer.hpp"


class EventQueue {
public:
  
  enum Type {
    None,
    Control,
    Program,
    ConfigRequest,
    Passthrough,
    Midi
  };
  
  struct ControlEvent {
    unsigned long port;
    float value;
  };
  
  struct ProgramEvent {
    unsigned long program;
  };
  
  struct ConfigRequestEvent {
    EventQueue* sender;
  };
  
  struct PassthroughEvent {
    char msg[5];
    void* ptr;
  };
  
  struct MidiEvent {
    uint32_t port;
    uint32_t size;
    unsigned char data[4];
  };
  
  union Event {
    ControlEvent control;
    ProgramEvent program;
    ConfigRequestEvent config;
    PassthroughEvent passthrough;
    MidiEvent midi;
  };
  
  EventQueue() : m_read_event_type(false) { 
    sem_init(&m_sem, 0, 0);
  }
  
  ~EventQueue() {
    sem_destroy(&m_sem);
  }
  
  Type read_event();
  const Event& get_event() const;
  
  bool write_control(unsigned long port, float value);
  bool write_program(unsigned long program);
  bool write_config_request(EventQueue* sender);
  bool write_passthrough(const char* msg, void* ptr = 0);
  bool write_midi(uint32_t port, uint32_t size, const unsigned char* data);
  
  bool wait();
  
protected:
  
  Ringbuffer<unsigned char, 102400> m_queue;
  bool m_read_event_type;
  Type m_next_event_type;
  Event m_event;
  
  sem_t m_sem;
};


#endif
