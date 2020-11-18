%{
#include <stdio.h>
%}
%token INT DOUBLE CHAR STRING FLOAT IDENT DELIMITER
%%
//grams
define: type IDENT DELIMITER { printf("declare a variable: %s\n", "sd"); }
      ;
type:
  INT |
  DOUBLE |
  CHAR |
  STRING |
  FLOAT
  {
    printf("declare a type\n");
  };
%%
extern FILE* yyin;

int main()
{
  do {
    yyparse();
  } while (!feof(yyin));
}
int yyerror(char* s)
{
  fprintf(stderr, "ooo%s\n", s);
}

