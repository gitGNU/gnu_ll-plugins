#ifndef ENVELOPEEDITOR_HPP
#define ENVELOPEEDITOR_HPP

#include <string>
#include <vector>

#include <gtkmm.h>


class EnvelopeEditor : public Gtk::DrawingArea {
public:
  
  EnvelopeEditor();
  
  bool set_string(const std::string& str);
  std::string get_string() const;
  
protected:

  bool on_expose_event(GdkEventExpose* event);
  bool on_motion_notify_event(GdkEventMotion* event);
  bool on_button_press_event(GdkEventButton* event);
  bool on_scroll_event(GdkEventScroll* event);
  
  enum SegmentType {
    Constant,
    Attack,
    Decay,
    Release,
    NumTypes
  };
  
  enum CurveType {
    Linear,
    Exponential,
    Exponential2,
    NumCurves
  };
  
  struct Segment {
    
    Segment(double _start = 0, double _length = 1, double _sustain_sens = 0,
            SegmentType _type = Constant, CurveType _curve = Linear)
      : start(_start),
        length(_length),
        sustain_sens(_sustain_sens),
        type(_type),
        curve(_curve) {

    }
    
    double start;
    double length;
    double sustain_sens;
    SegmentType type;
    CurveType curve;
  };
  
  int m_loop_start;
  int m_loop_end;
  
  std::vector<Segment> m_segments;
  
  double m_ruler_height;
  double m_xscale;
};


#endif
