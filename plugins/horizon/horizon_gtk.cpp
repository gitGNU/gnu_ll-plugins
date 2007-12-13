/****************************************************************************
    
    horizonguiplugin.cpp - A GUI for the Horizon LV2 synth
    
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

#include <iostream>
#include <sstream>

#include <gtkmm.h>

#include "lv2gui.hpp"
#include "horizon.peg"
#include "controlsourcegui.hpp"
#include "sampleeditor.hpp"
#include "chunkeditor.hpp"
#include "triggereditor.hpp"
#include "actiontriggermodel.hpp"
#include "horizonkeyboard.hpp"


using namespace std;
using namespace Gtk;
using namespace sigc;


class HorizonGUI : public LV2::GUI<HorizonGUI> {
public:

  
  HorizonGUI(const std::string& URI, const std::string& bundle_path) 
    : m_kb(m_atm),
      m_autochunk_number(1) {
    
    HBox* hbox = manage(new HBox(false, 6));
    hbox->set_border_width(6);
    VBox* wrp = manage(new VBox);
    wrp->pack_start(m_csg, false, false);
    hbox->pack_start(*wrp, false, false);
    VBox* vbox = manage(new VBox(false, 6));
    vbox->pack_start(m_sed);
    vbox->pack_start(m_ced, false, false);
    vbox->pack_start(m_ted, false, false);
    vbox->pack_start(m_kb, false, false);
    hbox->pack_start(*vbox);
    pack_start(*hbox);
    
    m_sed.signal_load_sample().
      connect(mem_fun(*this, &HorizonGUI::do_load_sample));
    m_sed.signal_delete_sample().
      connect(mem_fun(*this, &HorizonGUI::do_delete_sample));
    m_sed.signal_rename_sample().
      connect(mem_fun(*this, &HorizonGUI::do_rename_sample));
    m_sed.signal_add_splitpoint().
      connect(mem_fun(*this, &HorizonGUI::do_add_splitpoint));
    m_sed.signal_remove_splitpoint().
      connect(mem_fun(*this, &HorizonGUI::do_remove_splitpoint));
    m_sed.signal_move_splitpoint().
      connect(mem_fun(*this, &HorizonGUI::do_move_splitpoint));
    m_sed.signal_play_preview().
      connect(mem_fun(*this, &HorizonGUI::do_play_preview));
    m_sed.signal_stop_preview().
      connect(mem_fun(*this, &HorizonGUI::do_stop_preview));
    m_sed.signal_add_static_effect().
      connect(mem_fun(*this, &HorizonGUI::do_add_static_effect));
    m_sed.signal_remove_static_effect().
      connect(mem_fun(*this, &HorizonGUI::do_remove_static_effect));
    m_sed.signal_bypass_static_effect().
      connect(mem_fun(*this, &HorizonGUI::do_bypass_static_effect));
    
    m_kb.signal_segments_dropped.
      connect(mem_fun(*this, &HorizonGUI::segments_dropped_on_keyboard));
    m_kb.signal_key_on().
      connect(bind(mem_fun(*this, &HorizonGUI::do_send_note), true));
    m_kb.signal_key_off().
      connect(bind(mem_fun(*this, &HorizonGUI::do_send_note), false));
  }
  
  
  void feedback(uint32_t argc, const char* const* argv) {
    cerr<<"GUI was told:";
    for (unsigned i = 0; i < argc; ++i)
      cerr<<" '"<<argv[i]<<"'";
    cerr<<endl;
    
    if (argc >= 5 && !strcmp(argv[0], "sample_loaded")) {
      if (argc == 5)
	m_sed.add_sample(argv[1], atol(argv[2]), atof(argv[3]), argv[4]);
      else if (argc == 6)
	m_sed.add_sample(argv[1], atol(argv[2]), atof(argv[3]), argv[4], 
			 argv[5]);
      else
	cerr<<"Too many channels for the GUI!"<<endl;
    }
    
    else if (argc == 2 && !strcmp(argv[0], "sample_deleted")) {
      m_sed.remove_sample(argv[1]);
    }
    
    else if (argc == 3 && !strcmp(argv[0], "sample_renamed")) {
      m_sed.rename_sample(argv[1], argv[2]);
    }
    
    else if (argc == 3 && !strcmp(argv[0], "sample_modified")) {
      m_sed.modify_sample(argv[1], argv[2]);
    }
    
    else if (argc == 4 && !strcmp(argv[0], "sample_modified")) {
      m_sed.modify_sample(argv[1], argv[2], argv[3]);
    }
    
    else if (argc == 3 && !strcmp(argv[0], "splitpoint_added")) {
      m_sed.add_splitpoint(argv[1], atol(argv[2]));
    }

    else if (argc == 3 && !strcmp(argv[0], "splitpoint_removed")) {
      m_sed.remove_splitpoint(argv[1], atol(argv[2]));
    }

    else if (argc == 4 && !strcmp(argv[0], "splitpoint_moved")) {
      m_sed.move_splitpoint(argv[1], atol(argv[2]), atol(argv[3]));
    }
    
    else if (argc == 4 && !strcmp(argv[0], "static_effect_added")) {
      m_sed.add_static_effect(argv[1], atol(argv[2]), argv[3]);
    }
    
    else if (argc == 3 && !strcmp(argv[0], "static_effect_removed")) {
      m_sed.remove_static_effect(argv[1], atol(argv[2]));
    }
    
    else if (argc == 4 && !strcmp(argv[0], "static_effect_bypassed")) {
      m_sed.bypass_static_effect(argv[1], atol(argv[2]), atol(argv[3]));
    }
    
    else if (argc == 5 && !strcmp(argv[0], "chunk_added")) {

    }
    
    else if (argc == 5 && !strcmp(argv[0], "trigger_added")) {
      m_atm.add_action(argv[4], argv[2], argv[3]);
      m_atm.map_action(argv[4], atoi(argv[1]), atoi(argv[1]));
    }
    
  }
  
  
protected:
  
  void do_load_sample(const string& filename) {
    const char* argv[] = { "load_sample", 0 };
    argv[1] = filename.c_str();
    // XXX m_ctrl.command(2, argv);
  }
      
  void do_delete_sample(const string& sample) {
    const char* argv[] = { "delete_sample", 0 };
    argv[1] = sample.c_str();
    // XXX m_ctrl.command(2, argv);
  }
      
  void do_rename_sample(const string& old_name, const string& new_name) {
    const char* argv[] = { "rename_sample", 0, 0 };
    argv[1] = old_name.c_str();
    argv[2] = new_name.c_str();
    // XXX m_ctrl.command(3, argv);
  }
  
  void do_add_splitpoint(const string& sample, size_t frame) {
    const char* argv[] = { "add_splitpoint", 0, 0 };
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<frame;
    string tmp = oss.str();
    argv[2] = tmp.c_str();
    cerr<<"Adding splitpoint "<<argv[2]<<endl;

    cerr<<__PRETTY_FUNCTION__<<endl;
    cerr<<"POINTERS:"<<endl;
    cerr<<"  "<<(const void*)(argv[0])<<endl;
    cerr<<"  "<<(const void*)(argv[1])<<endl;
    cerr<<"  "<<(const void*)(argv[2])<<endl;

    // XXX m_ctrl.command(3, argv);
  }
      
  void do_remove_splitpoint(const string& sample, size_t frame) {
    const char* argv[] = { "remove_splitpoint", 0, 0 };
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<frame;
    string tmp = oss.str();
    argv[2] = tmp.c_str();
    // XXX m_ctrl.command(3, argv);
  }
      
  void do_move_splitpoint(const string& sample, size_t frame, size_t newframe) {
    const char* argv[] = { "move_splitpoint", 0, 0, 0};
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<frame;
    string a2 = oss.str();
    argv[2] = a2.c_str();
    oss.str("");
    oss<<newframe;
    string a3 = oss.str();
    argv[3] = a3.c_str();
    // XXX m_ctrl.command(4, argv);
  }
  
  
  void do_play_preview(const string& sample, size_t start, size_t end) {
    const char* argv[] = { "play_preview", 0, 0, 0};
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<start;
    string a2 = oss.str();
    argv[2] = a2.c_str();
    oss.str("");
    oss<<end;
    string a3 = oss.str();
    argv[3] = a3.c_str();
    // XXX m_ctrl.command(4, argv);
  }
  
  
  void do_stop_preview(const string& sample) {
    const char* argv[] = { "stop_preview", 0 };
    argv[1] = sample.c_str();
    // XXX m_ctrl.command(2, argv);
  }
  
  
  void do_add_static_effect(const string& sample, size_t index, 
			    const string& effect_uri) {
    const char* argv[] = { "add_static_effect", 0, 0, 0 };
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<index;
    string tmp = oss.str();
    argv[2] = tmp.c_str();
    argv[3] = effect_uri.c_str();
    // XXX m_ctrl.command(4, argv);
  }
  
  
  void do_remove_static_effect(const string& sample, size_t index) {
    const char* argv[] = { "remove_static_effect", 0, 0 };
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<index;
    string tmp = oss.str();
    argv[2] = tmp.c_str();
    // XXX m_ctrl.command(3, argv);
  }
  
  
  void do_bypass_static_effect(const string& sample, size_t index, bool yeah) {
    const char* argv[] = { "bypass_static_effect", 0, 0, 0 };
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<index;
    string tmp = oss.str();
    argv[2] = tmp.c_str();
    argv[3] = yeah ? "1" : "0";
    // XXX m_ctrl.command(4, argv);
  }
  
  
  void do_add_chunk(const string& sample, size_t from, size_t to, 
		    const string& name) {
    const char* argv[] = { "add_chunk", sample.c_str(), 0, 0, name.c_str() };
    ostringstream oss;
    oss<<from;
    string tmp1 = oss.str();
    argv[2] = tmp1.c_str();
    oss.str("");
    oss<<to;
    string tmp2 = oss.str();
    argv[3] = tmp2.c_str();
    // XXX m_ctrl.command(5, argv);
  }
  
  
  void do_add_trigger(unsigned char key, const string& sample, 
		      const string& chunk, const string& name) {
    const char* argv[] = { "add_trigger", 0, sample.c_str(),
			   chunk.c_str(), name.c_str() };
    ostringstream oss;
    oss<<int(key);
    string tmp = oss.str();
    argv[1] = tmp.c_str();
    // XXX m_ctrl.command(5, argv);
  }
  
  
  void do_send_note(unsigned char key, bool on) {
    unsigned char event[] = { on ? 0x90 : 0x80, key, 64 };
    // XXX m_ctrl.write(h_midi_input, 3, event);
  }
  
  
  void set_control(uint32_t port, float value) {

  }

  void add_program(unsigned char number, const char* name) {

  }
  
  void remove_program(unsigned char number) {

  }
  
  void clear_programs() {

  }
  
  void set_program(unsigned char number) {

  }
  
  
  void segments_dropped_on_keyboard(const string& sample, 
				    unsigned first, unsigned last,
				    unsigned char key) {
    ostringstream oss;
    oss<<"Auto chunk "<<(m_autochunk_number++);
    string chunk = oss.str();
    do_add_chunk(sample, first, last, chunk);
    do_add_trigger(key, sample, chunk, sample + "/" + chunk + "/play");
  }
  
protected:
  
  ControlSourceGUI m_csg;
  SampleEditor m_sed;
  ChunkEditor m_ced;
  TriggerEditor m_ted;
  ActionTriggerModel m_atm;
  HorizonKeyboard m_kb;
  unsigned long m_autochunk_number;
  
};


static int _ = HorizonGUI::register_class((string(h_uri) + "/gui").c_str());
