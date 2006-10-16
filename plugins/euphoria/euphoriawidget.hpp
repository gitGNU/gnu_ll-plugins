#ifndef EUPHORIAWIDGET_HPP
#define EUPHORIAWIDGET_HPP

#include <string>

#include <gtkmm.h>


class EuphoriaWidget : public Gtk::VBox {
public:
  
  EuphoriaWidget();
  
protected:
  
  Gtk::VBox* create_knob(const std::string& label, 
                         float red = 0, float green = 0.3, float blue = 1);
  
  Gtk::TreeView m_program_view;
};


#endif
