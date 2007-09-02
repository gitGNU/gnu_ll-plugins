/****************************************************************************
    
    euphoriawidget.hpp - A GUI for the Euphoria LV2 synth
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA

****************************************************************************/

#ifndef EUPHORIAWIDGET_HPP
#define EUPHORIAWIDGET_HPP

#include <string>

#include <gtkmm.h>

#include "shapereditor.hpp"
#include "springeditor.hpp"
#include "pdeditor.hpp"
#include "transitioneditor.hpp"
#include "envelopeeditor.hpp"


class EuphoriaWidget : public Gtk::VBox {
public:
  
  EuphoriaWidget();
  
  void add_program(int number, const std::string& name);
  void remove_program(int number);
  void clear_programs();
  void set_program(int number);
  
  void set_control(uint32_t port, float value);
  void configure(const std::string& key, const std::string& value);
  
  sigc::signal<void, int> signal_program_selected;
  sigc::signal<void, const std::string&, const std::string&> signal_configure;
  sigc::signal<void, uint32_t, float> signal_control_changed;
  
protected:
  
  Gtk::VBox* create_knob(const std::string& label, int port,
                         float red = 0, float green = 0.3, float blue = 1);
  Gtk::ToggleButton* create_toggle(const std::string& label, int port);
  static float toggle2float(bool active);
  static bool float2toggle(float value);
  
  void program_selection_changed();
  
  Gtk::Widget& init_pd_controls(); 
  Gtk::Widget& init_shp_controls();
  Gtk::Widget& init_spr_controls();
  Gtk::Widget& init_markov_controls();
  Gtk::Widget& init_fx_controls();
  Gtk::Widget& init_voice_controls();
  Gtk::Widget& init_program_controls();
  Gtk::Widget& init_global_controls();
  Gtk::Widget& init_bottom_buttons();
  

  Gtk::TreeView m_program_view;
  
  class ProgramColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    Gtk::TreeModelColumn<int> number;
    Gtk::TreeModelColumn<std::string> name;
    ProgramColumns() { add(number); add(name); }
  } m_program_columns;
  
  Glib::RefPtr<Gtk::ListStore> m_program_store;
  
  PDEditor m_phase;
  EnvelopeEditor m_pd_dist_env;
  EnvelopeEditor m_pd_amp_env;

  ShaperEditor m_shaper;
  EnvelopeEditor m_shp_amount_env;
  EnvelopeEditor m_shp_amp_env;
  
  SpringEditor m_spring;
  EnvelopeEditor m_spr_proj_env;
  
  TransitionEditor m_trans;
  EnvelopeEditor m_mrk_01_env;
  EnvelopeEditor m_mrk_10_env;
  EnvelopeEditor m_mrk_freq_env;
  EnvelopeEditor m_mrk_amp_env;
  
  std::vector<Gtk::Adjustment*> m_adj;
};


#endif
