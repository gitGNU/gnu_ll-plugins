/****************************************************************************
    
    horizonguiplugin.cpp - A GUI for the Horizon LV2 synth
    
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

#include <iostream>
#include <sstream>

#include <gtkmm.h>

#include "lv2gtk2gui.hpp"
#include "horizon.peg"
#include "controlsourcegui.hpp"
#include "sampleeditor.hpp"
#include "chunkeditor.hpp"
#include "triggereditor.hpp"


using namespace std;
using namespace Gtk;
using namespace sigc;


class HorizonGUI : public LV2GTK2GUI {
public:

  
  HorizonGUI(LV2Controller& ctrl, const std::string& URI, 
              const std::string& bundle_path) 
    : m_ctrl(ctrl) {
    
    HBox* hbox = manage(new HBox(false, 6));
    hbox->set_border_width(6);
    VBox* wrp = manage(new VBox);
    wrp->pack_start(m_csg, false, false);
    hbox->pack_start(*wrp, false, false);
    VBox* vbox = manage(new VBox(false, 6));
    vbox->pack_start(m_sed);
    vbox->pack_start(m_ced, false, false);
    vbox->pack_start(m_ted, false, false);
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
    m_sed.signal_add_static_effect().
      connect(mem_fun(*this, &HorizonGUI::do_add_static_effect));
    m_sed.signal_remove_static_effect().
      connect(mem_fun(*this, &HorizonGUI::do_remove_static_effect));
    m_sed.signal_bypass_static_effect().
      connect(mem_fun(*this, &HorizonGUI::do_bypass_static_effect));
    
  }
  
  
  void tell_gui(uint32_t argc, const char* const* argv) {
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
    
  }
  
  
protected:
  
  void do_load_sample(const string& filename) {
    const char* argv[] = { "load_sample", 0 };
    argv[1] = filename.c_str();
    m_ctrl.tell_plugin(2, argv);
  }
      
  void do_delete_sample(const string& sample) {
    const char* argv[] = { "delete_sample", 0 };
    argv[1] = sample.c_str();
    m_ctrl.tell_plugin(2, argv);
  }
      
  void do_rename_sample(const string& old_name, const string& new_name) {
    const char* argv[] = { "rename_sample", 0, 0 };
    argv[1] = old_name.c_str();
    argv[2] = new_name.c_str();
    m_ctrl.tell_plugin(3, argv);
  }
  
  void do_add_splitpoint(const string& sample, size_t frame) {
    const char* argv[] = { "add_splitpoint", 0, 0 };
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<frame;
    argv[2] = oss.str().c_str();
    m_ctrl.tell_plugin(3, argv);
  }
      
  void do_remove_splitpoint(const string& sample, size_t frame) {
    const char* argv[] = { "remove_splitpoint", 0, 0 };
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<frame;
    argv[2] = oss.str().c_str();
    m_ctrl.tell_plugin(3, argv);
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
    m_ctrl.tell_plugin(4, argv);
  }
  
  
  void do_add_static_effect(const string& sample, size_t index, 
			    const string& effect_uri) {
    const char* argv[] = { "add_static_effect", 0, 0, 0 };
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<index;
    argv[2] = oss.str().c_str();
    argv[3] = effect_uri.c_str();
    m_ctrl.tell_plugin(4, argv);
  }
  
  
  void do_remove_static_effect(const string& sample, size_t index) {
    const char* argv[] = { "remove_static_effect", 0, 0 };
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<index;
    argv[2] = oss.str().c_str();
    m_ctrl.tell_plugin(3, argv);
  }
  
  
  void do_bypass_static_effect(const string& sample, size_t index, bool yeah) {
    const char* argv[] = { "bypass_static_effect", 0, 0, 0 };
    argv[1] = sample.c_str();
    ostringstream oss;
    oss<<index;
    argv[2] = oss.str().c_str();
    argv[3] = yeah ? "1" : "0";
    m_ctrl.tell_plugin(4, argv);
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
  
protected:
  
  LV2Controller& m_ctrl;
  
  ControlSourceGUI m_csg;
  SampleEditor m_sed;
  ChunkEditor m_ced;
  TriggerEditor m_ted;
  
};


void initialise() __attribute__((constructor));
void initialise() {
  register_lv2gtk2gui<HorizonGUI>(string(h_uri) + "/gui");
}
