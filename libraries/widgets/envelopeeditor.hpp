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
  
  Gtk::Adjustment& get_adjustment();
  
  sigc::signal<void> signal_apply;
  
protected:

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
  
  
  void on_size_allocate(Gtk::Allocation& a);
  bool on_expose_event(GdkEventExpose* event);
  bool on_motion_notify_event(GdkEventMotion* event);
  bool on_button_release_event(GdkEventButton* event);
  bool on_button_press_event(GdkEventButton* event);
  bool on_scroll_event(GdkEventScroll* event);
  
  void update_adjustment();
  
  void new_segment();
  void delete_segment();
  void set_loop_start();
  void set_loop_end();
  void clear_loop();
  void set_length_modulator(SegmentType type);
  void set_curve_type(CurveType curve);
  void set_dirty();
  void apply();
  
  int x2p(double x);
  int y2p(double y);
  double p2x(int p);
  double p2y(int p);
  
  
  int m_loop_start;
  int m_loop_end;
  
  std::vector<Segment> m_segments;
  
  double m_margin;
  double m_ruler_height;
  double m_xscale;
  double m_click_x, m_click_y;
  int m_active_segment;
  bool m_dragging;
  bool m_move;
  int m_pix_drag_x, m_pix_drag_y;
  double m_drag_length, m_drag_y;
  
  bool m_dirty;
  
  Gtk::Menu m_menu;
  
  Gtk::Adjustment m_adj;
};


#endif
