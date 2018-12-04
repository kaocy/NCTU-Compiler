%{
#include <stdio.h>
#include <stdlib.h>
#include "symbolTable.h"

// from scanner.l
extern int linenum;
extern FILE *yyin;
extern char *yytext;
extern char buf[256];

int yylex();
int yyerror(char *msg);

SymbolTable *table;
IdList *idlist_head, *idlist_tail;
ArraySignature *arraylist_head, *arraylist_tail;
%}

%union {
    int inum;
    double dnum;
    char* str;
    struct Type* type;
    struct Value* value;
    struct Attribute* attribute;
}

%token <str> ID
%token <inum> INT_CONST
%token <dnum> FLOAT_CONST
%token <str> SCIENTIFIC
%token <str> STR_CONST

%token <str> LE_OP
%token <str> NE_OP
%token <str> GE_OP
%token <str> EQ_OP
%token <str> AND_OP
%token <str> OR_OP

%token <str> READ
%token <str> BOOLEAN
%token <str> WHILE
%token <str> DO
%token <str> IF
%token <str> ELSE
%token <str> TRUE
%token <str> FALSE
%token <str> FOR
%token <str> INT
%token <str> PRINT
%token <str> BOOL
%token <str> VOID
%token <str> FLOAT
%token <str> DOUBLE
%token <str> STRING
%token <str> CONTINUE
%token <str> BREAK
%token <str> RETURN
%token <str> CONST

%token <str> L_PAREN
%token <str> R_PAREN
%token <str> COMMA
%token <str> SEMICOLON
%token <str> ML_BRACE
%token <str> MR_BRACE
%token <str> L_BRACE
%token <str> R_BRACE
%token <str> ADD_OP
%token <str> SUB_OP
%token <str> MUL_OP
%token <str> DIV_OP
%token <str> MOD_OP
%token <str> ASSIGN_OP
%token <str> LT_OP
%token <str> GT_OP
%token <str> NOT_OP

%type <type> scalar_type
%type <value> literal_const
%type <str> array_decl

%start program

%%

/********************** Program Units **********************/

program : decl_list funct_def decl_and_def_list { 
              PrintTable(table);
              DeleteTable(table);
              table = NULL; 
          }
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

funct_def : scalar_type ID L_PAREN R_PAREN L_BRACE {
                table = InsertTable(table, table->level + 1);
            }
            var_const_stmt_list R_BRACE {
                PrintTable(table);
                table = table->prev;
                DeleteTable(table->next);
                table->next = NULL;
            }
          | scalar_type ID L_PAREN parameter_list R_PAREN L_BRACE {
                table = InsertTable(table, table->level + 1);
            }
            var_const_stmt_list R_BRACE {
                PrintTable(table);
                table = table->prev;
                DeleteTable(table->next);
                table->next = NULL;
            }
          | VOID ID L_PAREN R_PAREN L_BRACE {
                table = InsertTable(table, table->level + 1);
            }
            var_const_stmt_list R_BRACE {
                PrintTable(table);
                table = table->prev;
                DeleteTable(table->next);
                table->next = NULL;
            }
          | VOID ID L_PAREN parameter_list R_PAREN L_BRACE {
                table = InsertTable(table, table->level + 1);
            }
            var_const_stmt_list R_BRACE {
                PrintTable(table);
                table = table->prev;
                DeleteTable(table->next);
                table->next = NULL;
            }
          ;

parameter_list : parameter_list COMMA scalar_type ID
               | parameter_list COMMA scalar_type array_decl
               | scalar_type array_decl
               | scalar_type ID
               ;

/*************** Data Types and Declarations ***************/

var_decl : scalar_type identifier_list SEMICOLON {
               table->entry_tail = InsertEntryFromId(table, idlist_head, "variable", $1);
               DeleteType($1);
               DeleteIdList(idlist_head);
               idlist_head = idlist_tail = NULL;
           }
         ;

