#include <stdio.h>
#include "datatype.h"
extern FILE *fpout;

void programInitialization();
void generateVariableDeclaration(struct SymTableNode* entry);
void generateVariableReference(struct SymTable* table, const char* name);
void generateLiteralConstant(struct ConstAttr* constAttr);