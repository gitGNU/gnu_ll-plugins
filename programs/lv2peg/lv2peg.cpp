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
  PortInfo() 
    : min(-FLT_MAX), 
      max(FLT_MAX), 
      default_value(min),
      toggled(false),
      integer(false),
      logarithmic(false) { }
  string name;
  float min;
  float max;
  float default_value;
  bool toggled;
  bool integer;
  bool logarithmic;
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
  if (!fin.good()) {
    cerr<<"Could not open "<<argv[1]<<"."<<endl;
    return -1;
  }
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
    
    // get port hints
    Variable hint;
    qr = select(index, hint)
      .where(plug_iter->first, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("portHint"), hint)
      .run(data);
    for (int i = 0; i < qr.size(); ++i) {
      if (qr[i][hint]->name == lv2("toggled"))
        ports[atoi(qr[i][index]->name.c_str())].toggled = true;
      if (qr[i][hint]->name == lv2("integer"))
        ports[atoi(qr[i][index]->name.c_str())].integer = true;
      if (qr[i][hint]->name == lv2("logarithmic"))
        ports[atoi(qr[i][index]->name.c_str())].logarithmic = true;
    }
    
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
  
  // define data structure
  fout<<"#ifndef PEG_STRUCT"<<endl
      <<"#define PEG_STRUCT"<<endl
      <<"typedef struct {"<<endl
      <<"  float min;"<<endl
      <<"  float max;"<<endl
      <<"  float default_value;"<<endl
      <<"  char toggled;"<<endl
      <<"  char integer;"<<endl
      <<"  char logarithmic;"<<endl
      <<"} peg_data_t;"<<endl
      <<"#endif"<<endl<<endl;
    
  map<string, map<int, PortInfo> >::const_iterator piter;
  for (piter = info.begin(); piter != info.end(); ++piter) {
    fout<<"/* "<<piter->first<<" */"<<endl<<endl;
    
    // write port labels
    fout<<"enum "<<plugins[piter->first]<<"_port_enum {"<<endl;
    map<int, PortInfo>::const_iterator iter;
    for (iter = piter->second.begin(); iter != piter->second.end(); ++iter)
      fout<<"  "<<plugins[piter->first]<<"_"<<iter->second.name<<","<<endl;
    fout<<"  "<<plugins[piter->first]<<"_n_ports"<<endl
        <<"};"<<endl<<endl;
    
    // write port info
    fout<<"static const peg_data_t "
        <<plugins[piter->first]<<"_ports[] = {"<<endl;
    for (iter = piter->second.begin(); iter != piter->second.end(); ++iter) {
      fout<<"  { "
          <<iter->second.min<<", "
          <<iter->second.max<<", "
          <<iter->second.default_value<<", "
          <<(iter->second.toggled ? "1" : "0")<<", "
          <<(iter->second.integer ? "1" : "0")<<", "
          <<(iter->second.logarithmic ? "1" : "0")<<" }, "<<endl;
    }
    fout<<"};"<<endl<<endl<<endl;
    
  }
  
  fout<<"#endif /* "<<header_guard<<" */"<<endl;
  
  return 0;
}
