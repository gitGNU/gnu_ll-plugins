#include <iostream>

#include "regexcompiler.hpp"


using namespace std;


int main(int argc, char** argv) {
  RegExCompiler comp;
  if (comp.parse(argv[1])) {
    cout<<"Yes, that is a regular expression!"<<endl;
    return 0;
  }
  else
    cout<<"No, that is not a regular expression!"<<endl;
  return -1;
}
