%{
#include <stdio.h>
#include <stdlib.h>

extern int linenum;             /* declared in lex.l */
extern FILE *yyin;              /* declared by lex */
extern char *yytext;            /* declared by lex */
extern char buf[256];           /* declared in lex.l */
int yylex();
int yyerror(char *msg);
%}

%token ID
%token READ PRINT
%token BOOL INT FLOAT DOUBLE STRING CONST VOID
%token WHILE DO FOR IF ELSE
%token RETURN BREAK CONTINUE
%token INT_CONSTANT FLOAT_CONSTANT STRING_CONSTANT SCIENTIFIC BOOL_CONSTANT

%right '='
%left OR
%left AND
%right NOT
%left LT LE EQ NE GT GE
%left '+' '-' 
%left '*' '/' '%'

%%

/********************** Program Units **********************/

program : decl_and_def_list
        ;

decl_and_def_list : decl_and_def_list declaration_list
                  | decl_and_def_list definition_list
                  ;

declaration_list : declaration_list const_decl
                 | declaration_list var_decl
                 | declaration_list func_decl
                 ;

definition_list : definition_list func_defi
                ;

func_decl : type identifier '(' argument_list ')' ';'
          | VOID identifier '(' argument_list ')' ';'
          ;

func_defi : type identifier '(' argument_list ')' compound
          | VOID identifier '(' argument_list ')' compound

argument_list : argument MoreArguments
              ;

MoreArguments : ',' argument MoreArguments
              | 
              ;

argument : type identifier
         ;

/*************** Data Types and Declarations ***************/

const_decl : CONST type const_list ';'
           ;

const_list : const_item MoreConst
           ;

MoreConst : ',' const_item MoreConst
          |
          ;

const_item : identifier
           | identifier '=' literal_constant
           ;

var_decl : type var_list ';'
         ;

var_list : var_item MoreVar
         ;

MoreVar : ',' var_item MoreVar
        |
        ;

var_item : identifier
         | identifier '=' expression
         | array_declaration
         | array_declaration '=' initial_array
         ;

/*********************** Statements ***********************/

statement : compound 
          | simple
          | conditional
          | while
          | for
          | jump
          | function_invocation
          ;

compound : '{' compound_body '}'
         ;

compound_body : const_decl compound_body
              | var_decl compound_body
              | statement compound_body
              |
              ;

/* remove PRINT variable_reference due to reduce/reduce conflict */
simple : variable_reference '=' expression ';'
       | PRINT expression ';'
       | READ variable_reference ';'
       ;

expression : expression OR expression
           | expression AND expression
           | NOT expression
           | expression LT expression
           | expression LE expression
           | expression EQ expression
           | expression NE expression
           | expression GT expression
           | expression GE expression
           | expression '+' expression
           | expression '-' expression
           | expression '*' expression
           | expression '/' expression
           | expression '%' expression
           | '-' expression %prec '*'
           | '(' expression ')' %prec '*'
           | literal_constant
           | identifier
           | function_invocation
           | array_reference
           ;

conditional : IF '(' boolean_expression ')' compound ELSE compound 
            | IF '(' boolean_expression ')' compound
            ;

while : WHILE '(' boolean_expression ')' compound
      | DO compound WHILE '(' boolean_expression ')' ';'
      ;

for : FOR '(' initial_expression ';' control_expression ';' increment_expression ')' compound
    ;

jump : RETURN expression ';'
     | BREAK ';'
     | CONTINUE ';'
     ;

/************************ Utilities ************************/

type : BOOL
     | INT
     | FLOAT
     | DOUBLE
     | STRING
     ;

literal_constant : INT_CONSTANT
                 | FLOAT_CONSTANT
                 | STRING_CONSTANT
                 | SCIENTIFIC
                 | BOOL_CONSTANT
                 ;

variable_reference : identifier
                   | array_reference
                   ;

identifier : ID
           ;

array_declaration : identifier array_declaration_dimension

array_declaration_dimension : '[' INT_CONSTANT ']' array_declaration_dimension
                            | '[' INT_CONSTANT ']'
                            ;

array_reference : identifier array_reference_dimension
                ;

array_reference_dimension : '[' expression ']' array_reference_dimension
                          | '[' expression ']'
                          ;

initial_array : '{' expression_list '}'
              ;

function_invocation : identifier '(' expression_list ')'
                    ;

expression_list : expression MoreExpressions
                |
                ;

MoreExpressions : ',' expression MoreExpressions
                | 
                ;

boolean_expression : expression
                   ;

initial_expression : identifier '=' expression
                   | expression
                   ;

control_expression : identifier '=' expression
                   | expression
                   ;

increment_expression : identifier '=' expression
                     | expression
                     ;

%%

int yyerror(char *msg) {
    fprintf(stderr, "\n");
    fprintf(stderr, "|--------------------------------------------------------------------------\n");
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
