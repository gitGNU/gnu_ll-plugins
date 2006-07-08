#ifndef RDF_HPP
#define RDF_HPP

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>


namespace PAQ {


  struct Triple;

  struct RDFTerm;

  struct Variable {
    static Variable nil;
  };


  class Namespace {
  public:

    Namespace(const std::string& str);
  
    std::string operator()(const std::string& str);

  protected:
    std::string m_prefix;
  };
  
  

  struct RDFTermCompare {
    bool operator()(const RDFTerm* a, const RDFTerm* b);
  };


  struct RDFTerm {
    enum Type {
      URIRef,
      Literal,
      BNode,
      Variable
    };
  
    RDFTerm(Type t, const std::string& n = "", RDFTerm* d = 0);
    virtual ~RDFTerm();
    
    double get_numeric_value() const;
    
    Type type;
    std::string name;
    RDFTerm* datatype;
    std::string lang;
    
    typedef std::map<RDFTerm*, std::set<RDFTerm*> > tmap;
  
    tmap s_triples_;
    tmap o_triples_;
  
    size_t index;
  };


  struct RDFVariable : public RDFTerm {
    RDFVariable();
  };


  struct Triple {
    Triple(RDFTerm* s, RDFTerm* p, RDFTerm* o);
    RDFTerm* subject;
    RDFTerm* predicate;
    RDFTerm* object;
  };


  struct RDFData {
  
    RDFData();
  
    RDFTerm* add_uriref(const std::string& name);
    RDFTerm* add_bnode(const std::string& name = "");
    RDFTerm* add_literal(const std::string& name, RDFTerm* datatype = 0);
    RDFTerm* add_variable(PAQ::Variable* var);
    void add_triple(RDFTerm* subject, RDFTerm* predicate, RDFTerm* object);
  
    const std::vector<RDFTerm*>& get_terms() const;
  
    std::map<std::string, RDFTerm*> urirefs;
    std::map<std::string, RDFTerm*> literals;
    std::map<std::string, RDFTerm*> bnodes;
    std::map<PAQ::Variable*, RDFTerm*> variables;
  
    std::vector<RDFTerm*> terms;
    std::vector<Triple*> triples;
  
    unsigned long bnode_counter;
  };


}


#endif
