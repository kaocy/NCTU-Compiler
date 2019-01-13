%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "symtable.h"
#include "codeGenerator.h"

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
bool hasReturn; // check non-void function last line is return
bool isEntryFunc = true; // the first function definition is entry function
int inLoop = 0; // check whether in loop
char assignVar[33]; // record variable to be assigned
bool assignVarEnable = true; // check whether to record new assignVar
BTYPE declareVarType; // record declaration type before insert to symbol table
int varNumOffset; // record variable declaration number in one line;
%}

%union {
    int    intVal;
    float  floatVal;
    double doubleVal;
    char   *stringVal;
    char   *idName;
    struct ExtType      *extType;
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
%type <intVal> dimension
%type <funcAttrNode> parameter_list
%type <attribute> literal_const 
%type <symTableNode> const_list
%type <bType> scalar_type

%type <extType> literal_list
%type <extType> initial_array

%type <extType> logical_expression_list
%type <extType> logical_expression
%type <extType> logical_term
%type <extType> logical_factor
%type <extType> relation_expression
%type <extType> arithmetic_expression
%type <extType> term
%type <extType> factor
%type <extType> variable_reference
%type <extType> array_list

%type <stringVal> relation_operator

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
            checkUndeclaraFunction(symbolTableList->global);
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
                funcReturnType = createExtType($1, false, NULL, EXPRESSION_t);
                struct SymTableNode* node = findFuncDeclaration(symbolTableList->global, $2, funcReturnType, NULL);
                //no declaration yet
                if (node == NULL) {
                    struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, NULL, true);
                    insertTableNode(symbolTableList->global, newNode);
                }
                generateFunctionStart(funcReturnType, NULL, $2, isEntryFunc);
                free($2);
            }
            compound_statement {
                checkFunctionReturn(hasReturn);
                isEntryFunc = false;
                generateFunctionEnd(false);
            }
          | scalar_type ID L_PAREN parameter_list R_PAREN {
                funcReturnType = createExtType($1, false, NULL, EXPRESSION_t);
                struct Attribute *attr = createFunctionAttribute($4);
                struct SymTableNode* node = findFuncDeclaration(symbolTableList->global, $2, funcReturnType, attr);
                // no declaration yet
                if (node == NULL) {
                    struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, attr, true);
                    insertTableNode(symbolTableList->global,newNode);
                }
                generateFunctionStart(funcReturnType, $4, $2, isEntryFunc);
            }
            L_BRACE { // enter a new scope
                ++scope;
                AddSymTable(symbolTableList, isEntryFunc);
                // add parameters
                struct FuncAttrNode *attrNode = $4;
                while (attrNode != NULL) {
                    struct SymTableNode *newNode = createParameterNode(attrNode->name, scope, attrNode->value);
                    insertTableNode(symbolTableList->tail, newNode);
                    attrNode = attrNode->next;
                }
            }
            var_const_stmt_list R_BRACE {
                checkFunctionReturn(hasReturn);
                isEntryFunc = false;
                if (Opt_SymTable == 1)
                    printSymTable(symbolTableList->tail);
                deleteLastSymTable(symbolTableList);
                generateFunctionEnd(false);
                --scope;
                free($2);
            }
          | VOID ID L_PAREN R_PAREN {
                funcReturnType = createExtType(VOID_t, false, NULL, EXPRESSION_t);
                struct SymTableNode *node = findFuncDeclaration(symbolTableList->global, $2, funcReturnType, NULL);
                // no declaration yet
                if (node == NULL) {
                    struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, NULL, true);
                    insertTableNode(symbolTableList->global, newNode);
                }
                generateFunctionStart(funcReturnType, NULL, $2, isEntryFunc);
                free($2);
            }
            compound_statement {
                isEntryFunc = false;
                generateFunctionEnd(true);
            }
          | VOID ID L_PAREN parameter_list R_PAREN {
                funcReturnType = createExtType(VOID_t, false, NULL, EXPRESSION_t);
                struct Attribute *attr = createFunctionAttribute($4);
                struct SymTableNode *node = findFuncDeclaration(symbolTableList->global, $2, funcReturnType, attr);
                // no declaration yet
                if (node == NULL) {
                    struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, attr, true);
                    insertTableNode(symbolTableList->global, newNode);
                }
                generateFunctionStart(funcReturnType, $4, $2, isEntryFunc);
            }
            L_BRACE { // enter a new scope
                ++scope;
                AddSymTable(symbolTableList, isEntryFunc);
                // add parameters
                struct FuncAttrNode *attrNode = $4;
                while (attrNode != NULL) {
                    struct SymTableNode *newNode = createParameterNode(attrNode->name, scope, attrNode->value);
                    insertTableNode(symbolTableList->tail, newNode);
                    attrNode = attrNode->next;
                }
            }
            var_const_stmt_list R_BRACE {
                isEntryFunc = false;
                if (Opt_SymTable == 1)
                    printSymTable(symbolTableList->tail);
                deleteLastSymTable(symbolTableList);
                generateFunctionEnd(true);
                --scope;
                free($2);
            }
          ;

