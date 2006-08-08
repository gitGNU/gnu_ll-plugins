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
    Passthrough
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
  
  union Event {
    ControlEvent control;
    ProgramEvent program;
    ConfigRequestEvent config;
    PassthroughEvent passthrough;
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
  
  bool wait();
  
protected:
  
  Ringbuffer<unsigned char, 102400> m_queue;
  bool m_read_event_type;
  Type m_next_event_type;
  Event m_event;
  
  sem_t m_sem;
};


#endif
