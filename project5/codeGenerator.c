#include <stdio.h>
#include <string.h>
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

void generateVariableReference(struct SymTable* table, const char* name) {
    // find symbol table again due to level and varNum
    while (table != NULL) {
        struct SymTableNode *entry = table->head;
        while (entry != NULL) {
            if (entry->kind == CONSTANT_t) {
                if (strcmp(entry->name, name) == 0) {
                    switch (entry->type->baseType) {
                        case INT_t:
                            fprintf(fpout, "\tldc %d\n", entry->attr->constVal->value.integerVal);
                            return ;
                        case FLOAT_t:
                            fprintf(fpout, "\tldc %f\n", entry->attr->constVal->value.floatVal);
                            return ;
                        case DOUBLE_t:
                            fprintf(fpout, "\tldc %lf\n", entry->attr->constVal->value.doubleVal);
                            return ;
                        case BOOL_t:
                            if (entry->attr->constVal->value.boolVal)
                                fprintf(fpout, "\ticonst_1\n");
                            else
                                fprintf(fpout, "\ticonst_0\n");
                            return ;
                        case STRING_t:
                            fprintf(fpout, "\tldc \"%s\"\n", entry->attr->constVal->value.stringVal);
                            return ;
                        default:
                            return ;
                    }
                }
            }
            if (entry->kind == PARAMETER_t || entry->kind == VARIABLE_t) {
                if (strcmp(entry->name, name) == 0) {
                    if (entry->level == 0) {
                        switch (entry->type->baseType) {
                            case INT_t:
                                fprintf(fpout, "\t.getstatic output/%s I\n", entry->name);
                                return ;
                            case FLOAT_t:
                                fprintf(fpout, "\t.getstatic output/%s F\n", entry->name);
                                return ;
                            case DOUBLE_t:
                                fprintf(fpout, "\t.getstatic output/%s D\n", entry->name);
                                return ;
                            case BOOL_t:
                                fprintf(fpout, "\t.getstatic output/%s Z\n", entry->name);
                                return ;
                            default:
                                return ;
                        }
                    }
                    else {
                        switch (entry->type->baseType) {
                            case INT_t:
                                fprintf(fpout, "\tiload %d\n", entry->varNum);
                                return ;
                            case FLOAT_t:
                                fprintf(fpout, "\tfload %d\n", entry->varNum);
                                return ;
                            case DOUBLE_t:
                                fprintf(fpout, "\tdload %d\n", entry->varNum);
                                return ;
                            case BOOL_t:
                                fprintf(fpout, "\tiload %d\n", entry->varNum);
                                return ;
                            default:
                                return ;
                        }
                    }
                }
            }
            entry = entry->next;
        }
        table = table->prev;
    }
}

void generateLiteralConstant(struct ConstAttr* constAttr) {
    switch (constAttr->type) {
        case INT_t:
            fprintf(fpout, "\tldc %d\n", constAttr->value.integerVal);
            return ;
        case FLOAT_t:
            fprintf(fpout, "\tldc %f\n", constAttr->value.floatVal);
            return ;
        case DOUBLE_t:
            fprintf(fpout, "\tldc %lf\n", constAttr->value.doubleVal);
            return ;
        case BOOL_t:
            if (constAttr->value.boolVal)
                fprintf(fpout, "\ticonst_1\n");
            else
                fprintf(fpout, "\ticonst_0\n");
            return ;
        case STRING_t:
            fprintf(fpout, "\tldc \"%s\"\n", constAttr->value.stringVal);
            return ;
        default:
            return ;
    }
}
