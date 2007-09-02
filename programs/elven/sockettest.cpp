/****************************************************************************
    
    sockettest.cpp - A simple test program that embeds windows from other 
                     processes
    
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

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/socket.h>


using namespace Gtk;
using namespace std;


int main(int argc, char** argv) {
  Main kit(argc, argv);
  Window win;
  win.set_title("Socket");
  Socket socket;
  win.add(socket);
  win.show_all();
  cout<<"Socket window ID: "<<socket.get_id()<<endl;
  kit.run(win);
  return 0;
}
