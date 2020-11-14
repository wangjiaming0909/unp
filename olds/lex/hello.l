%{
#include "y.tab.h"
%}
%%
[\t ]+ /*ignore*/
int {return INT;}
string {return STRING;}
float {return FLOAT;}
char {return CHAR;}
double {return DOUBLE;}
^[a-zA-Z_][a-zA-Z_0-9]+ {return IDENT;}
;
{
  return DELIMITER;
}
%%
