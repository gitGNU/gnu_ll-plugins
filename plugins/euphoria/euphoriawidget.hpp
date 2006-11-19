#ifndef EUPHORIAWIDGET_HPP
#define EUPHORIAWIDGET_HPP

#include <string>

#include <gtkmm.h>

#include "lv2uiclient.hpp"
#include "shapereditor.hpp"
#include "pdeditor.hpp"
#include "envelopeeditor.hpp"


class EuphoriaWidget : public Gtk::VBox {
public:
  
  EuphoriaWidget(LV2UIClient& lv2);
  
  void add_program(int number, const std::string& name);
  void remove_program(int number);
  void clear_programs();
  
  void configure(const std::string& key, const std::string& value);
  
  sigc::signal<void, int> signal_program_selected;
  sigc::signal<void, std::string> signal_shape_changed;
  sigc::signal<void, std::string> signal_shape_envelope_changed;
  sigc::signal<void, std::string> signal_phase_changed;
  sigc::signal<void, std::string> signal_phase_envelope_changed;
  
protected:
  
  Gtk::VBox* create_knob(LV2UIClient& lv2, const std::string& label, int port,
                         float red = 0, float green = 0.3, float blue = 1);
  
  void program_selection_changed();
  
  Gtk::TreeView m_program_view;
  
  class ProgramColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    Gtk::TreeModelColumn<int> number;
    Gtk::TreeModelColumn<std::string> name;
    ProgramColumns() { add(number); add(name); }
  } m_program_columns;
  
  Glib::RefPtr<Gtk::ListStore> m_program_store;
  
  PDEditor m_phase;
  EnvelopeEditor m_phase_env;
  ShaperEditor m_shaper;
  EnvelopeEditor m_shape_env;
};


#endif
