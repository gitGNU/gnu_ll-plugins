#include <iostream>

#include "eventqueue.hpp"


EventQueue::Type EventQueue::read_event() {
  
  if (!m_queue.available())
    return None;
  
  if (!m_read_event_type) {
    if (m_queue.available() < sizeof(Type))
      return None;
    m_queue.read(reinterpret_cast<unsigned char*>(&m_next_event_type),
                 sizeof(Type));
  }
  
  switch (m_next_event_type) {
    
  case Program:
    if (m_queue.available() < sizeof(ProgramEvent))
      return None;
    m_queue.read(reinterpret_cast<unsigned char*>(&m_event.program),
                 sizeof(ProgramEvent));
    return Program;
    
  case Control:
    if (m_queue.available() < sizeof(ControlEvent))
      return None;
    m_queue.read(reinterpret_cast<unsigned char*>(&m_event.control),
                 sizeof(ControlEvent));
    return Control;
    
  case ConfigRequest:
    return ConfigRequest;
    
  case Passthrough:
    if (m_queue.available() < sizeof(PassthroughEvent))
      return None;
    m_queue.read(reinterpret_cast<unsigned char*>(&m_event.passthrough),
                 sizeof(PassthroughEvent));
    return Passthrough;
  }
  
  return None;
}


const EventQueue::Event& EventQueue::get_event() const {
  return m_event;
}


bool EventQueue::write_control(unsigned long port, float value) {
  Type t = Control;
  m_queue.write(reinterpret_cast<unsigned char*>(&t), sizeof(Type));
  ControlEvent ce;
  ce.port = port;
  ce.value = value;
  m_queue.write(reinterpret_cast<unsigned char*>(&ce), sizeof(ControlEvent));
  sem_post(&m_sem);
  return true;
}


bool EventQueue::write_program(unsigned long program) {
  Type t = Program;
  m_queue.write(reinterpret_cast<unsigned char*>(&t), sizeof(Type));
  ProgramEvent pe;
  pe.program = program;
  m_queue.write(reinterpret_cast<unsigned char*>(&pe), sizeof(ProgramEvent));
  sem_post(&m_sem);
  return true;
}


bool EventQueue::write_config_request() {
  Type t = ConfigRequest;
  m_queue.write(reinterpret_cast<unsigned char*>(&t), sizeof(Type));
  sem_post(&m_sem);
  return true;
}


bool EventQueue::write_passthrough(const char* msg, void* ptr) {
  Type t = Passthrough;
  m_queue.write(reinterpret_cast<unsigned char*>(&t), sizeof(Type));
  PassthroughEvent pe;
  std::memset(pe.msg, 0, 5);
  std::strncpy(pe.msg, msg, 4);
  pe.ptr = ptr;
  m_queue.write(reinterpret_cast<unsigned char*>(&pe), 
                sizeof(PassthroughEvent));
  sem_post(&m_sem);
  return true;
}


bool EventQueue::wait() {
  return sem_wait(&m_sem) == 0;
}