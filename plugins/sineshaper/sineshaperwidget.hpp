/****************************************************************************
    
    sineshaperwidget.hpp - A GUI for the Sineshaper LV2 synth plugin
    
    Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
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

#ifndef SINESHAPERWIDGET_HPP
#define SINESHAPERWIDGET_HPP

#include <gtkmm.h>


class SineshaperWidget : public Gtk::HBox {
public:
  
  SineshaperWidget();

protected:
  
  Gtk::Widget* init_tuning_controls();
  Gtk::Widget* init_osc2_controls();
  Gtk::Widget* init_vibrato_controls();
  Gtk::Widget* init_portamento_controls();
  Gtk::Widget* init_tremolo_controls();
  Gtk::Widget* init_envelope_controls();
  Gtk::Widget* init_amp_controls();
  Gtk::Widget* init_delay_controls();
  Gtk::Widget* init_shaper_controls();
  Gtk::Widget* init_preset_list();

};


#endif
