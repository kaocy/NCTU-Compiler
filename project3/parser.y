%{
#include <stdio.h>
#include <stdlib.h>

extern int linenum;
extern FILE *yyin;
extern char *yytext;
extern char buf[256];

int yylex();
int yyerror(char *msg);
%}

%token  ID
%token  INT_CONST
%token  FLOAT_CONST
%token  SCIENTIFIC
%token  STR_CONST

%token  LE_OP
%token  NE_OP
%token  GE_OP
%token  EQ_OP
%token  AND_OP
%token  OR_OP

%token  READ
%token  BOOLEAN
%token  WHILE
%token  DO
%token  IF
%token  ELSE
%token  TRUE
%token  FALSE
%token  FOR
%token  INT
%token  PRINT
%token  BOOL
%token  VOID
%token  FLOAT
%token  DOUBLE
%token  STRING
%token  CONTINUE
%token  BREAK
%token  RETURN
%token  CONST

%token  L_PAREN
%token  R_PAREN
%token  COMMA
%token  SEMICOLON
%token  ML_BRACE
%token  MR_BRACE
%token  L_BRACE
%token  R_BRACE
%token  ADD_OP
%token  SUB_OP
%token  MUL_OP
%token  DIV_OP
%token  MOD_OP
%token  ASSIGN_OP
%token  LT_OP
%token  GT_OP
%token  NOT_OP

/*  Program 
    Function 
    Array 
    Const 
    IF 
    ELSE 
    RETURN 
    FOR 
    WHILE
*/
%start program

%%

/********************** Program Units **********************/

program : decl_list funct_def decl_and_def_list
        ;

decl_list : decl_list var_decl
          | decl_list const_decl
          | decl_list funct_decl
          |
          ;

decl_and_def_list : decl_and_def_list var_decl
                  | decl_and_def_list const_decl
                  | decl_and_def_list funct_decl
                  | decl_and_def_list funct_def
                  |
                  ;

funct_decl : scalar_type ID L_PAREN R_PAREN SEMICOLON
           | scalar_type ID L_PAREN parameter_list R_PAREN SEMICOLON
           | VOID ID L_PAREN R_PAREN SEMICOLON
           | VOID ID L_PAREN parameter_list R_PAREN SEMICOLON
           ;

funct_def : scalar_type ID L_PAREN R_PAREN compound_statement
          | scalar_type ID L_PAREN parameter_list R_PAREN  compound_statement
          | VOID ID L_PAREN R_PAREN compound_statement
          | VOID ID L_PAREN parameter_list R_PAREN compound_statement
          ;

parameter_list : parameter_list COMMA scalar_type ID
               | parameter_list COMMA scalar_type array_decl
               | scalar_type array_decl
               | scalar_type ID
               ;

/*************** Data Types and Declarations ***************/

var_decl : scalar_type identifier_list SEMICOLON
         ;

identifier_list : identifier_list COMMA ID
                | identifier_list COMMA ID ASSIGN_OP logical_expression
                | identifier_list COMMA array_decl ASSIGN_OP initial_array
                | identifier_list COMMA array_decl
                | array_decl ASSIGN_OP initial_array
                | array_decl
                | ID ASSIGN_OP logical_expression
                | ID
                ;

array_decl : ID dim
           ;

dim : dim ML_BRACE INT_CONST MR_BRACE
    | ML_BRACE INT_CONST MR_BRACE
    ;

initial_array : L_BRACE literal_list R_BRACE
              ;

literal_list : literal_list COMMA logical_expression
             | logical_expression
             |
             ;

const_decl : CONST scalar_type const_list SEMICOLON;

const_list : const_list COMMA ID ASSIGN_OP literal_const
           | ID ASSIGN_OP literal_const
           ;

/*********************** Statements ***********************/

statement : compound_statement
          | simple_statement
          | conditional_statement
          | while_statement
          | for_statement
          | function_invoke_statement
          | jump_statement
          ;

compound_statement : L_BRACE var_const_stmt_list R_BRACE
                   ;

var_const_stmt_list : var_const_stmt_list statement
                    | var_const_stmt_list var_decl
                    | var_const_stmt_list const_decl
                    |
                    ;

simple_statement : variable_reference ASSIGN_OP logical_expression SEMICOLON
                 | PRINT logical_expression SEMICOLON
                 | READ variable_reference SEMICOLON
                 ;

