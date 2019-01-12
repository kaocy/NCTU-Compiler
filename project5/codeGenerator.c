#include <stdio.h>
#include "codeGenerator.h"

void programInitialization() {
    fprintf(fpout, ".class public output\n");
    fprintf(fpout, ".super java/lang/Object\n\n");
}

void generateVariableDeclaration(struct SymTableNode* entry) {
    // for global variable
    if (entry->level == 0) {
        switch (entry->type->baseType) {
            case INT_t:
                fprintf(fpout, ".field public static %s I\n", entry->name);
                break;
            case FLOAT_t:
                fprintf(fpout, ".field public static %s F\n", entry->name);
                break;
            case DOUBLE_t:
                fprintf(fpout, ".field public static %s D\n", entry->name);
                break;
            case BOOL_t:
                fprintf(fpout, ".field public static %s Z\n", entry->name);
                break;
            default:
                break;
        }
    }
}

