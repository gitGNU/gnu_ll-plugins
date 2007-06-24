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
  
  // the main vbox that holds all components
  VBox* vbox = manage(new VBox(false, 6));
  vbox->set_border_width(6);
  
  // the sample and effect stack widgets
  Table* stab = manage(new Table(2, 2));
  stab->set_row_spacings(3);
  m_view.set_size_request(100, 100);
  stab->attach(m_sview, 0, 1, 0, 1, AttachOptions(0));
  stab->attach(m_view, 1, 2, 0, 1);
  HScrollbar* scrb = manage(new HScrollbar(m_view.get_scroll_adjustment()));
  stab->attach(*scrb, 1, 2, 1, 2, FILL | EXPAND, AttachOptions(0));
  
  // combo box and buttons
  HBox* bottom_row = manage(new HBox(false, 6));
  m_cmb_sample.append_text("No samples");
  m_cmb_sample.set_active_text("No samples");
  m_cmb_sample.set_sensitive(false);
  bottom_row->pack_start(m_cmb_sample, false, false);
  m_btn_delete.set_sensitive(false);
  m_btn_rename.set_sensitive(false);
  bottom_row->pack_end(m_btn_load, false, false);
  bottom_row->pack_end(m_btn_delete, false, false);
  bottom_row->pack_end(m_btn_rename, false, false);
  
  // arrange the components
  vbox->pack_start(*stab, true, true);
  vbox->pack_end(*bottom_row, false, false);
  add(*vbox);
  
  // set up the sample loading dialog
  m_dlg_load.add_button(Stock::CANCEL, RESPONSE_CANCEL);
  m_dlg_load.add_button(Stock::OPEN, RESPONSE_OK);
  m_dlg_load.set_default_response(RESPONSE_OK); 
  
  // connect some signals
  m_btn_load.signal_clicked().
    connect(mem_fun(*this, &SampleEditor::do_load_sample));
  m_btn_delete.signal_clicked().
    connect(mem_fun(*this, &SampleEditor::do_delete_sample));
  m_btn_rename.signal_clicked().
    connect(mem_fun(*this, &SampleEditor::do_rename_sample));

  m_cmb_sample.signal_changed().
    connect(mem_fun(*this, &SampleEditor::sample_selected));
  
  slot<Glib::ustring> get_sample = mem_fun(m_cmb_sample, 
  					   &ComboBoxText::get_active_text);
  
  m_view.signal_add_splitpoint().
    connect(group(m_signal_add_splitpoint, 
		  group(sigc::hide(get_sample), _1), _1));
  m_view.signal_remove_splitpoint().
    connect(group(m_signal_remove_splitpoint, 
		  group(sigc::hide(get_sample), _1), _1));
  m_view.signal_move_splitpoint().
    connect(group(m_signal_move_splitpoint, 
		  group(sigc::hide(get_sample), _1), _1, _2));
  m_sview.signal_add_effect().
    connect(group(m_signal_add_static_effect,
		  group(sigc::hide(get_sample), _1), _1, _2));
  m_sview.signal_remove_effect().
    connect(group(m_signal_remove_static_effect,
		  group(sigc::hide(get_sample), _1), _1));
  m_sview.signal_bypass_effect().
    connect(group(m_signal_bypass_static_effect,
		  group(sigc::hide(get_sample), _1), _1, _2));
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


sigc::signal<void, const std::string&, size_t>& 
SampleEditor::signal_remove_splitpoint() {
  return m_signal_remove_splitpoint;
}


sigc::signal<void, const std::string&, size_t, size_t>& 
SampleEditor::signal_move_splitpoint() {
  return m_signal_move_splitpoint;
}


sigc::signal<void, const std::string&, size_t, const std::string&>& 
SampleEditor::signal_add_static_effect() {
  return m_signal_add_static_effect;
}


sigc::signal<void, const std::string&, size_t>& 
SampleEditor::signal_remove_static_effect() {
  return m_signal_remove_static_effect;
}


sigc::signal<void, const std::string&, size_t, bool>& 
SampleEditor::signal_bypass_static_effect() {
  return m_signal_bypass_static_effect;
}


bool SampleEditor::add_sample(const std::string& name, long length, double rate,
			      const std::string& left,
			      const std::string& right) {
  
  SampleModel* model = new SampleModel(name, length, rate, left, right);
  // XXX do not overwrite here! will leak!
  m_models[name] = model;
  m_view.set_model(model);
  m_sview.set_model(model);
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
      m_sview.set_model(0);
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


bool SampleEditor::remove_splitpoint(const std::string& name, size_t frame) {
  std::map<string, SampleModel*>::iterator iter = m_models.find(name);
  if (iter != m_models.end()) {
    iter->second->remove_splitpoint(frame);
    if (m_cmb_sample.get_active_text() == iter->second->get_name())
      m_view.queue_draw();
    return true;
  }
  
  return false;
}


bool SampleEditor::move_splitpoint(const std::string& name, 
				   size_t frame, size_t newframe) {
  std::map<string, SampleModel*>::iterator iter = m_models.find(name);
  if (iter != m_models.end()) {
    iter->second->move_splitpoint(frame, newframe);
    if (m_cmb_sample.get_active_text() == iter->second->get_name())
      m_view.queue_draw();
    return true;
  }
  
  return false;
}


bool SampleEditor::add_static_effect(const std::string& sample, size_t pos, 
				     const std::string& label) {
  std::map<string, SampleModel*>::iterator iter = m_models.find(sample);
  if (iter != m_models.end() &&
      iter->second->get_effect_stack_model().insert_effect(pos, label)) {
    m_view.queue_draw();
    m_sview.queue_draw();
    return true;
  }
  return false;
}


bool SampleEditor::remove_static_effect(const std::string& sample, size_t pos) {
  std::map<string, SampleModel*>::iterator iter = m_models.find(sample);
  if (iter != m_models.end() &&
      iter->second->get_effect_stack_model().remove_effect(pos)) {
    m_view.queue_draw();
    m_sview.queue_draw();
    return true;
  }
  return false;
}


bool SampleEditor::bypass_static_effect(const std::string& sample, 
					size_t pos, bool bypass) {
  std::map<string, SampleModel*>::iterator iter = m_models.find(sample);
  if (iter != m_models.end() &&
      iter->second->get_effect_stack_model().bypass_effect(pos, bypass)) {
    m_view.queue_draw();
    m_sview.queue_draw();
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
  if (iter != m_models.end()) {
    m_view.set_model(iter->second);
    m_sview.set_model(iter->second);
  }
}

