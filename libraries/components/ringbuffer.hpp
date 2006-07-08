/****************************************************************************
    
    ringbuffer.hpp - a ringbuffer designed to be used in shared memory
    
    Copyright (C) 2005  Lars Luthman <larsl@users.sourceforge.net>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP


template <class T, size_t S> class Ringbuffer {
public:

  Ringbuffer();

  int read(T* dest, size_t size);
  int write(T* src, size_t size);
  int write_zeros(size_t size);
  int available() const;
  int available_contiguous() const;
  int get_read_pos() const;
  int get_write_pos() const;
  const T* get_read_ptr() const;
  
private:
  
  volatile int m_read_pos;
  volatile int m_write_pos;
  
  char volatile m_data[S * sizeof(T)];

};


template <class T, size_t S> 
Ringbuffer<T, S>::Ringbuffer()
  : m_read_pos(0),
    m_write_pos(0) {
  
}


template <class T, size_t S> 
int Ringbuffer<T, S>::read(T* dest, size_t size) {
  
  size_t n = 0;
  T* data = (T*)m_data;
  
  if (size == 0)
    return 0;
  
  if (m_read_pos > m_write_pos) {
    n = S - m_read_pos;
    n = (n > size ? size : n);
    if (dest)
      memcpy(dest, data + m_read_pos, n * sizeof(T));
    m_read_pos = (m_read_pos + n) % S;
  }
  
  if (m_read_pos < m_write_pos && n < size) {
    size_t m = m_write_pos - m_read_pos;
    m = (m > size - n ? size - n : m);
    if (dest)
      memcpy(dest + n, data + m_read_pos, m * sizeof(T));
    m_read_pos = (m_read_pos + m) % S;
    n += m;
  }
  
  /*  fprintf(stderr, "RB %d atoms read from the ringbuffer\n"
      "\tread_pos = %d write_pos = %d max_pos = %d\n",
      n, rb->read_pos, rb->write_pos, rb->max_pos); */

  /*if (n < size)
    fprintf(stderr, "RB: read collision %d\n", col++);*/

  return n;

}


template <class T, size_t S>
int Ringbuffer<T, S>::write(T* src, size_t size) {
  size_t n = 0;
  T* data = (T*)(m_data);

  if (size == 0)
    return 0;

  if (m_write_pos >= m_read_pos) {
    n = S - m_write_pos;
    if (m_read_pos == 0)
      --n;
    n = (n > size ? size : n);
    memcpy(data + m_write_pos, src, n * sizeof(T));
    m_write_pos = (m_write_pos + n) % S;
  }
  
  if (m_write_pos + 1 < m_read_pos && n < size) {
    size_t m = m_read_pos - m_write_pos - 1;
    m = (m > size - n ? size - n : m);
    memcpy(data + m_write_pos, src + n, m * sizeof(T));
    m_write_pos = (m_write_pos + m) % S;
    n += m;
  }

  /*fprintf(stderr, "RB %d atoms written to the ringbuffer\n"
    "\tread_pos = %d write_pos = %d max_pos = %d\n",
    n, rb->read_pos, rb->write_pos, rb->max_pos);*/
  
  /* if (n < size)
     fprintf(stderr, "RB: write collision %d\n", col++); */
  
  return n;

}


template <class T, size_t S>
int Ringbuffer<T, S>::write_zeros(size_t size) {
  size_t n = 0;
  T* data = (T*)(m_data);

  if (size == 0)
    return 0;

  if (m_write_pos >= m_read_pos) {
    n = S - m_write_pos;
    if (m_read_pos == 0)
      --n;
    n = (n > size ? size : n);
    memset(data + m_write_pos, 0, n * sizeof(T));
    m_write_pos = (m_write_pos + n) % S;
  }
  
  if (m_write_pos + 1 < m_read_pos && n < size) {
    size_t m = m_read_pos - m_write_pos - 1;
    m = (m > size - n ? size - n : m);
    memset(data + m_write_pos, 0, m * sizeof(T));
    m_write_pos = (m_write_pos + m) % S;
    n += m;
  }

  /*fprintf(stderr, "RB %d atoms written to the ringbuffer\n"
    "\tread_pos = %d write_pos = %d max_pos = %d\n",
    n, rb->read_pos, rb->write_pos, rb->max_pos);*/
  
  /* if (n < size)
     fprintf(stderr, "RB: write collision %d\n", col++); */
  
  return n;

}


template <class T, size_t S>
int Ringbuffer<T, S>::available() const {
  if (m_read_pos <= m_write_pos)
    return m_write_pos - m_read_pos;
  else
    return S - m_read_pos + m_write_pos;
}


template <class T, size_t S>
int Ringbuffer<T, S>::available_contiguous() const {
  if (m_read_pos <= m_write_pos)
    return m_write_pos - m_read_pos;
  else
    return S - m_read_pos;
}


template <class T, size_t S>
int Ringbuffer<T, S>::get_read_pos() const {
  return m_read_pos;
}
  

template <class T, size_t S>
int Ringbuffer<T, S>::get_write_pos() const {
  return m_write_pos;
}


template <class T, size_t S>
const T* Ringbuffer<T, S>::get_read_ptr() const {
  T* data = (T*)m_data;
  return data + m_read_pos;
}


#endif


