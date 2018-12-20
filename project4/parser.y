%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"datatype.h"
#include"symtable.h"

// declared in scanner.l
extern int linenum;
extern FILE *yyin;
extern char *yytext;
extern char buf[256];
extern int Opt_SymTable;

int yylex();
int yyerror(char *msg);

int scope = 0; // default is 0(global)
struct SymTableList *symbolTableList; // create and initialize in main.c
struct ExtType *funcReturnType;
%}

%union {
    int    intVal;
    float  floatVal;
    double doubleVal;
    char   *stringVal;
    char   *idName;
    // struct ExtType      *extType;
    struct Variable     *variable;
    struct VariableList *variableList;
    struct ArrayDimNode *arrayDimNode;
    // struct ConstAttr    *constAttr;
    struct FuncAttrNode *funcAttrNode;
    // struct FuncAttr     *funcAttr;
    struct Attribute    *attribute;
    struct SymTableNode *symTableNode;
    // struct SymTable     *symTable;
    BTYPE  bType;
};

%token <idName> ID
%token <intVal> INT_CONST
%token <floatVal> FLOAT_CONST
%token <doubleVal> SCIENTIFIC
%token <stringVal> STR_CONST

%type <variable> array_decl
%type <variableList> identifier_list
%type <arrayDimNode> dim
%type <funcAttrNode> parameter_list
%type <attribute> literal_const 
%type <symTableNode> const_list
%type <bType> scalar_type

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

%start program

%%

/********************** Program Units **********************/

program : decl_list funct_def decl_and_def_list {
            if (Opt_SymTable == 1)
                printSymTable(symbolTableList->global);
            deleteLastSymTable(symbolTableList);
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

funct_def : scalar_type ID L_PAREN R_PAREN {
                funcReturnType = createExtType($1, 0, NULL);
                struct SymTableNode *node;
                node = findFuncDeclaration(symbolTableList->global, $2);
                //no declaration yet
                if (node == NULL) {
                    struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, NULL);
                    insertTableNode(symbolTableList->global, newNode);
                }
                free($2);
            }
            compound_statement
          | scalar_type ID L_PAREN parameter_list R_PAREN {
                funcReturnType = createExtType($1, 0, NULL);
                struct SymTableNode *node;
                node = findFuncDeclaration(symbolTableList->global, $2);
                struct Attribute *attr = createFunctionAttribute($4);
                // no declaration yet
                if (node == NULL) {
                    struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, attr);
                    insertTableNode(symbolTableList->global,newNode);
                }
            }
            L_BRACE { // enter a new scope
                ++scope;
                AddSymTable(symbolTableList);
                // add parameters
                struct FuncAttrNode *attrNode = $4;
                while (attrNode != NULL) {
                    struct SymTableNode *newNode = createParameterNode(attrNode->name, scope, attrNode->value);
                    insertTableNode(symbolTableList->tail, newNode);
                    attrNode = attrNode->next;
                }
            }
            var_const_stmt_list R_BRACE {   
                if (Opt_SymTable == 1)
                    printSymTable(symbolTableList->tail);
                deleteLastSymTable(symbolTableList);
                --scope;
                free($2);
            }
          | VOID ID L_PAREN R_PAREN {
                funcReturnType = createExtType(VOID_t, 0, NULL);
                struct SymTableNode *node;
                node = findFuncDeclaration(symbolTableList->global, $2);
                // no declaration yet
                if (node == NULL) {
                    struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, NULL);
                    insertTableNode(symbolTableList->global, newNode);
                }       
                free($2);
            }
            compound_statement
          | VOID ID L_PAREN parameter_list R_PAREN {
                funcReturnType = createExtType(VOID_t, 0, NULL);
                struct SymTableNode *node;
                node = findFuncDeclaration(symbolTableList->global, $2);
                // no declaration yet
                if (node == NULL) {
                    struct Attribute *attr = createFunctionAttribute($4);
                    struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, attr);
                    insertTableNode(symbolTableList->global, newNode);
                }
            }
            L_BRACE { // enter a new scope
                ++scope;
                AddSymTable(symbolTableList);
                // add parameters
                struct FuncAttrNode *attrNode = $4;
                while (attrNode != NULL) {
                    struct SymTableNode *newNode = createParameterNode(attrNode->name, scope, attrNode->value);
                    insertTableNode(symbolTableList->tail, newNode);
                    attrNode = attrNode->next;
                }
            }
            var_const_stmt_list R_BRACE {   
                if (Opt_SymTable == 1)
                    printSymTable(symbolTableList->tail);
                deleteLastSymTable(symbolTableList);
                --scope;
                free($2);
            }
          ;

