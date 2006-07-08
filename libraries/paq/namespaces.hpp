#ifndef NAMESPACES_HPP
#define NAMESPACES_HPP

#include <string>


namespace PAQ {


  std::string rdf(const std::string& str) {
    return std::string("<http://www.w3.org/1999/02/22-rdf-syntax-ns#") + 
      str + '>';
  }


  std::string rdfs(const std::string& str) {
    return std::string("<http://www.w3.org/2000/01/rdf-schema#") + 
      str + '>';
  }

  std::string doap(const std::string& str) {
    return std::string("<http://usefulinc.com/ns/doap#") + str + '>';
  }


  std::string foaf(const std::string& str) {
    return std::string("<http://xmlns.com/foaf/0.1/") + str + '>';
  }


}


#endif
