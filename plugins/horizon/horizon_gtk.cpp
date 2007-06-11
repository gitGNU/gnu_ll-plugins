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
    
  }
  
  
  void tell_gui(uint32_t argc, const char* const* argv) {
    cerr<<"GUI was told:";
    for (unsigned i = 0; i < argc; ++i)
      cerr<<" '"<<argv[i]<<"'";
    cerr<<endl;
  }
  
  
protected:
  
  void do_load_sample(const string& filename) {
    static const char* argv[] = { "load_sample", 0 };
    argv[1] = filename.c_str();
    m_ctrl.tell_plugin(2, argv);
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