conditional_statement : IF L_PAREN logical_expression R_PAREN L_BRACE var_const_stmt_list R_BRACE
                      | IF L_PAREN logical_expression R_PAREN
                            L_BRACE var_const_stmt_list R_BRACE
                        ELSE
                            L_BRACE var_const_stmt_list R_BRACE
                      ;

while_statement : WHILE L_PAREN logical_expression R_PAREN
                    L_BRACE var_const_stmt_list R_BRACE
                | DO L_BRACE
                    var_const_stmt_list
                  R_BRACE WHILE L_PAREN logical_expression R_PAREN SEMICOLON
                ;

for_statement : FOR L_PAREN initial_expression_list SEMICOLON control_expression_list SEMICOLON increment_expression_list R_PAREN
                    L_BRACE var_const_stmt_list R_BRACE
              ;

initial_expression_list : initial_expression
                        |
                        ;

initial_expression : initial_expression COMMA variable_reference ASSIGN_OP logical_expression
                   | initial_expression COMMA logical_expression
                   | logical_expression
                   | variable_reference ASSIGN_OP logical_expression

control_expression_list : control_expression
                        |
                        ;

control_expression : control_expression COMMA variable_reference ASSIGN_OP logical_expression
                   | control_expression COMMA logical_expression
                   | logical_expression
                   | variable_reference ASSIGN_OP logical_expression
                   ;

increment_expression_list : increment_expression 
                          |
                          ;

increment_expression : increment_expression COMMA variable_reference ASSIGN_OP logical_expression
                     | increment_expression COMMA logical_expression
                     | logical_expression
                     | variable_reference ASSIGN_OP logical_expression
                     ;

function_invoke_statement : ID L_PAREN logical_expression_list R_PAREN SEMICOLON
                          | ID L_PAREN R_PAREN SEMICOLON
                          ;

jump_statement : CONTINUE SEMICOLON
               | BREAK SEMICOLON
               | RETURN logical_expression SEMICOLON
               ;

/************************ Utilities ************************/

scalar_type : INT
            | DOUBLE
            | STRING
            | BOOL
            | FLOAT
            ;
 
literal_const : INT_CONST
              | FLOAT_CONST
              | SCIENTIFIC
              | STR_CONST
              | TRUE
              | FALSE
              ;

variable_reference : array_list
                   | ID
                   ;

array_list : ID dimension
           ;

dimension : dimension ML_BRACE logical_expression MR_BRACE
          | ML_BRACE logical_expression MR_BRACE
          ;

logical_expression_list : logical_expression_list COMMA logical_expression
                        | logical_expression
                        ;

logical_expression : logical_expression OR_OP logical_term
                   | logical_term
                   ;

logical_term : logical_term AND_OP logical_factor
             | logical_factor
             ;

logical_factor : NOT_OP logical_factor
               | relation_expression
               ;

relation_expression : relation_expression relation_operator arithmetic_expression
                    | arithmetic_expression
                    ;

relation_operator : LT_OP
                  | LE_OP
                  | EQ_OP
                  | GE_OP
                  | GT_OP
                  | NE_OP
                  ;

arithmetic_expression : arithmetic_expression ADD_OP term
                      | arithmetic_expression SUB_OP term
                      | term
                      ;

term : term MUL_OP factor
     | term DIV_OP factor
     | term MOD_OP factor
     | factor
     ;

factor : SUB_OP factor
       | L_PAREN logical_expression R_PAREN
       | literal_const
       | variable_reference     
       | ID L_PAREN logical_expression_list R_PAREN
       | ID L_PAREN R_PAREN
       ;

%%

int yyerror(char *msg) {
    fprintf(stderr, "\n");
    fprintf(stderr, "|--------------------------------------------------------------------------\n");
    fprintf(stderr, "| Error found in Line #%d: %s\n", linenum, buf);
    fprintf(stderr, "|\n" );
    fprintf(stderr, "| Unmatched token: %s\n", yytext);
    fprintf(stderr, "|--------------------------------------------------------------------------\n");
    exit(-1);
    // fprintf(stderr, "%s\t%d\t%s\t%s\n", "Error found in Line ", linenum, "next token: ", yytext);
}
