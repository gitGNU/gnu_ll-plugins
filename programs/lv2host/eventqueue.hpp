#ifndef EVENTQUEUE_HPP
#define EVENTQUEUE_HPP

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
  
  EventQueue() : m_read_event_type(false) { }
  
  Type read_event();
  const Event& get_event() const;
  
  bool write_control(unsigned long port, float value);
  bool write_program(unsigned long program);
  bool write_config_request();
  bool write_passthrough(const char* msg, void* ptr);
  
protected:
  
  Ringbuffer<unsigned char, 1024> m_queue;
  bool m_read_event_type;
  Type m_next_event_type;
  Event m_event;
};


#endif
