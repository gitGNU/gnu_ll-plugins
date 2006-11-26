/****************************************************************************
    
    main.cpp - Main source file for the paqtest program
    
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
#include <iomanip>

#include "query.hpp"
#include "rdf.hpp"
#include "turtleparser.hpp"
#include "namespaces.hpp"


using namespace PAQ;


int main() {
  
  TurtleParser tp;
  RDFData data;
  string text;
  string line;
  
  while (getline(cin, line))
    text += line + "\n";
  //cout<<text;
    
  if (tp.parse_ttl(text, data)) {
    cout << "-------------------------\n";
    cout << "Parsing succeeded\n";
    cout << "-------------------------\n";
  }
  else {
    cout << "-------------------------\n";
    cout << "Parsing failed\n";
    cout << "-------------------------\n";
  }
  
  /*
  Namespace lv2("<http://lv2plug.in/ontology#>");
  Variable uri, plugin, maintainer, blank, type, symbol;
  Query q1 = 
    select(uri, plugin, maintainer)
    .where(uri, rdf("type"), lv2("Plugin"))
    .where(uri, doap("name"), plugin)
    .where(uri, doap("maintainer"), blank)
    .where(blank, foaf("name"), maintainer);
  
  Query q2 =
    select(symbol, type)
    .where(uri, rdf("type"), lv2("Plugin"))
    .where(uri, lv2("port"), blank)
    .where(blank, lv2("datatype"), type)
    .where(blank, rdf("type"), lv2("ControlRateInputPort"))
    .where(blank, lv2("symbol"), symbol);
  
  vector<QueryResult> qr = q2.run(data);
  
  for (int i = 0; i < qr.size(); ++i) {
    cerr<<"Results: "
        <<qr[i][uri]->name<<", "
        <<qr[i][plugin]->name<<", "
        <<qr[i][maintainer]->name<<endl;
    cerr<<"Results: "
        <<qr[i][symbol]->name<<", "
        <<qr[i][type]->name<<endl;
  }
  */
  
  /*
  Namespace lv2("<http://lv2plug.in/ontology#>");
  Variable plugin, port, symbol, type;
  Query q = 
    select(symbol, type)
    .where(plugin, rdf("type"), lv2("Plugin"))
    .where(plugin, lv2("port"), port)
    .where(port, lv2("symbol"), symbol)
    .where(port, lv2("datatype"), type)
    .filter(plugin == port);
  std::vector<QueryResult> result = q.run(data);
  for (size_t i = 0; i < result.size(); ++i) {
    std::cout << "symbol = " << result[i][symbol]->name << ", "
              << "type = " << result[i][type]->name << std::endl;
  }
  */
  
  /*
  Namespace lv2("<http://lv2plug.in/ontology#>");
  Variable plugin, port, name, index;
  Query q = 
    select(plugin, name)
    .where(plugin, doap("name"), name)
    .filter(lang_matches(name, "fr"));
  std::vector<QueryResult> result = q.run(data);
  for (size_t i = 0; i < result.size(); ++i) {
    std::cout << "plugin = " << result[i][plugin]->name << ", "
              << "name = " << result[i][name]->name << std::endl;
  }
  */
  
  Namespace lv2("<http://lv2plug.in/ontology#>");
  Namespace ll("<http://ll-plugins.nongnu.org/lv2/namespace#>");
  Variable name, plugin, port, index, controller, ccnumber;
  Query q = 
    select(name, ccnumber)
    .where(plugin, rdf("type"), lv2("Plugin"))
    .where(plugin, lv2("port"), port)
    .where(port, lv2("name"), name)
    .where(port, lv2("index"), index)
    .where(port, ll("defaultMidiController"), controller)
    .where(controller, ll("controllerType"), ll("CC"))
    .where(controller, ll("controllerNumber"), ccnumber)
    .filter(is_integer(ccnumber))
    .filter(index >= 0)
    .filter(index < 128);
  std::vector<QueryResult> result = q.run(data);
  for (size_t i = 0; i < result.size(); ++i) {
    std::cerr<<std::setw(30)<<result[i][name]->name<<" -> "
             <<result[i][ccnumber]->name<<std::endl;
  }
  return 0;
}
