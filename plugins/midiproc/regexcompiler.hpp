/****************************************************************************
    
    regexcompiler.hpp - A compiler for regular expressions over the 
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

#ifndef REGEXCOMPILER_HPP
#define REGEXCOMPILER_HPP

#include <string>


/*

Production rules:

Z -> E  
E -> n | S | O | K | P | C
S -> (E)
O -> E|E
C -> EE
K -> E*
P -> E+


FOLLOW sets:

FOLLOW(E) = { $, ), |, n, (, *, + }
FOLLOW(S) = { $, ), |, n, (, *, + }
FOLLOW(O) = { $, ), |, n, (, *, + }
FOLLOW(C) = { $, ), |, n, (, *, + }
FOLLOW(K) = { $, ), |, n, (, *, + }
FOLLOW(P) = { $, ), |, n, (, *, + }


Items:

[Z -> .E], [Z -> E.], 
[E -> .n], [E -> n.], [E -> .S], [E -> S.], [E -> .O], [E -> O.], 
[E -> .K], [E -> K.], [E -> .P], [E -> P.], [E -> .C], [E -> C.],
[S -> .(E)], [S -> (.E)], [S -> (E.)], [S -> (E).], 
[O -> .E|E], [O -> E.|E], [O -> E|.E], [O -> E|E.],
[C -> .EE], [C -> E.E], [C -> EE.],
[K -> .E*], [K -> E.*], [K -> E*.],
[P -> .E+], [P -> E.+], [P -> E+.]

States:

0: V[] = { [Z -> .E], [E -> .n], [E -> .S], [E -> .O], [E -> .K], [E -> .P], 
           [E -> .C], [S -> .(E)], [O -> .E|E], [C -> .EE], [K -> .E*], 
           [P -> .E+] }

1: V[E] = { [Z -> E.], [O -> E.|E], [C -> E.E], [K -> E.*], [P -> E.+], 
            [E -> .n], [E -> .S], [E -> .0], [E -> .K], [E -> .P],
            [E -> .C], [S -> .(E)], [O -> .E|E], [K -> .E*], [P -> .E+] }

2: V[n] = { [E -> n.] }

3: V[S] = { [E -> S.] }

4: V[O] = { [E -> O.] }

5: V[K] = { [E -> K.] }

6: V[P] = { [E -> P.] }

7: V[C] = { [E -> C.] }

8: V[(] = { [S -> (.E), [E -> .n], [E -> .S], [E -> .O], [E -> .K], [E -> .P]
            [E -> .C], [S -> .(E)], [O -> .E|E], [K -> .E*], [P -> .E+], 
            [C -> .EE] }

9: V[E|] = { [O -> E|.E], [E -> .n], [E -> .S], [E -> .O], [E -> .K], [E -> .P]
             [E -> .C], [S -> .(E)], [O -> .E|E], [K -> .E*], [P -> .E+], 
             [C -> .EE] }

10: V[EE] = { [C -> EE.], [O -> E.|E], [K -> E.*], [P -> E.+] }

11: V[E*] = { [K -> E*.] }

12: V[E+] = { [P -> E+.] }

13: V[(E] = { [S -> (E.)], [O -> E.|E], [K -> E.*], [P -> E.+], [C -> E.E],
	      [E -> .n], [E -> .S], [E -> .O], [E -> .K], [E -> .P]
              [E -> .C], [S -> .(E)], [O -> .E|E], [K -> .E*], [P -> .E+], 
              [C -> .EE] }

14: V[E|E] = { [O -> E|E.], [O -> E.|E], [K -> E.*], [P -> E.+], [C -> E.E],
               [E -> .n], [E -> .S], [E -> .O], [E -> .K], [E -> .P]
               [E -> .C], [S -> .(E)], [O -> .E|E], [K -> .E*], [P -> .E+], 
               [C -> .EE] }

15: V[(E)] = { [S -> (E).] }


Table:

STATE | n | ( | ) | | | * | + | $ || E | S | O | K | P | C |
============================================================
    0 | 2 | 8 |   |   |   |   |   || 1 | 3 | 4 | 5 | 6 | 7 |
    1 | 2 | 8 |   | 9 | 11| 12|   || 10| 3 | 4 | 5 | 6 | 7 |
    2 |   |   |   |   |   |   |   ||   |   |   |   |   |   |
    3 |   |   |   |   |   |   |   ||   |   |   |   |   |   |
    4 |   |   |   |   |   |   |   ||   |   |   |   |   |   |
    5 |   |   |   |   |   |   |   ||   |   |   |   |   |   |
    6 |   |   |   |   |   |   |   ||   |   |   |   |   |   |
    7 |   |   |   |   |   |   |   ||   |   |   |   |   |   |
    8 | 2 | 8 |   |   |   |   |   || 13| 3 | 4 | 5 | 6 | 7 |
    9 | 2 | 8 |   |   |   |   |   || 14| 3 | 4 | 5 | 6 | 7 |
   10 |   |   |   | 9 | 11| 12|   ||   |   |   |   |   |   |
   11 |   |   |   |   |   |   |   ||   |   |   |   |   |   |
   12 |   |   |   |   |   |   |   ||   |   |   |   |   |   |
   13 | 2 |   | 15| 9 | 11| 12|   || 1 | 3 | 4 | 5 | 6 | 7 |
   14 | 2 | 8 |   | 9 | 11| 12|   || 1 | 3 | 4 | 5 | 6 | 7 |
   15 |   |   |   |   |   |   |   ||   |   |   |   |   |   |


*/

class RegExCompiler {
public:
  
  RegExCompiler();
  
  bool parse(const std::string& regex);

protected:
  
  struct ActiveProductions {
    ActiveProductions() 
      : Or(false), Kleene(false), Plus(false), Concat(false) { }
    bool Or;
    bool Kleene;
    bool Plus;
    bool Concat;
  };
  
  int expression(const char* expr, ActiveProductions* ap);
  int nibble(const char* expr, ActiveProductions* ap);
  int sub_expression(const char* expr, ActiveProductions* ap);
  int or_expression(const char* expr, ActiveProductions* ap);
  int kleene_expression(const char* expr, ActiveProductions* ap);
  int plus_expression(const char* expr, ActiveProductions* ap);
  int concat_expression(const char* expr, ActiveProductions* ap);
  
};


#endif
