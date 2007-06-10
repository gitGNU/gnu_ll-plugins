#include <iostream>

#include "sampleeditor.hpp"


using namespace Gtk;
using namespace std;


namespace {
  
  class MLabel : public Gtk::Label {
  public:
    MLabel(const std::string& str) 
      : Gtk::Label(str) {
      set_use_markup(true);
    }
  };

}


SampleEditor::SampleEditor() 
  : m_dlg_load("Select a sample file") {
  
  set_shadow_type(SHADOW_IN);
  set_label_widget(*manage(new MLabel("<b>Samples</b>")));
  
  VBox* vbox = manage(new VBox(false, 6));
  vbox->set_border_width(6);
  
  ScrolledWindow* scrw = manage(new ScrolledWindow);
  scrw->set_policy(POLICY_ALWAYS, POLICY_NEVER);
  scrw->set_shadow_type(SHADOW_IN);
  int w, h;
  scrw->get_size_request(w, h);
  scrw->set_size_request(w, 100);

  HBox* bottom_row = manage(new HBox(false, 6));
  ComboBoxText* smpcbt = manage(new ComboBoxText);
  smpcbt->append_text("No samples");
  bottom_row->pack_start(*smpcbt, false, false);
  Button* btn_load = manage(new Button("Load"));
  Button* btn_delete = manage(new Button("Delete"));
  Button* btn_rename = manage(new Button("Rename"));
  btn_load->signal_clicked().
    connect(mem_fun(*this, &SampleEditor::do_load_sample));
  btn_delete->signal_clicked().
    connect(mem_fun(*this, &SampleEditor::do_delete_sample));
  btn_rename->signal_clicked().
    connect(mem_fun(*this, &SampleEditor::do_rename_sample));
  bottom_row->pack_end(*btn_load, false, false);
  bottom_row->pack_end(*btn_delete, false, false);
  bottom_row->pack_end(*btn_rename, false, false);
  
  vbox->pack_start(*scrw, true, true);
  vbox->pack_end(*bottom_row, false, false);
  add(*vbox);
  
  m_dlg_load.add_button(Stock::CANCEL, RESPONSE_CANCEL);
  m_dlg_load.add_button(Stock::OPEN, RESPONSE_OK);
  m_dlg_load.set_default_response(RESPONSE_OK); 
}


sigc::signal<void, const std::string&>& SampleEditor::signal_load_sample() {
  return m_signal_load_sample;
}


sigc::signal<void, const std::string&>& SampleEditor::signal_delete_sample() {
  return m_signal_delete_sample;
}


sigc::signal<void, const std::string&, const std::string&>& 
SampleEditor::signal_rename_sample() {
  return m_signal_rename_sample;
}


void SampleEditor::do_load_sample() {
  m_dlg_load.show_all();
  if (m_dlg_load.run() == RESPONSE_OK)
    m_signal_load_sample(m_dlg_load.get_filename());
  m_dlg_load.hide();
}


void SampleEditor::do_delete_sample() {
  cerr<<"Deleting samples is not supported!"<<endl;
}


void SampleEditor::do_rename_sample() {
  cerr<<"Renaming samples is not supported!"<<endl;
}


