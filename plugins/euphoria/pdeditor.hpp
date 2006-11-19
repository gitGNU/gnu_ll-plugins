#ifndef PDEDITOR_HPP
#define PDEDITOR_HPP

#include <string>
#include <vector>

#include <gtkmm.h>


class PDEditor : public Gtk::DrawingArea {
public:
  
  PDEditor();
  
  bool set_string(const std::string& str);
  std::string get_string() const;
  
  sigc::signal<void> signal_apply;

protected:
  
  struct Point {
    
    Point(double _ox = 0, double _oy = 1)
      : ox(_ox),
        oy(_oy),
        x(_ox),
        y(_oy) {

    }
    
    double ox;
    double oy;
    double x;
    double y;
  };
  
  
  bool on_expose_event(GdkEventExpose* event);
  bool on_motion_notify_event(GdkEventMotion* event);
  bool on_button_release_event(GdkEventButton* event);
  bool on_button_press_event(GdkEventButton* event);
  bool on_scroll_event(GdkEventScroll* event);
  
  void new_point();
  void delete_point();
  void set_dirty();
  void apply();
  
  double x2p(double x);
  double y2p(double y);
  double p2x(int p);
  double p2y(int p);
  
  std::vector<Point> m_points;
  
  double m_margin;
  double m_click_x, m_click_y;
  int m_active_point;
  bool m_dragging;
  bool m_odragging;
  int m_pix_drag_x, m_pix_drag_y;
  double m_drag_x, m_drag_y;
  
  bool m_wave;
  
  bool m_dirty;
  
  Gtk::Menu m_menu;
  
};


#endif