funct_decl : scalar_type ID L_PAREN R_PAREN SEMICOLON {
                funcReturnType = createExtType($1, false, NULL, EXPRESSION_t);
                struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, NULL, false);
                insertTableNode(symbolTableList->global, newNode);
                free($2);
            }
           | scalar_type ID L_PAREN parameter_list R_PAREN SEMICOLON {
                funcReturnType = createExtType($1, false, NULL, EXPRESSION_t);
                struct Attribute *attr = createFunctionAttribute($4);
                struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, attr, false);
                insertTableNode(symbolTableList->global, newNode);
                free($2);
            }
           | VOID ID L_PAREN R_PAREN SEMICOLON {
                funcReturnType = createExtType(VOID_t, false, NULL, EXPRESSION_t);
                struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, NULL, false);
                insertTableNode(symbolTableList->global, newNode);
                free($2);
            }
           | VOID ID L_PAREN parameter_list R_PAREN SEMICOLON {
                funcReturnType = createExtType(VOID_t, false, NULL, EXPRESSION_t);
                struct Attribute *attr = createFunctionAttribute($4);
                struct SymTableNode *newNode = createFunctionNode($2, scope, funcReturnType, attr, false);
                insertTableNode(symbolTableList->global, newNode);
                free($2);
            }
           ;

parameter_list : parameter_list COMMA scalar_type ID {
                    struct FuncAttrNode *newNode = (struct FuncAttrNode*)malloc(sizeof(struct FuncAttrNode));
                    newNode->value = createExtType($3, false, NULL, PARAMETER_t);
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
                    newNode->value = createExtType($1, false, NULL, PARAMETER_t);
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
                    // check initial value type
                    if (listNode->valueType != NULL) {
                        checkVariableInitialization(newNode->type, listNode->valueType);
                    }
                    // check initial array type
                    if (listNode->initArrayHead != NULL) {
                        checkArrayInitialization(newNode->type, listNode->initArrayHead);
                    }
                    int result = insertTableNode(symbolTableList->tail, newNode);
                    listNode = listNode->next;
                }
                deleteVariableList($2);
                varNumOffset = 0;
            }
         ;

