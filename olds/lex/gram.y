
%{
#include <stdio.h>
%}
%token INT DOUBLE CHAR STRING FLOAT IDENT DELIMITER
%%
//grams
define:
  type IDENT DELIMITER
  {
    printf("declare a variable: %s\n", $2);
  }
type:
  INT |
  DOUBLE |
  CHAR |
  STRING |
  FLOAT
  {
    return $1;
  };
%%
extern FILE* yyin;

main()
{
  do {
    yyparse();
  } while (!feof(yyin));
}
yyerror(s)
char *s;
{
  fprintf(stderr, "%s\n", s);
}

