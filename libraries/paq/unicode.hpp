#ifndef UNICODE_HPP
#define UNICODE_HPP

#include <iostream>
#include <string>
#include <utility>


#define BOOST_SPIRIT_DEBUG 1


using namespace std;
using namespace boost::spirit;


template <typename ScannerT>
pair<unsigned int, unsigned int> decode_utf8(ScannerT const& scan) {
  
  typedef typename ScannerT::iterator_t iterator_t;
  typedef typename ScannerT::value_t value_t;
  
  if (scan.at_end())
    return make_pair(0, 0);
  
  iterator_t save(scan.first);
  iterator_t iter(scan.first);
  
  value_t ch = *scan.first;
  
  unsigned int size;
  unsigned int codepoint;
  
  // one byte char
  if ((unsigned char)ch < 0x80) {
    size = 1;
    codepoint = ch;
  }
  
  // two byte char
  if ((unsigned char)ch >= 192 && (unsigned char)ch < 224) {
    codepoint = ((unsigned char)ch && 0x1F) << 6;
    ++iter;
    if (iter == scan.last)
      return make_pair(0, 0);
    codepoint += (unsigned char)(*iter) && 0x3F;
    size = 2;
  }
  
  // three byte char
  if ((unsigned char)ch >= 224 && (unsigned char)ch < 240) {
    codepoint = ((unsigned char)ch && 0x0F) << 12;
    ++iter;
    if (iter == scan.last)
      return make_pair(0, 0);
    codepoint += ((unsigned char)(*iter) && 0x3F) << 6;
    ++iter;
    if (iter == scan.last)
      return make_pair(0, 0);
    codepoint += ((unsigned char)(*iter) && 0x3F);
    size = 3;
  }
  
  // four byte char
  if ((unsigned char)ch >= 240 && (unsigned char)ch < 248) {
    codepoint = ((unsigned char)ch && 0x07) << 18;
    ++iter;
    if (iter == scan.last)
      return make_pair(0, 0);
    codepoint += ((unsigned char)(*iter) && 0x3F) << 12;
    ++iter;
    if (iter == scan.last)
      return make_pair(0, 0);
    codepoint += ((unsigned char)(*iter) && 0x3F) << 6;
    ++iter;
    if (iter == scan.last)
      return make_pair(0, 0);
    codepoint += ((unsigned char)(*iter) && 0x3F);
    size = 4;
  }
  
  return make_pair(size, codepoint);
}

                                                     
class uch_p : public parser<uch_p> {
public:
  
  uch_p(unsigned int codepoint = 0) 
    : valid(codepoint) {
    
  }
  
  unsigned int valid;
  
  typedef uch_p self_t;
  
  template <typename ScannerT> struct result {
    typedef typename match_result<ScannerT, unsigned int>::type type;
  };
  
  template <typename ScannerT> typename parser_result<self_t, ScannerT>::type
  parse(ScannerT const& scan) const {
    
    pair<unsigned int, unsigned int> p = decode_utf8(scan);
    
    if (p.first > 0 && p.second == valid) {
      typedef typename ScannerT::iterator_t iterator_t;
      iterator_t save = scan.first;
      for (unsigned int i = 0; i < p.first; ++i)
        ++scan.first;
      return scan.create_match(p.first, p.second, save, scan.first);
    }
    
    return scan.no_match();
  }
  
};


class urange_p : public parser<urange_p> {
public:
  
  urange_p(unsigned int from, unsigned int to) 
    : m_from(from),
      m_to(to) {
    
  }
  
  unsigned int m_from, m_to;
  
  typedef urange_p self_t;
  
  template <typename ScannerT> struct result {
    typedef typename match_result<ScannerT, unsigned int>::type type;
  };

  template <typename ScannerT> typename parser_result<self_t, ScannerT>::type
  parse(ScannerT const& scan) const {
    
    pair<unsigned int, unsigned int> p = decode_utf8(scan);
    
    if (p.first > 0 && p.second >= m_from && p.second <= m_to) {
      typedef typename ScannerT::iterator_t iterator_t;
      iterator_t save = scan.first;
      for (unsigned int i = 0; i < p.first; ++i)
        ++scan.first;
      return scan.create_match(p.first, p.second, save, scan.first);
    }
    
    return scan.no_match();
  }
  
};


#endif