identifier_list : identifier_list COMMA ID assign_op logical_expression {
                    struct ExtType *type = createExtType(VOID, false, NULL, VARIABLE_t); // type unknown here
                    $5->reference -= 1; // due to createVariable
                    struct Variable *newVariable = createVariable($3, type, $5, NULL);

                    type->baseType = declareVarType;
                    generateVariableDeclaration(symbolTableList->tail, $3, type);
                    generateVariableInitialization(symbolTableList->tail, $3, type, $5, varNumOffset);
                    varNumOffset++;
                    if (type->baseType == DOUBLE_t) varNumOffset++;
                    assignVarEnable = true;

                    free($3);
                    connectVariableList($1, newVariable);
                    $$ = $1;
                }
                | identifier_list COMMA ID {
                    struct ExtType *type = createExtType(VOID, false, NULL, VARIABLE_t); // type unknown here
                    struct Variable *newVariable = createVariable($3, type, NULL, NULL);

                    type->baseType = declareVarType;
                    generateVariableDeclaration(symbolTableList->tail, $3, type);
                    varNumOffset++;
                    if (type->baseType == DOUBLE_t) varNumOffset++;

                    free($3);
                    connectVariableList($1, newVariable);
                    $$ = $1;
                }
                | ID assign_op logical_expression {  
                    struct ExtType *type = createExtType(VOID, false, NULL, VARIABLE_t); // type unknown here
                    $3->reference -= 1; // due to createVariable
                    struct Variable *newVariable = createVariable($1, type, $3, NULL);

                    type->baseType = declareVarType;
                    generateVariableDeclaration(symbolTableList->tail, $1, type);
                    generateVariableInitialization(symbolTableList->tail, $1, type, $3, varNumOffset);
                    varNumOffset = 1;
                    if (type->baseType == DOUBLE_t) varNumOffset++;
                    assignVarEnable = true;

                    $$ = createVariableList(newVariable);
                    free($1);
                }
                | ID {
                    struct ExtType *type = createExtType(VOID, false, NULL, VARIABLE_t); // type unknown here
                    struct Variable *newVariable = createVariable($1, type, NULL, NULL);

                    type->baseType = declareVarType;
                    generateVariableDeclaration(symbolTableList->tail, $1, type);
                    varNumOffset = 1;
                    if (type->baseType == DOUBLE_t) varNumOffset++;

                    $$ = createVariableList(newVariable);
                    free($1);           
                }
                | identifier_list COMMA array_decl assign_op initial_array {
                    $3->initArrayHead = $5;
                    connectVariableList($1, $3);
                    $$ = $1;
                    assignVarEnable = true;
                }
                | identifier_list COMMA array_decl {
                    connectVariableList($1, $3);
                    $$ = $1;
                }
                | array_decl assign_op initial_array {
                    $1->initArrayHead = $3;
                    $$ = createVariableList($1);
                    assignVarEnable = true;
                }
                | array_decl {
                    $$ = createVariableList($1);
                }
                ;

