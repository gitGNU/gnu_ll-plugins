%{
#include <stdlib.h>
#include "regex.tab.h"
%}

 
%%


"+"                          { return PLUS;    }
"|"                          { return OR;      }
"*"                          { return KLEENE;  }
"("                          { return LPAREN;  }
")"                          { return RPAREN;  }
[a-fA-F0-9\.]                { if (yytext[0] == '.') {
                                 yylval.masked_value = 0;
                               }
                               else {
				 yylval.masked_value = strtol(yytext, NULL, 16);
				 yylval.masked_value |= 0xF0;
			       }
                               return NIBBLE;  }
\<[01\.][01\.][01\.][01\.]\> { int i;
                               yylval.masked_value = 0;
                               for (i = 1; i < 5; ++i) {
                                 if (yytext[i] == '1')
				   yylval.masked_value |= 1 << (4 - i);
                                 if (yytext[i] != '.')
				   yylval.masked_value |= 1 << (4 + 4 - i);
                               }
			       return NIBBLE;  }
.                            { return UNKNOWN; }
%%
 
