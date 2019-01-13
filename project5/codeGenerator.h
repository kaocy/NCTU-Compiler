#include <stdio.h>
#include "datatype.h"
#include "symtable.h"

extern FILE *fpout;
int branchNum;

void programInitialization();
void generateVariableDeclaration(struct SymTableNode* entry);
void generateVariableReference(struct SymTable* table, const char* name);
void generateLiteralConstant(struct ConstAttr* constAttr);
void convertType(struct ExtType* base, struct ExtType *type);
void generateArithmeticOP(struct ExtType* base, struct ExtType *type1, struct ExtType *type2, const char op);
void generateModuloOP(struct ExtType* base);
void generateNegativeOP(struct ExtType* base);
void generateLogicalOP(struct ExtType* base, const char* op);
void generateRelationalOP(struct ExtType *type1, struct ExtType *type2, const char* op);