array_decl : ID dim {
                struct ExtType *type = createExtType(VOID, true, $2, VARIABLE_t); // type unknown here
                struct Variable *newVariable = createVariable($1, type, NULL, NULL);
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

initial_array : L_BRACE literal_list R_BRACE { $$ = $2; }
              ;

literal_list : literal_list COMMA logical_expression {
                    $3->reference -= 1; // in connect, reference += 1
                    connectExtType($1, $3);
                    $$ = $1;
                }
             | logical_expression { $$ = $1; }
             | { $$ = NULL; }
             ;

const_decl : CONST scalar_type const_list SEMICOLON {
                // symTableNode base on initailized data type, scalar_type is not used
                struct SymTableNode *list = $3;
                while (list->next != NULL) {
                    struct SymTableNode *temp = list->next;
                    list->next = NULL;
                    checkConstantInitialization($2, list->attr->constVal->type);
                    insertTableNode(symbolTableList->tail, list);
                    list = temp;
                }
                // for last node
                checkConstantInitialization($2, list->attr->constVal->type);
                insertTableNode(symbolTableList->tail, list);
            }
           ;

const_list : const_list COMMA ID ASSIGN_OP literal_const {
                struct ExtType *type = createExtType($5->constVal->type, false, NULL, CONSTANT_t);
                struct SymTableNode *temp = $1;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = createConstNode($3, scope, type, $5); 
                free($3);
            }
           | ID ASSIGN_OP literal_const {
                struct ExtType *type = createExtType($3->constVal->type, false, NULL, CONSTANT_t);
                $$ = createConstNode($1, scope, type, $3); 
                free($1);
            }    
           ;

var_const_stmt_list : var_const_stmt_list statement 
                    | var_const_stmt_list var_decl { hasReturn = false; }
                    | var_const_stmt_list const_decl { hasReturn = false; }
                    |
                    ;

/*********************** Statements ***********************/

statement : compound_statement { hasReturn = false; }
          | simple_statement { hasReturn = false; }
          | conditional_statement { hasReturn = false; }
          | while_statement { hasReturn = false; }
          | for_statement { hasReturn = false; }
          | function_invoke_statement { hasReturn = false; }
          | jump_statement
          ; 

compound_statement : L_BRACE  { // enter a new scope
                        ++scope;
                        AddSymTable(symbolTableList, isEntryFunc);
                    }
                     var_const_stmt_list R_BRACE {
                        if (Opt_SymTable == 1)
                            printSymTable(symbolTableList->tail);
                        deleteLastSymTable(symbolTableList);
                        --scope;
                    }
                   ;    

simple_statement : variable_reference assign_op logical_expression SEMICOLON {
                        checkAssignType($1, $3);
                        generateVariableAssignment(symbolTableList->tail, assignVar, $3);
                        assignVarEnable = true;
                        deleteExtType($1);
                        deleteExtType($3);
                    }
                 | PRINT {
                        generatePrintStart();
                        assignVarEnable = false;
                    } 
                   logical_expression SEMICOLON {
                        checkScalarType($3);
                        generatePrintEnd($3);
                        deleteExtType($3);
                        assignVarEnable = true;
                    }
                 | READ variable_reference SEMICOLON {
                        checkScalarType($2);
                        generateReadStart(symbolTableList->tail, assignVar, $2);
                        deleteExtType($2);
                    }
                 ;

conditional_statement : if_statement
                        compound_statement { generateElseStart(); generateIfEnd(); }
                      | if_statement
                        compound_statement { generateElseStart(); }
                        ELSE compound_statement { generateIfEnd(); }
                      ;

if_statement : IF { assignVarEnable = false; }
               L_PAREN boolean_expression R_PAREN { assignVarEnable = true; generateIfStart(); }

while_statement : WHILE L_PAREN { generateLoopStart(); assignVarEnable = false; }
                  boolean_expression R_PAREN { generateLoopControl(); assignVarEnable = true; generateLoopIncrement(); }
                  L_BRACE { // enter a new scope
                    ++scope;
                    ++inLoop;
                    AddSymTable(symbolTableList, false);
                } 
                  var_const_stmt_list R_BRACE {   
                    if (Opt_SymTable == 1)
                        printSymTable(symbolTableList->tail);
                    deleteLastSymTable(symbolTableList);
                    --scope;
                    --inLoop;
                    generateLoopEnd();
                }
                | DO L_BRACE { // enter a new scope
                    ++scope;
                    ++inLoop;
                    AddSymTable(symbolTableList, false);
                    generateLoopStart();
                }
                  var_const_stmt_list
                  R_BRACE WHILE L_PAREN { assignVarEnable = false; }
                  boolean_expression R_PAREN SEMICOLON {
                    if (Opt_SymTable == 1)
                        printSymTable(symbolTableList->tail);
                    deleteLastSymTable(symbolTableList);
                    --scope;
                    --inLoop;
                    generateDoWhileControl();
                    assignVarEnable = true;
                }
                ;

for_statement : FOR L_PAREN 
                initial_expression { generateLoopStart(); assignVarEnable = false; } SEMICOLON
                control_expression { generateLoopControl(); assignVarEnable = true; } SEMICOLON
                increment_expression { generateLoopIncrement(); } R_PAREN 
                L_BRACE { // enter a new scope
                    ++scope;
                    ++inLoop;
                    AddSymTable(symbolTableList, false);
                }
                var_const_stmt_list R_BRACE {
                    if (Opt_SymTable == 1)
                        printSymTable(symbolTableList->tail);
                    deleteLastSymTable(symbolTableList);
                    --scope;
                    --inLoop;
                    generateLoopEnd();
                }
              ;

boolean_expression : logical_expression {
                        checkConditionalExpression($1);
                        deleteExtType($1);
                    }

initial_expression : logical_expression
                   | variable_reference assign_op logical_expression {
                        checkAssignType($1, $3);
                        generateVariableAssignment(symbolTableList->tail, assignVar, $3);
                        deleteExtType($1);
                        deleteExtType($3);
                        assignVarEnable = true;
                    }

control_expression : logical_expression {
                        checkControlExpression($1);
                        deleteExtType($1);
                    }
                   | variable_reference assign_op logical_expression {
                        checkAssignType($1, $3);
                        checkControlExpression($1);
                        generateVariableAssignment(symbolTableList->tail, assignVar, $3);
                        deleteExtType($1);
                        deleteExtType($3);
                        assignVarEnable = true;
                    }
                   ;

increment_expression : logical_expression
                     | variable_reference assign_op logical_expression {
                            checkAssignType($1, $3);
                            generateVariableAssignment(symbolTableList->tail, assignVar, $3);
                            deleteExtType($1);
                            deleteExtType($3);
                            assignVarEnable = true;
                        }
                     ;

function_invoke_statement : ID L_PAREN logical_expression_list R_PAREN SEMICOLON {
                                findFuncForInvocation(symbolTableList->global, $1, $3);
                                deleteExtTypeList($3);
                                free($1);
                            }
                          | ID L_PAREN R_PAREN SEMICOLON {
                                findFuncForInvocation(symbolTableList->global, $1, NULL);
                                free($1);
                            }
                          ;

jump_statement : CONTINUE SEMICOLON {
                    hasReturn = false;
                    checkInLoop(inLoop, "continue");
                }
               | BREAK SEMICOLON {
                    hasReturn = false;
                    checkInLoop(inLoop, "break");
                }
               | RETURN logical_expression SEMICOLON { 
                    hasReturn = true;
                    checkFunctionReturnType(funcReturnType, $2);
                    deleteExtType($2);
                }
               ;

/************************ Utilities ************************/

logical_expression_list : logical_expression_list COMMA logical_expression {
                            $3->reference -= 1; // in connect, reference += 1
                            connectExtType($1, $3);
                            $$ = $1;
                        }
                        | logical_expression { $$ = $1; }
                        ;

logical_expression : logical_expression OR_OP logical_term {
                        $$ = logicalOP($1, $3, "||");
                        $$->reference += 1;
                        generateLogicalOP($$, "||");
                        deleteExtType($1);
                        deleteExtType($3);
                    }
                   | logical_term { $$ = $1; }
                   ;

logical_term : logical_term AND_OP logical_factor {
                    $$ = logicalOP($1, $3, "&&");
                    $$->reference += 1;
                    generateLogicalOP($$, "&&");
                    deleteExtType($1);
                    deleteExtType($3);
                }
             | logical_factor { $$ = $1; }
             ;

logical_factor : NOT_OP logical_factor {
                    $$ = logicalNotOP($2);
                    $$->reference += 1;
                    generateLogicalOP($$, "!");
                    deleteExtType($2);
                }
               | relation_expression { $$ = $1; }
               ;

relation_expression : arithmetic_expression relation_operator arithmetic_expression {
                        $$ = relationalOP($1, $3, $2);
                        $$->reference += 1;
                        generateRelationalOP($1, $3, $2);
                        deleteExtType($1);
                        deleteExtType($3);
                        free($2);
                    }
                    | arithmetic_expression { $$ = $1; }
                    ;

relation_operator : LT_OP { $$ = strdup("<"); }
                  | LE_OP { $$ = strdup("<="); }
                  | EQ_OP { $$ = strdup("=="); }
                  | GE_OP { $$ = strdup(">="); }
                  | GT_OP { $$ = strdup(">"); }
                  | NE_OP { $$ = strdup("!="); }
                  ;

arithmetic_expression : arithmetic_expression ADD_OP term {
                            $$ = arithmeticOP($1, $3, '+');
                            $$->reference += 1;
                            generateArithmeticOP($$, $1, $3, '+');
                            deleteExtType($1);
                            deleteExtType($3);
                        }
                      | arithmetic_expression SUB_OP term {
                            $$ = arithmeticOP($1, $3, '-');
                            $$->reference += 1;
                            generateArithmeticOP($$, $1, $3, '-');
                            deleteExtType($1);
                            deleteExtType($3);
                        }
                      | term { $$ = $1; }
                      ;

term : term MUL_OP factor {
            $$ = arithmeticOP($1, $3, '*');
            $$->reference += 1;
            generateArithmeticOP($$, $1, $3, '*');
            deleteExtType($1);
            deleteExtType($3);
        }
     | term DIV_OP factor {
            $$ = arithmeticOP($1, $3, '/');
            $$->reference += 1;
            generateArithmeticOP($$, $1, $3, '/');
            deleteExtType($1);
            deleteExtType($3);
        }
     | term MOD_OP factor {
            $$ = moduloOP($1, $3);
            $$->reference += 1;
            generateModuloOP($$);
            deleteExtType($1);
            deleteExtType($3);
        }
     | factor { $$ = $1; }
     ;

factor : variable_reference { $$ = $1; }
       | SUB_OP factor {
            $$ = $2;
            generateNegativeOP($2);
        }
       | L_PAREN logical_expression R_PAREN { $$ = $2; }
       | ID L_PAREN logical_expression_list R_PAREN {
            $$ = findFuncForInvocation(symbolTableList->global, $1, $3);
            $$->reference += 1;
            deleteExtTypeList($3);
            free($1);
        }
       | ID L_PAREN R_PAREN {
            $$ = findFuncForInvocation(symbolTableList->global, $1, NULL);
            $$->reference += 1;
            free($1);
        }
       | literal_const {
            $$ = createExtType($1->constVal->type, false, NULL, EXPRESSION_t);
            $$->reference += 1;
            generateLiteralConstant($1->constVal);
            deleteAttribute($1);
        }
       ;

variable_reference : array_list { $$ = $1; }
                   | ID {
                        $$ = findVariable(symbolTableList->tail, $1, 0);
                        $$->reference += 1;
                        // variable assignment need not get the origin value of variable
                        if (!assignVarEnable)
                            generateVariableReference(symbolTableList->tail, $1);
                        if (assignVarEnable)
                            strcpy(assignVar, $1);
                        free($1);
                    }
                   ;

array_list : ID dimension {
                $$ = findVariable(symbolTableList->tail, $1, $2);
                $$->reference += 1;
                free($1);
            }
           ;

dimension : dimension ML_BRACE logical_expression MR_BRACE {
                checkArrayIndex($3);
                $$ = $1 + 1;
            }
          | ML_BRACE logical_expression MR_BRACE {
                checkArrayIndex($2);
                $$ = 1;
            }
          ;

scalar_type : INT    { $$ = INT_t;    declareVarType = $$; }
            | DOUBLE { $$ = DOUBLE_t; declareVarType = $$; }
            | STRING { $$ = STRING_t; declareVarType = $$; }
            | BOOL   { $$ = BOOL_t;   declareVarType = $$; }
            | FLOAT  { $$ = FLOAT_t;  declareVarType = $$; }
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

assign_op: ASSIGN_OP { assignVarEnable = false; }

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
