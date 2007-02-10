#ifndef DELAYLINE_HPP
#define DELAYLINE_HPP

#include <cstring>


namespace {


  class DelayLine {
  public:
    
    DelayLine(unsigned int delay);
    ~DelayLine();
    
    void write(float input);
    float read();

  protected:
    
    unsigned int m_size;
    float* m_line;
    unsigned int m_pos;

  };
  
  
  DelayLine::DelayLine(unsigned int delay) 
    : m_size(delay),
      m_line(new float[m_size]),
      m_pos(0) {
    std::memset(m_line, 0, sizeof(float) * m_size);
  }
  
  
  DelayLine::~DelayLine() {
    delete [] m_line;
  }
  
  
  void DelayLine::write(float input) {
    m_line[m_pos] = input;
    ++m_pos;
    m_pos %= m_size;
  }
  
  
  float DelayLine::read() {
    return m_line[m_pos];
  }
  
}


#endif
