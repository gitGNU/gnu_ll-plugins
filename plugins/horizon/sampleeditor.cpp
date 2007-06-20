#include <iostream>

#include <sigc++/sigc++.h>

#include "sampleeditor.hpp"
#include "samplemodel.hpp"
#include "sampleview.hpp"


using namespace Gtk;
using namespace sigc;
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
  : m_dlg_load("Select a sample file"),
    m_btn_load("Load"),
    m_btn_rename("Rename"),
    m_btn_delete("Delete") {
  
  set_shadow_type(SHADOW_IN);
  set_label_widget(*manage(new MLabel("<b>Samples</b>")));
  
  VBox* vbox = manage(new VBox(false, 6));
  vbox->set_border_width(6);
  
  m_view.set_size_request(100, 100);
  VBox* scrbx = manage(new VBox(false, 3));
  scrbx->pack_start(m_view);
  HScrollbar* scrb = manage(new HScrollbar(m_view.get_scroll_adjustment()));
  scrbx->pack_start(*scrb);

  HBox* bottom_row = manage(new HBox(false, 6));
  m_cmb_sample.append_text("No samples");
  m_cmb_sample.set_active_text("No samples");
  m_cmb_sample.set_sensitive(false);
  bottom_row->pack_start(m_cmb_sample, false, false);
  m_btn_delete.set_sensitive(false);
  m_btn_rename.set_sensitive(false);
  m_btn_load.signal_clicked().
    connect(mem_fun(*this, &SampleEditor::do_load_sample));
  m_btn_delete.signal_clicked().
    connect(mem_fun(*this, &SampleEditor::do_delete_sample));
  m_btn_rename.signal_clicked().
    connect(mem_fun(*this, &SampleEditor::do_rename_sample));
  bottom_row->pack_end(m_btn_load, false, false);
  bottom_row->pack_end(m_btn_delete, false, false);
  bottom_row->pack_end(m_btn_rename, false, false);
  
  vbox->pack_start(*scrbx, true, true);
  vbox->pack_end(*bottom_row, false, false);
  add(*vbox);
  
  m_dlg_load.add_button(Stock::CANCEL, RESPONSE_CANCEL);
  m_dlg_load.add_button(Stock::OPEN, RESPONSE_OK);
  m_dlg_load.set_default_response(RESPONSE_OK); 
  
  m_cmb_sample.signal_changed().
    connect(mem_fun(*this, &SampleEditor::sample_selected));
  
  m_view.signal_add_splitpoint().
    connect(group(m_signal_add_splitpoint, 
		  group(sigc::hide(mem_fun(m_cmb_sample, 
					   &ComboBoxText::get_active_text)),
			_1), _1));
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


sigc::signal<void, const std::string&, size_t>& 
SampleEditor::signal_add_splitpoint() {
  return m_signal_add_splitpoint;
}


bool SampleEditor::add_sample(const std::string& name, long length, double rate,
			      const std::string& left,
			      const std::string& right) {
  
  SampleModel* model = new SampleModel(name, length, rate, left, right);
  // XXX do not overwrite here! will leak!
  m_models[name] = model;
  m_view.set_model(model);
  m_cmb_sample.remove_text("No samples");
  m_cmb_sample.append_text(name);
  m_cmb_sample.set_active_text(name);
  m_cmb_sample.set_sensitive(true);
  m_btn_rename.set_sensitive(true);
  m_btn_delete.set_sensitive(true);
  
  return true;
}


bool SampleEditor::rename_sample(const std::string& old_name, 
				 const std::string& new_name) {
  
  std::map<string, SampleModel*>::iterator iter = m_models.find(new_name);
  
  if (iter != m_models.end())
    return false;
  
  iter = m_models.find(old_name);
  if (iter == m_models.end())
    return false;
  
  SampleModel* model = iter->second;
  m_models.erase(iter);
  m_models[new_name] = model;
  model->set_name(new_name);
  
  // XXX need to preserve the order of the samples in the combo box
  string selected = m_cmb_sample.get_active_text();
  if (selected == old_name)
    selected = new_name;
  m_cmb_sample.clear();
  for (iter = m_models.begin(); iter != m_models.end(); ++iter)
    m_cmb_sample.append_text(iter->first);
  m_cmb_sample.set_active_text(selected);
  
  return true;
}


void SampleEditor::do_load_sample() {
  m_dlg_load.show_all();
  if (m_dlg_load.run() == RESPONSE_OK)
    m_signal_load_sample(m_dlg_load.get_filename());
  m_dlg_load.hide();
}


bool SampleEditor::remove_sample(const std::string& name) {
  std::map<string, SampleModel*>::iterator iter = m_models.find(name);
  if (iter != m_models.end()) {
    m_cmb_sample.remove_text(name);
    if (m_cmb_sample.get_model()->children().size() == 0) {
      m_cmb_sample.append_text("No samples");
      m_cmb_sample.set_active_text("No samples");
      m_cmb_sample.set_sensitive(false);
      m_btn_delete.set_sensitive(false);
      m_btn_rename.set_sensitive(false);
      m_view.set_model(0);
    }
    else
      m_cmb_sample.set_active(0);
    
    delete iter->second;
    m_models.erase(iter);

    return true;
  }
  
  return false;
}


bool SampleEditor::add_splitpoint(const std::string& name, size_t frame) {
  std::map<string, SampleModel*>::iterator iter = m_models.find(name);
  if (iter != m_models.end()) {
    iter->second->add_splitpoint(frame);
    if (m_cmb_sample.get_active_text() == iter->second->get_name())
      m_view.queue_draw();
    return true;
  }
  
  return false;
  
}


void SampleEditor::do_delete_sample() {
  MessageDialog msg_dlg(string("Are you sure that you want to remove "
			       "the sample <b><i>") + 
			m_cmb_sample.get_active_text() + "</i></b> and all its "
			"chunks and related triggers?", true,
			MESSAGE_WARNING, BUTTONS_YES_NO, true);
  msg_dlg.show_all();
  if (msg_dlg.run() == RESPONSE_YES)
    m_signal_delete_sample(m_cmb_sample.get_active_text());
}


void SampleEditor::do_rename_sample() {
  Dialog dlg(string("Rename ") + m_cmb_sample.get_active_text());
  HBox hbox(false, 6);
  Label lbl("New name:");
  Entry ent;
  ent.set_size_request(300, -1);
  hbox.pack_start(lbl, PACK_SHRINK);
  hbox.pack_start(ent, PACK_EXPAND_WIDGET);
  hbox.set_border_width(6);
  dlg.get_vbox()->pack_start(hbox);
  dlg.get_vbox()->set_spacing(6);
  dlg.show_all();
  dlg.add_button(Stock::CANCEL, RESPONSE_CANCEL);
  dlg.add_button(Stock::OK, RESPONSE_OK);
  if (dlg.run() == RESPONSE_OK) {
    m_signal_rename_sample(m_cmb_sample.get_active_text(),
			   ent.get_text());
  }
  dlg.hide();
}


void SampleEditor::sample_selected() {
  std::map<string, SampleModel*>::iterator iter = 
    m_models.find(m_cmb_sample.get_active_text());
  if (iter != m_models.end())
    m_view.set_model(iter->second);
}

