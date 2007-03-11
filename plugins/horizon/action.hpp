#ifndef ACTION_HPP
#define ACTION_HPP


class Chunk;


class Action {
public:
  
  Action(const Chunk& chunk);
  
  const Chunk& get_chunk() const;
  
protected:
  
  const Chunk& m_chunk;
  
};


#endif
