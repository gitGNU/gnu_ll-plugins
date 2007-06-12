#ifndef SAMPLEEDITOR_HPP
#define SAMPLEEDITOR_HPP

#include <vector>

#include <gtkmm.h>

#include "sampleview.hpp"


class SampleEditor : public Gtk::Frame {
public:
  
  SampleEditor();
  
  bool add_sample(const std::string& name, long length, double rate,
		  const std::string& left, const std::string& right = "");
  bool remove_sample(const std::string& name);
  
  sigc::signal<void, const std::string&>& signal_load_sample();
  sigc::signal<void, const std::string&>& signal_delete_sample();
  sigc::signal<void, const std::string&, const std::string&>& 
  signal_rename_sample();

protected:
  
  void do_load_sample();
  void do_delete_sample();
  void do_rename_sample();
  
  void sample_selected();
  
  sigc::signal<void, const std::string&> m_signal_load_sample;
  sigc::signal<void, const std::string&> m_signal_delete_sample;
  sigc::signal<void, const std::string&, const std::string&>
  m_signal_rename_sample;
  
  SampleView m_view;
  Gtk::FileChooserDialog m_dlg_load;
  Gtk::ComboBoxText m_cmb_sample;
  Gtk::Button m_btn_load;
  Gtk::Button m_btn_rename;
  Gtk::Button m_btn_delete;
  
  std::map<std::string, SampleModel*> m_models;
};


#endif