funct_decl : scalar_type ID L_PAREN R_PAREN SEMICOLON {
                funcReturnType = createExtType($1, 0, NULL);
                struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, NULL);
                insertTableNode(symbolTableList->global, newNode);
                free($2);
            }
           | scalar_type ID L_PAREN parameter_list R_PAREN SEMICOLON {
                funcReturnType = createExtType($1, 0, NULL);
                struct Attribute *attr = createFunctionAttribute($4);
                struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, attr);
                insertTableNode(symbolTableList->global, newNode);
                free($2);
            }
           | VOID ID L_PAREN R_PAREN SEMICOLON {
                funcReturnType = createExtType(VOID_t, 0, NULL);
                struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, NULL);
                insertTableNode(symbolTableList->global, newNode);
                free($2);
            }
           | VOID ID L_PAREN parameter_list R_PAREN SEMICOLON {
                funcReturnType = createExtType(VOID_t, 0, NULL);
                struct Attribute *attr = createFunctionAttribute($4);
                struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, attr);
                insertTableNode(symbolTableList->global, newNode);
                free($2);
            }
           ;

parameter_list : parameter_list COMMA scalar_type ID {
                    struct FuncAttrNode *newNode = (struct FuncAttrNode*)malloc(sizeof(struct FuncAttrNode));
                    newNode->value = createExtType($3, 0, NULL);
                    newNode->name = strdup($4);
                    newNode->next = NULL;
                    free($4);              
                    connectFuncAttrNode($1, newNode);
                    $$ = $1;
                }
               | parameter_list COMMA scalar_type array_decl {
                    struct FuncAttrNode *newNode = (struct FuncAttrNode*)malloc(sizeof(struct FuncAttrNode));
                    newNode->value = $4->type; // use pre-built ExtType(type is unknown)
                    newNode->value->baseType = $3; // set correct type
                    newNode->name = strdup($4->name);
                    newNode->next = NULL;
                    free($4->name);
                    free($4);
                    connectFuncAttrNode($1, newNode);
                    $$ = $1;
                }
               | scalar_type ID {
                    struct FuncAttrNode *newNode = (struct FuncAttrNode*)malloc(sizeof(struct FuncAttrNode));
                    newNode->value = createExtType($1, 0, NULL);
                    newNode->name = strdup($2);
                    newNode->next = NULL;
                    free($2);  
                    $$ = newNode;
                }
               | scalar_type array_decl {
                    struct FuncAttrNode *newNode = (struct FuncAttrNode*)malloc(sizeof(struct FuncAttrNode));
                    newNode->value = $2->type; // use pre-built ExtType(type is unknown)
                    newNode->value->baseType = $1; // set correct type
                    newNode->name = strdup($2->name);
                    newNode->next = NULL;
                    free($2->name);
                    free($2);
                    $$ = newNode;
                }
               ;

/*************** Data Types and Declarations ***************/

var_decl : scalar_type identifier_list SEMICOLON {
                struct Variable* listNode = $2->head;
                struct SymTableNode *newNode;
                while (listNode != NULL) {
                    newNode = createVariableNode(listNode->name, scope, listNode->type);
                    newNode->type->baseType = $1;
                    insertTableNode(symbolTableList->tail, newNode);
                    listNode = listNode->next;
                }
                deleteVariableList($2);
            }
         ;

