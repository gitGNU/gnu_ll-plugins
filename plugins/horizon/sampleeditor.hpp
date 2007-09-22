/****************************************************************************

    sampleeditor.hpp

    Copyright (C) 2007 Lars Luthman <lars.luthman@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef SAMPLEEDITOR_HPP
#define SAMPLEEDITOR_HPP

#include <vector>

#include <gtkmm.h>

#include "effectstackview.hpp"
#include "sampleview.hpp"


class SampleEditor : public Gtk::Frame {
public:
  
  SampleEditor();
  
  bool add_sample(const std::string& name, long length, double rate,
		  const std::string& left, const std::string& right = "");
  bool remove_sample(const std::string& name);
  bool rename_sample(const std::string& old_name, const std::string& new_name);
  bool modify_sample(const std::string& name, 
		     const std::string& left, const std::string& right = "");
  bool add_splitpoint(const std::string& name, size_t frame);
  bool remove_splitpoint(const std::string& name, size_t frame);
  bool move_splitpoint(const std::string& name, size_t frame, size_t newframe);
  bool add_static_effect(const std::string& sample, size_t pos, 
			 const std::string& label);
  bool remove_static_effect(const std::string& sample, size_t pos);
  bool bypass_static_effect(const std::string& sample, size_t pos, bool bypass);
  
  sigc::signal<void, const std::string&>& signal_load_sample();
  sigc::signal<void, const std::string&>& signal_delete_sample();
  sigc::signal<void, const std::string&, const std::string&>& 
  signal_rename_sample();
  sigc::signal<void, const std::string&, size_t>& signal_add_splitpoint();
  sigc::signal<void, const std::string&, size_t>& signal_remove_splitpoint();
  sigc::signal<void, const std::string&, size_t, size_t>& 
  signal_move_splitpoint();
  sigc::signal<void, const std::string&, size_t, size_t>& signal_play_preview();
  sigc::signal<void, const std::string&>& signal_stop_preview();
  sigc::signal<void, const std::string&, size_t, const std::string&>& 
  signal_add_static_effect();
  sigc::signal<void, const std::string&, size_t>& signal_remove_static_effect();
  sigc::signal<void, const std::string&, size_t, bool>& 
  signal_bypass_static_effect();
  
protected:
  
  void do_load_sample();
  void do_delete_sample();
  void do_rename_sample();
  
  void sample_selected();
  
  sigc::signal<void, const std::string&> m_signal_load_sample;
  sigc::signal<void, const std::string&> m_signal_delete_sample;
  sigc::signal<void, const std::string&, const std::string&>
  m_signal_rename_sample;
  sigc::signal<void, const std::string&, size_t> m_signal_add_splitpoint;
  sigc::signal<void, const std::string&, size_t> m_signal_remove_splitpoint;
  sigc::signal<void, const std::string&, size_t, size_t> 
  m_signal_move_splitpoint;
  sigc::signal<void, const std::string&, size_t, size_t> m_signal_play_preview;
  sigc::signal<void, const std::string&> m_signal_stop_preview;
  sigc::signal<void, const std::string&, size_t, const std::string&> 
  m_signal_add_static_effect;
  sigc::signal<void, const std::string&, size_t> m_signal_remove_static_effect;
  sigc::signal<void, const std::string&, size_t, bool> 
  m_signal_bypass_static_effect;
  
  SampleView m_view;
  EffectStackView m_sview;
  
  Gtk::FileChooserDialog m_dlg_load;
  Gtk::ComboBoxText m_cmb_sample;
  Gtk::Button m_btn_load;
  Gtk::Button m_btn_rename;
  Gtk::Button m_btn_delete;
  
  std::map<std::string, SampleModel*> m_models;
};


#endif
