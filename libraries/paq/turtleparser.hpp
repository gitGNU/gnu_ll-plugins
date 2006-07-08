#ifndef TURTLEPARSER_HPP
#define TURTLEPARSER_HPP

#include <boost/spirit/core.hpp>
#include <boost/spirit/utility.hpp>
#include <boost/spirit/tree/parse_tree.hpp>
#include <boost/spirit/tree/ast.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <utility>

#include "unicode.hpp"
#include "rdf.hpp"


namespace PAQ {
  
  using namespace std;
  using namespace boost;
  using namespace boost::spirit;
  
  
  class TurtleParser {
  public:
    
    TurtleParser();
    
    bool parse_ttl(const std::string&, RDFData& data);
    
  protected:
    
    typedef const char* iterator_t;
    typedef tree_match<iterator_t> parse_tree_match_t;
    typedef parse_tree_match_t::const_tree_iterator iter_t;
    typedef pt_match_policy<iterator_t> match_policy_t;
    typedef scanner_policies<iteration_policy, match_policy_t, action_policy> 
    scanner_policy_t;
    typedef scanner<iterator_t, scanner_policy_t> scanner_t;
    typedef rule<scanner_t> rule_t;
    
    void do_directive(iter_t iter);  
    void do_statement(iter_t iter);
    void do_triples(iter_t iter);
    RDFTerm* do_subject(iter_t iter);
    RDFTerm* do_resource(iter_t iter);
    RDFTerm* do_blank(iter_t iter);
    RDFTerm* do_uriref(iter_t iter);
    RDFTerm* do_qname(iter_t iter);
    RDFTerm* do_nodeID(iter_t iter);
    multimap<RDFTerm*, RDFTerm*> do_predicateObjectList(iter_t iter);
    RDFTerm* do_collection(iter_t iter);
    RDFTerm* do_verb(iter_t iter);
    vector<RDFTerm*> do_objectList(iter_t iter);
    RDFTerm* do_predicate(iter_t iter);
    RDFTerm* do_object(iter_t iter);
    RDFTerm* do_literal(iter_t iter);
    vector<RDFTerm*> do_itemList(iter_t iter);
    RDFTerm* do_quotedString(iter_t iter);
    RDFTerm* do_string(iter_t iter);
    RDFTerm* do_longString(iter_t iter);
    RDFTerm* do_double(iter_t iter);
    RDFTerm* do_decimal(iter_t iter);
    RDFTerm* do_integer(iter_t iter);
    RDFTerm* do_boolean(iter_t iter);
    RDFTerm* do_datatypeString(iter_t iter);
    
    string node_to_string(iter_t node);  
    void ucharacter_escape(string& str);
    void scharacter_escape(string& str);
    
    rule_t turtleDoc,
      statement,
      directive,
      triples,
      predicateObjectList,
      objectList,
      verb,
      comment,
      subject,
      predicate,
      object,
      literal,
      datatypeString,
      integer,
      _double,
      decimal,
      exponent,
      boolean,
      blank,
      itemList,
      collection,
      _ws,
      resource,
      nodeID,
      qname,
      uriref,
      language,
      nameStartChar,
      nameChar,
      name,
      prefixName,
      relativeURI,
      quotedString,
      _string,
      longString,
      character,
      echaracter,
      _hex,
      ucharacter,
      scharacter,
      lcharacter;
    
    std::map<parser_id, string> m_rule_map;
    std::map<std::string, std::string> m_prefix_map;
    RDFData* m_data;
  };
  
}


#endif
