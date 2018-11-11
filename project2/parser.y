%{
#include <stdio.h>
#include <stdlib.h>

extern int linenum;             /* declared in lex.l */
extern FILE *yyin;              /* declared by lex */
extern char *yytext;            /* declared by lex */
extern char buf[256];           /* declared in lex.l */
%}

%token ID
%token READ PRINT
%token BOOL INT FLOAT DOUBLE STRING CONST VOID
%token WHILE DO FOR IF ELSE
%token RETURN BREAK CONTINUE
%token TRUE FALSE
%token INT_CONSTANT FLOAT_CONSTANT STRING_CONSTANT SCIENTIFIC

%right ASSIGN
%left OR
%left AND
%right NOT
%left LT LE EQ NE GT GE
%left '+' '-' 
%left '*' '/' '%'

%%

program : decl_and_def_list
	;

decl_and_def_list	: decl_and_def_list declaration_list
			| decl_and_def_list definition_list
			;

declaration_list : declaration_list const_decl
                 | declaration_list var_decl
                 | declaration_list funct_decl
				 ;

var_decl : type identifier SEMICOLON
         ;

type : INT
     ; 

identifier : ID
	   ;	

%%

int yyerror(char *msg) {
  fprintf(stderr, "\n|--------------------------------------------------------------------------\n");
	fprintf(stderr, "| Error found in Line #%d: %s\n", linenum, buf);
	fprintf(stderr, "|\n");
	fprintf(stderr, "| Unmatched token: %s\n", yytext);
  fprintf(stderr, "|--------------------------------------------------------------------------\n");
  exit(-1);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stdout, "Usage:  ./parser  [filename]\n");
		exit(0);
	}

	FILE *fp = fopen(argv[1], "r");
	
	if(fp == NULL) {
		fprintf(stdout, "Open  file  error\n");
		exit(-1);
	}
	
	yyin = fp;
	yyparse();

	fprintf(stdout, "\n");
	fprintf(stdout, "|--------------------------------|\n");
	fprintf(stdout, "|  There is no syntactic error!  |\n");
	fprintf(stdout, "|--------------------------------|\n");
	exit(0);
}
