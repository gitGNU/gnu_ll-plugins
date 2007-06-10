#ifndef SAMPLEEDITOR_HPP
#define SAMPLEEDITOR_HPP

#include <gtkmm.h>


class SampleEditor : public Gtk::Frame {
public:
  
  SampleEditor();
  
  sigc::signal<void, const std::string&>& signal_load_sample();
  sigc::signal<void, const std::string&>& signal_delete_sample();
  sigc::signal<void, const std::string&, const std::string&>& 
  signal_rename_sample();

protected:
  
  void do_load_sample();
  void do_delete_sample();
  void do_rename_sample();
  
  sigc::signal<void, const std::string&> m_signal_load_sample;
  sigc::signal<void, const std::string&> m_signal_delete_sample;
  sigc::signal<void, const std::string&, const std::string&>
  m_signal_rename_sample;
  
  Gtk::FileChooserDialog m_dlg_load;
};


#endif
