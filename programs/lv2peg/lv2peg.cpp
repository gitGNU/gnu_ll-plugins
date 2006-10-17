#include <cfloat>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "turtleparser.hpp"
#include "query.hpp"
#include "namespaces.hpp"


using namespace std;
using namespace PAQ;


struct PortInfo {
  PortInfo() : min(-FLT_MAX), max(FLT_MAX), default_value(min) { }
  string name;
  float min;
  float max;
  float default_value;
};


int main(int argc, char** argv) {
  
  if (argc < 3) {
    cerr<<"Use like this: lv2peg <input file> <output file> "<<endl;
    return -1;
  }
  
  // parse turtle file
  TurtleParser tp;
  RDFData data;
  string text;
  string line;
  ifstream fin(argv[1]);
  while (getline(fin, line))
    text += line + "\n";
  if (!tp.parse_ttl(text, data)) {
    cerr<<"Could not parse input. Not valid Turtle syntax."<<endl;
    return -1;
  }
  
  // find all plugins in the file
  Namespace lv2("<http://lv2plug.in/ontology#>");
  Namespace ll("<http://ll-plugins.nongnu.org/lv2/namespace#>");
  Variable plugin, pegname;
  vector<QueryResult> qr =
    select(plugin, pegname)
    .where(plugin, ll("pegName"), pegname)
    .where(plugin, rdf("type"), lv2("Plugin"))
    .run(data);
  map<string, string> plugins;
  for (int i = 0; i < qr.size(); ++i)
    plugins[qr[i][plugin]->name] = qr[i][pegname]->name;
  
  // iterate over all plugins
  map<string, map<int, PortInfo> > info;
  map<string, string>::const_iterator plug_iter;
  for (plug_iter = plugins.begin(); plug_iter != plugins.end(); ++plug_iter) {
    // query the ports
    Variable port, index, symbol;
    qr = select(index, symbol)
      .where(plug_iter->first, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("symbol"), symbol)
      .run(data);
  
    // put ports in a map so they get sorted
    map<int, PortInfo> ports;
    for (size_t i = 0; i < qr.size(); ++i) {
      int port_index = atoi(qr[i][index]->name.c_str());
      ports[port_index].name = qr[i][symbol]->name;
    }
    
    // check that the port indices are OK
    map<int, PortInfo>::const_iterator iter;
    int next = 0;
    for (iter = ports.begin(); iter != ports.end(); ++iter) {
      if (iter->first != next) {
        cerr<<"There was no description of port "<<next
            <<" in plugin "<<plug_iter->first<<endl;
        return -1;
      }
      ++next;
    }
    
    // get min values
    Variable value;
    qr = select(index, value)
      .where(plug_iter->first, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("minimum"), value)
      .run(data);
    for (int i = 0; i < qr.size(); ++i)
      ports[atoi(qr[i][index]->name.c_str())].min = 
        atof(qr[i][value]->name.c_str());

    // get max values
    qr = select(index, value)
      .where(plug_iter->first, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("maximum"), value)
      .run(data);
    for (int i = 0; i < qr.size(); ++i)
      ports[atoi(qr[i][index]->name.c_str())].max = 
        atof(qr[i][value]->name.c_str());
    
    // get default values
    qr = select(index, value)
      .where(plug_iter->first, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("default"), value)
      .run(data);
    for (int i = 0; i < qr.size(); ++i)
      ports[atoi(qr[i][index]->name.c_str())].default_value = 
        atof(qr[i][value]->name.c_str());
    
    info[plug_iter->first] = ports;
    
  }
  
  // write the header file
  string header_guard = argv[2];
  for (size_t i = 0; i < header_guard.size(); ++i) {
    char& c = header_guard[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')))
      header_guard[i] = '_';
  }
  ofstream fout(argv[2]);
  fout<<"#ifndef "<<header_guard<<endl
      <<"#define "<<header_guard<<endl<<endl<<endl;
  
  map<string, map<int, PortInfo> >::const_iterator piter;
  for (piter = info.begin(); piter != info.end(); ++piter) {
    fout<<"/* "<<piter->first<<" */"<<endl<<endl;
    
    // write port labels
    fout<<"enum "<<plugins[piter->first]<<"_ports {"<<endl;
    map<int, PortInfo>::const_iterator iter;
    for (iter = piter->second.begin(); iter != piter->second.end(); ++iter)
      fout<<"  "<<plugins[piter->first]<<"_"<<iter->second.name<<","<<endl;
    fout<<"  "<<plugins[piter->first]<<"_n_ports"<<endl
        <<"};"<<endl<<endl;
    
    // write minimum values
    fout<<"static const float "<<plugins[piter->first]<<"_min[] = {"<<endl;
    for (iter = piter->second.begin(); iter != piter->second.end(); ++iter)
      fout<<"  "<<iter->second.min<<","<<endl;
    fout<<"};"<<endl<<endl;

    // write maximum values
    fout<<"static const float "<<plugins[piter->first]<<"_max[] = {"<<endl;
    for (iter = piter->second.begin(); iter != piter->second.end(); ++iter)
      fout<<"  "<<iter->second.max<<","<<endl;
    fout<<"};"<<endl<<endl<<endl;

    // write default values
    fout<<"static const float "<<plugins[piter->first]<<"_default[] = {"<<endl;
    for (iter = piter->second.begin(); iter != piter->second.end(); ++iter)
      fout<<"  "<<iter->second.default_value<<","<<endl;
    fout<<"};"<<endl<<endl<<endl;
  }
  
  fout<<"#endif /* "<<header_guard<<" */"<<endl;
  
  return 0;
}
