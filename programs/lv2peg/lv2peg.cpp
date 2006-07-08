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


int main(int argc, char** argv) {
  
  if (argc < 6) {
    cerr<<"Use like this: lv2peg <input file> <output file> "
        <<"<plugin uri> <enum name> <enum guard> [<prefix>]"<<endl;
  }
  string prefix;
  if (argc > 6)
    prefix = argv[6];
  
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
  
  // query the ports
  Namespace lv2("<http://lv2plug.in/ontology#>");
  Variable plugin, port, index, symbol;
  Query q = 
    select(index, symbol)
    .where(argv[3], rdf("type"), lv2("Plugin"))
    .where(argv[3], lv2("port"), port)
    .where(port, lv2("index"), index)
    .where(port, lv2("symbol"), symbol);
  
  // put ports in a map so they get sorted
  vector<QueryResult> qr = q.run(data);
  map<int, string> ports;
  for (size_t i = 0; i < qr.size(); ++i) {
    int port_index = atoi(qr[i][index]->name.c_str());
    ports[port_index] = qr[i][symbol]->name;
  }
  
  // check that the port indices are OK
  map<int, string>::const_iterator iter;
  int next = 0;
  for (iter = ports.begin(); iter != ports.end(); ++iter) {
    if (iter->first != next) {
      cerr<<"There was no description of port "<<next<<"."<<endl;
      return -1;
    }
    ++next;
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
      <<"#define "<<header_guard<<endl<<endl
      <<"enum "<<argv[4]<<" {"<<endl;
  for (iter = ports.begin(); iter != ports.end(); ++iter)
    fout<<"  "<<prefix<<iter->second<<","<<endl;
  fout<<"  "<<argv[5]<<endl
      <<"};"<<endl<<endl
      <<"#endif "<<header_guard<<endl;
  
  return 0;
}
