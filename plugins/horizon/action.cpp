#include "action.hpp"


Action::Action(const Chunk& chunk) 
  : m_chunk(chunk) {

}

  
const Chunk& Action::get_chunk() const {
  return m_chunk;
}
