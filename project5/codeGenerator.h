#include <stdio.h>
#include "datatype.h"
#include "symtable.h"

extern FILE *fpout;
int compareNum;
int ifNum, ifStack[50], ifStackPointer;
int loopNum, loopStack[50], loopStackPointer;

void programInitialization();
void generateVariableDeclaration(struct SymTable* table, const char* name, struct ExtType* varType);
void generateVariableReference(struct SymTable* table, const char* name);
void generateLiteralConstant(struct ConstAttr* constAttr);
void convertType(struct ExtType* base, struct ExtType *type);
void generateArithmeticOP(struct ExtType* base, struct ExtType *type1, struct ExtType *type2, const char op);
void generateModuloOP(struct ExtType* base);
void generateNegativeOP(struct ExtType* base);
void generateLogicalOP(struct ExtType* base, const char* op);
void generateRelationalOP(struct ExtType *type1, struct ExtType *type2, const char* op);
void generateVariableInitialization(struct SymTable* table, const char* name, struct ExtType* varType, struct ExtType* valueType, int varNumOffset);
void generateVariableAssignment(struct SymTable* table, const char* name, struct ExtType* valueType);
void generatePrintStart();
void generatePrintEnd(struct ExtType* type);
void generateReadStart(struct SymTable* table, const char* name, struct ExtType* varType);
void generateIfStart();
void generateElseStart();
void generateIfEnd();
void generateLoopStart();
void generateLoopControl();
void generateLoopIncrement();
void generateLoopEnd();
void generateDoWhileControl();
void generateFunctionStart(struct ExtType* returnType, struct FuncAttrNode* paramHead, const char* name, bool isEnrtyFunc);
void generateFunctionEnd(bool isVoid);
void generateFunctionReturn(struct ExtType* funcitonType, struct ExtType* returnType, bool isEnrtyFunc);
void generateFunctionInvocation(struct SymTable* table, const char* name);