identifier_list : identifier_list COMMA ID {
                      idlist_tail = InsertIdList(idlist_tail, $3, NULL, NULL);
                  }
                | identifier_list COMMA ID ASSIGN_OP logical_expression {
                      idlist_tail = InsertIdList(idlist_tail, $3, NULL, NULL);
                  }
                | identifier_list COMMA array_decl ASSIGN_OP initial_array {
                      idlist_tail = InsertIdList(idlist_tail, $3, arraylist_head, NULL);
                      arraylist_head = arraylist_tail = NULL;
                  }
                | identifier_list COMMA array_decl {
                      idlist_tail = InsertIdList(idlist_tail, $3, arraylist_head, NULL);
                      arraylist_head = arraylist_tail = NULL;
                  }
                | ID ASSIGN_OP logical_expression {
                      idlist_head = idlist_tail = InsertIdList(idlist_tail, $1, NULL, NULL);
                  }
                | ID {
                      idlist_head = idlist_tail = InsertIdList(idlist_tail, $1, NULL, NULL);
                  }
                | array_decl ASSIGN_OP initial_array {
                      idlist_head = idlist_tail = InsertIdList(idlist_tail, $1, arraylist_head, NULL);
                      arraylist_head = arraylist_tail = NULL;
                  }
                | array_decl {
                      idlist_head = idlist_tail = InsertIdList(idlist_tail, $1, arraylist_head, NULL);
                      arraylist_head = arraylist_tail = NULL;
                  }
                ;

array_decl : ID dim { $$ = $1; }
           ;

dim : dim ML_BRACE INT_CONST MR_BRACE {
          arraylist_tail = InsertArraySignature(arraylist_tail, $3);
      }
    | ML_BRACE INT_CONST MR_BRACE {
          arraylist_head = arraylist_tail = InsertArraySignature(arraylist_tail, $2);
      }
    ;

initial_array : L_BRACE literal_list R_BRACE
              ;

literal_list : literal_list COMMA logical_expression
             | logical_expression
             |
             ;

const_decl : CONST scalar_type const_list SEMICOLON {
                 table->entry_tail = InsertEntryFromId(table, idlist_head, "constant", $2);
                 DeleteType($2);
                 DeleteIdList(idlist_head);
                 idlist_head = idlist_tail = NULL;
             }
           ;

const_list : const_list COMMA ID ASSIGN_OP literal_const {
                 idlist_tail = InsertIdList(idlist_tail, $3, NULL, $5);
             }
           | ID ASSIGN_OP literal_const {
                 idlist_head = idlist_tail = InsertIdList(idlist_tail, $1, NULL, $3);
             }
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

compound_statement : L_BRACE { table = InsertTable(table, table->level + 1); }
                     var_const_stmt_list R_BRACE {
                         PrintTable(table);
                         table = table->prev;
                         DeleteTable(table->next);
                         table->next = NULL;
                     }
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

conditional_statement : IF L_PAREN logical_expression R_PAREN compound_statement
                      | IF L_PAREN logical_expression R_PAREN compound_statement
                        ELSE compound_statement
                      ;

while_statement : WHILE L_PAREN logical_expression R_PAREN compound_statement
                | DO compound_statement
                  WHILE L_PAREN logical_expression R_PAREN SEMICOLON
                ;

for_statement : FOR L_PAREN initial_expression_list SEMICOLON control_expression_list SEMICOLON increment_expression_list R_PAREN
                    compound_statement
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

scalar_type : INT    { $$ = CreateType($1, NULL); }
            | DOUBLE { $$ = CreateType($1, NULL); }
            | STRING { $$ = CreateType($1, NULL); }
            | BOOL   { $$ = CreateType($1, NULL); }
            | FLOAT  { $$ = CreateType($1, NULL); }
            ;
 
literal_const : INT_CONST   { $$ = CreateValue("int", yytext); }
              | FLOAT_CONST { $$ = CreateValue("float", yytext); }
              | SCIENTIFIC  { $$ = CreateValue("scientific", yytext); }
              | STR_CONST   { $$ = CreateValue("string", $1); }
              | TRUE        { $$ = CreateValue("bool", yytext); }
              | FALSE       { $$ = CreateValue("bool", yytext); }
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
