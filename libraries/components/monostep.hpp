#ifndef MONOSTEP_HPP
#define MONOSTEP_HPP


class MonoStep {
public:
  MonoStep() : note(64), velocity(5), on(false), slide(false) { }
  unsigned char note;
  unsigned char velocity;
  bool on;
  bool slide;
};


#endif
