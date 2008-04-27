/****************************************************************************
    
    regexcompiler.cpp - A compiler for regular expressions over the 
                        alphabet of 4-bit nibbles
    
    Copyright (C) 2008 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
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

#include <cstring>
#include <iostream>

#include "regexcompiler.hpp"


using namespace std;


RegExCompiler::RegExCompiler() {

}
 

bool RegExCompiler::parse(const std::string& regex) {
  bool result = false;
  ActiveProductions* ap = new ActiveProductions[regex.size()];
  if (expression(regex.c_str(), ap) == regex.size()) 
    result = true;
  delete [] ap;
  return result;
}


int RegExCompiler::expression(const char* expr, ActiveProductions* ap) {
  cerr<<__PRETTY_FUNCTION__<<" : "<<expr<<endl;
  int max, l;
  max = nibble(expr, ap);
  l = sub_expression(expr, ap);
  max = l > max ? l : max;
  l = concat_expression(expr, ap);
  max = l > max ? l : max;
  return max;
}


int RegExCompiler::nibble(const char* expr, ActiveProductions* ap) {
  cerr<<__PRETTY_FUNCTION__<<" : "<<expr<<endl;
  if (strlen(expr) == 0)
    return 0;
  
  // base 2 notation
  if (expr[0] == '<' && expr[5] == '>') {
    for (unsigned i = 1; i < 5; ++i) {
      if (!(expr[i] == '0' || expr[i] == '1' || expr[i] == '.'))
	return 0;
    }
    return 6;
  }
  
  // base 16 notation
  if ((expr[0] >= 'a' && expr[0] <= 'f') ||
      (expr[0] >= 'A' && expr[0] <= 'F') || expr[0] == '.')
    return 1;
  
  return 0;
}


int RegExCompiler::sub_expression(const char* expr, ActiveProductions* ap) {
  cerr<<__PRETTY_FUNCTION__<<" : "<<expr<<endl;
  if (expr[0] != '(')
    return 0;
  int l = expression(expr + 1, ap + 1);
  if (l > 0 && expr[1 + l] == ')')
    return l + 2;
  return 0;
}


int RegExCompiler::or_expression(const char* expr, ActiveProductions* ap) {
  cerr<<__PRETTY_FUNCTION__<<" : "<<expr<<endl;
  return false;
}


int RegExCompiler::kleene_expression(const char* expr, ActiveProductions* ap) {
  cerr<<__PRETTY_FUNCTION__<<" : "<<expr<<endl;
  return false;
}


int RegExCompiler::plus_expression(const char* expr, ActiveProductions* ap) {
  cerr<<__PRETTY_FUNCTION__<<" : "<<expr<<endl;
  return false;
}


int RegExCompiler::concat_expression(const char* expr, ActiveProductions* ap) {
  cerr<<__PRETTY_FUNCTION__<<" : "<<expr<<endl;
  if (ap[0].Concat)
    return 0;
  ap[0].Concat = true;
  int l = expression(expr, ap);
  if (l > 0) {
    int m = expression(expr + l, ap + l);
    if (m > 0)
      return m + l;
  }
  return 0;
}


