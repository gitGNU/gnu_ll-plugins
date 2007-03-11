#ifndef MIXER_HPP
#define MIXER_HPP

#include "sample.hpp"
#include "voice.hpp"


/** The Mixer class handles all mixing of voices and writing of audio
    to the output buffers. */
class Mixer {
public:
  
  /** Create a new Mixer object. */
  Mixer();
  
  /** Set the buffers to write to. */
  void set_buffers(float* left, float* right);
  
  /** Write @c nframes of audio to the output buffers (do not call this
      without first calling set_buffers() with pointers to buffers
      that are large enough to hold @c nframes frames of audio). */
  void run(uint32_t nframes);
  
  void play_chunk(const Chunk* chunk);
  void stop();

protected:
  
  Sample m_sample;
  float* m_left;
  float* m_right;
  float m_gain;
  
  Voice m_voice;
};


#endif
