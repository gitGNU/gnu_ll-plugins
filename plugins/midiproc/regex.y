%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void yyerror(const char *str) {
  fprintf(stderr,"error: %s\n",str);
}
 
int yywrap() {
  return 1;
}
  
enum NodeType {
  NibbleNode,
  SubNode,
  OrNode,
  ConcatNode,
  KleeneNode,
  PlusNode,
};


struct Node {
  enum NodeType type;
  struct Node* n1;
  struct Node* n2;
  unsigned char value;
  unsigned char mask;
};


struct Node* node_new(enum NodeType type, 
		      struct Node* n1, struct Node* n2, int nibble_val) {
  struct Node* n = malloc(sizeof(struct Node));
  n->type = type;
  n->n1 = n1;
  n->n2 = n2;
  n->value = nibble_val & 0x0F;
  n->mask = (nibble_val & 0xF0) >> 4;
  return n;
}


 void node_print(struct Node* node) {
  switch (node->type) {
  case NibbleNode: 
    printf("Nibble [ %X %X ]", node->value, node->mask);
    break;
  case SubNode: 
    printf("Sub [ ");
    node_print(node->n1);
    printf(" ]");
    break;
  case OrNode:
    printf("Or [ ");
    node_print(node->n1);
    printf(" ");
    node_print(node->n2);
    printf(" ]");
    break;
  case ConcatNode:
    printf("Concat [ ");
    node_print(node->n1);
    printf(" ");
    node_print(node->n2);
    printf(" ]");
    break;
  case KleeneNode:
    printf("Kleene [ ");
    node_print(node->n1);
    printf(" ]");
    break;
  case PlusNode:
    printf("Plus [ ");
    node_print(node->n1);
    printf(" ]");
    break;
  }
}


struct Node* top_semval = 0;

 
main() {
  yyparse();
  node_print(top_semval);
  printf("\n");
}



%}


%union {
  int masked_value;
  struct Node* node;
}


%expect 8


%start top


/* CONCAT isn't an actual token, it's just used for %prec */
%token PLUS OR KLEENE LPAREN RPAREN NIBBLE UNKNOWN CONCAT


%type<node> top regex sub_regex or_regex concat_regex kleene_regex plus_regex
%type<masked_value> NIBBLE


/* This is needed to give CONCAT reduction precedence over OR shift,
   to make sure that 'ab|c' is parsed as '(ab)|c' and not 'a(b|c)' */
%left OR
%left CONCAT

%%


top: regex { $$ = $1; top_semval = $1; } ;

regex: NIBBLE { $$ = node_new(NibbleNode, NULL, NULL, $1); }
     | sub_regex { $$ = $1; }
     | or_regex { $$ = $1; }
     | concat_regex { $$ = $1; }
     | kleene_regex { $$ = $1; }
     | plus_regex { $$ = $1; }
     ;

sub_regex: LPAREN regex RPAREN { $$ = node_new(SubNode, $2, NULL, 0); }
	 ;

or_regex: regex OR regex { $$ = node_new(OrNode, $1, $3, 0); }
	;

concat_regex: regex regex %prec CONCAT { $$ = node_new(ConcatNode, $1, $2, 0); }
	    ;

kleene_regex: regex KLEENE { $$ = node_new(KleeneNode, $1, NULL, 0); }
	    ;

plus_regex: regex PLUS { $$ = node_new(PlusNode, $1, NULL, 0); }
	  ;
