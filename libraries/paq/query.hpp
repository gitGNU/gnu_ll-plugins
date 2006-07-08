#ifndef QUERY_HPP
#define QUERY_HPP

#include <map>
#include <string>
#include <vector>

#include "rdf.hpp"


namespace PAQ {

  class Pattern;
  
  
  class QueryResult {
  public:
    
    RDFTerm* operator[](size_t var);
    RDFTerm* operator[](Variable& var);
    
  protected:
    
    friend class QueryEngine;
    
    std::map<Variable*, size_t> m_variables;
    std::vector<RDFTerm*> m_data;
  };
  
  
  class Query;
  class QueryEngine;
  
  
  class Filter {
  public:
    virtual ~Filter() { }
    virtual void set_variable_indices(Query& query) = 0;
    virtual bool execute(QueryEngine& engine) = 0;
  };
  
  
  class LangMatches : public Filter {
  public:
    LangMatches(Variable& var, const std::string& lang);
    void set_variable_indices(Query& query);
    bool execute(QueryEngine& engine);
  protected:
    Variable* m_var;
    size_t m_index;
    std::string m_lang;
  };
  Filter* lang_matches(Variable& var, const std::string& lang);
  

  class TermEquality : public Filter {
  public:
    TermEquality(Variable& var1, Variable& var2);
    void set_variable_indices(Query& query);
    bool execute(QueryEngine& engine);
  protected:
    Variable* m_var1;
    Variable* m_var2;
    size_t m_index1;
    size_t m_index2;
  };
  Filter* operator==(Variable& var1, Variable& var2);
  
  
  class UnaryFilter : public Filter {
  public:
    UnaryFilter(Variable& var);
    virtual void set_variable_indices(Query& query);
  protected:
    Variable* m_var;
    size_t m_index;
  };
  
  
  class IsURI : public UnaryFilter {
  public:
    IsURI(Variable& var) : UnaryFilter(var) { }
    bool execute(QueryEngine& engine);
  };
  Filter* is_uriref(Variable& var);
  
  
  class IsLiteral : public UnaryFilter {
  public:
    IsLiteral(Variable& var) : UnaryFilter(var) { }
    bool execute(QueryEngine& engine);
  };
  Filter* is_literal(Variable& var);
  
  
  class IsNumeric : public UnaryFilter {
  public:
    IsNumeric(Variable& var) : UnaryFilter(var) { }
    bool execute(QueryEngine& engine);
  };
  Filter* is_numeric(Variable& var);
  
  
  class IsInteger : public UnaryFilter {
  public:
    IsInteger(Variable& var) : UnaryFilter(var) { }
    bool execute(QueryEngine& engine);
  };
  Filter* is_integer(Variable& var);
  
  
  class Negation : public Filter {
  public:
    Negation(Filter* f);
    ~Negation();
    void set_variable_indices(Query& q);
    bool execute(QueryEngine& engine);
  protected:
    Filter* m_filter;
  };
  Filter* no(Filter* filter);
  
  
  class NumericCompare : public Filter {
  public:
    NumericCompare(Variable& var, double value);
    virtual void set_variable_indices(Query& q);
  protected:
    Variable* m_var;
    double m_value;
    size_t m_index;
  };
  
  
  class NumericLess : public NumericCompare {
  public:
    NumericLess(Variable& var, double value) : NumericCompare(var, value) { }
    bool execute(QueryEngine& engine);
  };
  Filter* operator<(Variable& var, double value);
  Filter* operator>=(double value, Variable& var);
  
  
  class NumericLessEqual : public NumericCompare {
  public:
    NumericLessEqual(Variable& var, double value) : NumericCompare(var, value) {}
    bool execute(QueryEngine& engine);
  };
  Filter* operator<=(Variable& var, double value);
  Filter* operator>(double value, Variable& var);
  
  
  class NumericGreater : public NumericCompare {
  public:
    NumericGreater(Variable& var, double value) : NumericCompare(var, value) { }
    bool execute(QueryEngine& engine);
  };
  Filter* operator>(Variable& var, double value);
  Filter* operator<=(double value, Variable& var);
  

  class NumericGreaterEqual : public NumericCompare {
  public:
    NumericGreaterEqual(Variable& var, double value) 
      : NumericCompare(var, value) { }
    bool execute(QueryEngine& engine);
  };
  Filter* operator>=(Variable& var, double value);
  Filter* operator<(double value, Variable& var);
  
  
  class NumericEquality : public NumericCompare {
  public:
    NumericEquality(Variable& var, double value) : NumericCompare(var, value) { }
    bool execute(QueryEngine& engine);
  };
  Filter* operator==(Variable& var, double value);
  Filter* operator==(double value, Variable& var);
  Filter* operator!=(Variable& var, double value);
  Filter* operator!=(double value, Variable& var);
  
  
  class BinaryFilter : public Filter {
  public:
    BinaryFilter(Variable& var1, Variable& var2);
    virtual void set_variable_indices(Query& query);
  protected:
    Variable* m_var1;
    Variable* m_var2;
    size_t m_index1;
    size_t m_index2;
  };
  
  
  class Query {
  public:
    
    Query();
    
    Query& where(Variable& s, Variable& p, Variable& o);
    Query& where(const std::string& s, Variable& p, Variable& o);
    Query& where(Variable& s, const std::string& p, Variable& o);
    Query& where(Variable& s, Variable& p, const std::string& o);
    Query& where(Variable& s, const std::string& p, const std::string& o);
    Query& where(const std::string& s, Variable& p, const std::string& o);
    Query& where(const std::string& s, const std::string& p, Variable& o);
    Query& where(const std::string& s, const std::string& p,
                 const std::string& o);
    Query& filter(Filter* f); 
    
    std::vector<QueryResult> run(RDFData& data);

    RDFData data;
    
  protected:
    
    friend class QueryEngine;
    
    friend Query select(Variable& var1, Variable& var2, 
                        Variable& var3, Variable& var4);
    
    Query(Variable& var1, Variable& var2, Variable& var3, Variable& var4);
    RDFTerm* add_term(const std::string& str);
    
    std::vector<Filter*> filters;
    
    std::map<Variable*, size_t> selected;
  };
  
  
  Query select(Variable& var1, Variable& var2 = Variable::nil, 
               Variable& var3 = Variable::nil, Variable& var4 = Variable::nil);
  
  
  class QueryEngine {
  public:
    
    QueryEngine(Query& q, RDFData& d);
    
    std::vector<QueryResult> run();
    
    std::vector<size_t> mapping;
    const std::vector<RDFTerm*>& N1;
    const std::vector<RDFTerm*>& N2;
    const std::vector<Triple*>& T1;
    const std::vector<Triple*>& T2;
    
  protected:
    
    const Query& query;
    
    void match(size_t depth);
    bool match_node(RDFTerm* p, RDFTerm* t, size_t& bound);
    
    size_t NULL_NODE;

    std::vector<QueryResult> result;
    
  };
  
}


#endif