identifier_list : identifier_list COMMA ID {
                    struct ExtType *type = createExtType(VOID, false, NULL); // type unknown here
                    struct Variable *newVariable = createVariable($3, type);
                    free($3);
                    connectVariableList($1, newVariable);
                    $$ = $1;
                }
                | identifier_list COMMA ID ASSIGN_OP logical_expression {
                    struct ExtType *type = createExtType(VOID, false, NULL); // type unknown here
                    struct Variable *newVariable = createVariable($3, type);
                    free($3);
                    connectVariableList($1, newVariable);
                    $$ = $1;
                }
                | ID ASSIGN_OP logical_expression {  
                    struct ExtType *type = createExtType(VOID, false, NULL); // type unknown here
                    struct Variable *newVariable = createVariable($1, type);
                    $$ = createVariableList(newVariable);
                    free($1);
                }
                | ID {
                    struct ExtType *type = createExtType(VOID, false, NULL); // type unknown here
                    struct Variable *newVariable = createVariable($1, type);
                    $$ = createVariableList(newVariable);
                    free($1);
                }
                | identifier_list COMMA array_decl ASSIGN_OP initial_array {
                    connectVariableList($1, $3);
                    $$ = $1;
                }
                | identifier_list COMMA array_decl {
                    connectVariableList($1, $3);
                    $$ = $1;
                }
                | array_decl ASSIGN_OP initial_array {
                    $$ = createVariableList($1);
                }
                | array_decl {
                    $$ = createVariableList($1);
                }
                ;

array_decl : ID dim {
                struct ExtType *type = createExtType(VOID, true, $2); // type unknown here
                struct Variable *newVariable = createVariable($1, type);
                free($1);
                $$ = newVariable;
            }
           ;

dim : dim ML_BRACE INT_CONST MR_BRACE {
        connectArrayDimNode($1, createArrayDimNode($3));
        $$ = $1;
    }
    | ML_BRACE INT_CONST MR_BRACE {
        $$ = createArrayDimNode($2);
    }
    ;

initial_array : L_BRACE literal_list R_BRACE
              ;

literal_list : literal_list COMMA logical_expression
             | logical_expression
             | 
             ;

const_decl : CONST scalar_type const_list SEMICOLON {
                // symTableNode base on initailized data type, scalar_type is not used
                struct SymTableNode *list = $3;
                while (list->next != NULL) {
                    struct SymTableNode *temp = list->next;
                    list->next = NULL;
                    insertTableNode(symbolTableList->tail, list);
                    list = temp;
                }
                insertTableNode(symbolTableList->tail, list);
            }
           ;

const_list : const_list COMMA ID ASSIGN_OP literal_const {
                struct ExtType *type = createExtType($5->constVal->type, false, NULL);
                struct SymTableNode *temp = $1;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = createConstNode($3, scope, type, $5); 
                free($3);
            }
           | ID ASSIGN_OP literal_const {
                struct ExtType *type = createExtType($3->constVal->type, false, NULL);
                $$ = createConstNode($1, scope, type, $3); 
                free($1);
            }    
           ;

var_const_stmt_list : var_const_stmt_list statement 
                    | var_const_stmt_list var_decl
                    | var_const_stmt_list const_decl
                    |
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

compound_statement : L_BRACE  { // enter a new scope
                        ++scope;
                        AddSymTable(symbolTableList);
                    }
                     var_const_stmt_list R_BRACE {
                        if (Opt_SymTable == 1)
                            printSymTable(symbolTableList->tail);
                        deleteLastSymTable(symbolTableList);
                        --scope;
                    }
                   ;    

simple_statement : variable_reference ASSIGN_OP logical_expression SEMICOLON
                 | PRINT logical_expression SEMICOLON
                 | READ variable_reference SEMICOLON
                 ;

conditional_statement : IF L_PAREN logical_expression R_PAREN compound_statement
                      | IF L_PAREN logical_expression R_PAREN compound_statement  
                        ELSE compound_statement  
                      ;

while_statement : WHILE { // enter a new scope
                    ++scope;
                    AddSymTable(symbolTableList);
                }
                  L_PAREN logical_expression R_PAREN
                  L_BRACE var_const_stmt_list R_BRACE {   
                    if (Opt_SymTable == 1)
                        printSymTable(symbolTableList->tail);
                    deleteLastSymTable(symbolTableList);
                    --scope;
                }
                | DO L_BRACE { // enter a new scope
                    ++scope;
                    AddSymTable(symbolTableList);
                }
                  var_const_stmt_list
                  R_BRACE WHILE L_PAREN logical_expression R_PAREN SEMICOLON {
                    if (Opt_SymTable == 1)
                        printSymTable(symbolTableList->tail);
                    deleteLastSymTable(symbolTableList);
                    --scope;
                }
                ;

for_statement : FOR { // enter a new scope
                    ++scope;
                    AddSymTable(symbolTableList);
                }
                L_PAREN initial_expression_list SEMICOLON control_expression_list SEMICOLON increment_expression_list R_PAREN 
                L_BRACE var_const_stmt_list R_BRACE {
                    if (Opt_SymTable == 1)
                        printSymTable(symbolTableList->tail);
                    deleteLastSymTable(symbolTableList);
                    --scope;
                }
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

function_invoke_statement : ID L_PAREN logical_expression_list R_PAREN SEMICOLON { free($1); }
                          | ID L_PAREN R_PAREN SEMICOLON { free($1); }
                          ;

jump_statement : CONTINUE SEMICOLON
               | BREAK SEMICOLON
               | RETURN logical_expression SEMICOLON
               ;

/************************ Utilities ************************/

variable_reference : array_list
                   | ID { free($1); }
                   ;

array_list : ID dimension { free($1); }
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

relation_expression : arithmetic_expression relation_operator arithmetic_expression
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

factor : variable_reference
       | SUB_OP factor
       | L_PAREN logical_expression R_PAREN
       | ID L_PAREN logical_expression_list R_PAREN { free($1); }
       | ID L_PAREN R_PAREN { free($1); }
       | literal_const { deleteAttribute($1); }
       ;

scalar_type : INT { $$ = INT_t; }
            | DOUBLE { $$ = DOUBLE_t; }
            | STRING { $$ = STRING_t; }
            | BOOL { $$ = BOOL_t; }
            | FLOAT { $$ = FLOAT_t; }
            ;
 
literal_const : INT_CONST {
                    int val = $1;
                    $$ = createConstantAttribute(INT_t, &val);
                }
              | SUB_OP INT_CONST {
                    int val = -$2;
                    $$ = createConstantAttribute(INT_t, &val);
                }
              | FLOAT_CONST {
                    float val = $1;
                    $$ = createConstantAttribute(FLOAT_t, &val);
                }
              | SUB_OP FLOAT_CONST {
                    float val = -$2;
                    $$ = createConstantAttribute(FLOAT_t, &val);
                }
              | SCIENTIFIC {
                    double val = $1;
                    $$ = createConstantAttribute(DOUBLE_t, &val);
                }
              | SUB_OP SCIENTIFIC {
                    double val = -$2;
                    $$ = createConstantAttribute(DOUBLE_t, &val);
                }
              | STR_CONST {
                    $$ = createConstantAttribute(STRING_t, $1);
                    free($1);
                }
              | TRUE {
                    bool val = true;
                    $$ = createConstantAttribute(BOOL_t, &val);
                }
              | FALSE {
                    bool val = false;
                    $$ = createConstantAttribute(BOOL_t, &val);
                }
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
