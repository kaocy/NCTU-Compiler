#include <stdio.h>
#include <string.h>
#include "codeGenerator.h"

void programInitialization() {
    fprintf(fpout, ".class public output\n");
    fprintf(fpout, ".super java/lang/Object\n\n");
    branchNum = 0;
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

void convertType(struct ExtType* base, struct ExtType *type) {
    if (base->baseType == FLOAT_t) {
        if (type->baseType == INT_t) fprintf(fpout, "\ti2f\n");
    }
    if (base->baseType == DOUBLE_t) {
        if (type->baseType == INT_t) fprintf(fpout, "\ti2d\n");
        if (type->baseType == FLOAT_t) fprintf(fpout, "\tf2d\n");
    }
}

void generateArithmeticOP(struct ExtType* base, struct ExtType *type1, struct ExtType *type2, const char op) {
    if (base->baseType == INT_t) {
        if (op == '+')  fprintf(fpout, "\tiadd\n");
        if (op == '-')  fprintf(fpout, "\tisub\n");
        if (op == '*')  fprintf(fpout, "\timul\n");
        if (op == '/')  fprintf(fpout, "\tidiv\n");
    }          
    if (base->baseType == FLOAT_t) {
        // may need to convert type
        convertType(base, type2);
        fprintf(fpout, "\tfstore 99\n");
        convertType(base, type1);
        fprintf(fpout, "\tfload 99\n");
        
        if (op == '+')  fprintf(fpout, "\tfadd\n");
        if (op == '-')  fprintf(fpout, "\tfsub\n");
        if (op == '*')  fprintf(fpout, "\tfmul\n");
        if (op == '/')  fprintf(fpout, "\tfdiv\n");
    }
    if (base->baseType == DOUBLE_t) {
        // may need to convert type
        convertType(base, type2);
        fprintf(fpout, "\tdstore 99\n");
        convertType(base, type1);
        fprintf(fpout, "\tdload 99\n");
        
        if (op == '+')  fprintf(fpout, "\tdadd\n");
        if (op == '-')  fprintf(fpout, "\tdsub\n");
        if (op == '*')  fprintf(fpout, "\tdmul\n");
        if (op == '/')  fprintf(fpout, "\tddiv\n");
    }
}

void generateModuloOP(struct ExtType* base) {
    if (base->baseType == INT_t) {
        fprintf(fpout, "\tirem\n");
    }
}

void generateNegativeOP(struct ExtType* base) {
    switch (base->baseType) {
        case INT_t:
            fprintf(fpout, "\tineg\n");
            break;
        case FLOAT_t:
            fprintf(fpout, "\tfneg\n");
            break;
        case DOUBLE_t:
            fprintf(fpout, "\tdneg\n");
            break;
        default:
            break;
    }
}

void generateLogicalOP(struct ExtType* base, const char* op) {
    if (base->baseType == BOOL_t) {
        if (strcmp(op, "&&") == 0)  fprintf(fpout, "\tiand\n");
        if (strcmp(op, "||") == 0)  fprintf(fpout, "\tior\n");
        if (strcmp(op, "!") == 0) {
            // x XOR 1 -> !x (logical)
            fprintf(fpout, "\ticonst_1\n");
            fprintf(fpout, "\tixor\n");
        }  
    }
}

void generateRelationalOP(struct ExtType *type1, struct ExtType *type2, const char* op) {
    if (type1->baseType == BOOL_t && type2->baseType == BOOL_t) {
        if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
            fprintf(fpout, "\tisub\n");

            if (strcmp(op, "==") == 0)  fprintf(fpout, "\tifeq ");
            if (strcmp(op, "!=") == 0)  fprintf(fpout, "\tifne ");

            fprintf(fpout, "LTrue%d\n", branchNum);
            fprintf(fpout, "\ticonst_0\n");
            fprintf(fpout, "\tgoto LFalse%d\n", branchNum);
            fprintf(fpout, "LTrue%d:\n", branchNum);
            fprintf(fpout, "\ticonst_1\n");
            fprintf(fpout, "LFalse%d:\n", branchNum++);
        }
        return ;
    }

    if (checkType(type1, type2, false) == 1) {
        convertType(type1, type2);
        if (type1->baseType == INT_t)        fprintf(fpout, "\tisub\n");
        if (type1->baseType == FLOAT_t)      fprintf(fpout, "\tfcmpl\n");
        if (type1->baseType == DOUBLE_t)     fprintf(fpout, "\tdcmpl\n");
    }
    else if (checkType(type2, type1, false) == 1) {
        if (type2->baseType == FLOAT_t)     fprintf(fpout, "\tfstore 99\n");
        if (type2->baseType == DOUBLE_t)    fprintf(fpout, "\tdstore 99\n");
        convertType(type2, type1);
        if (type2->baseType == INT_t) {
            fprintf(fpout, "\tisub\n");
        }
        if (type2->baseType == FLOAT_t) {
            fprintf(fpout, "\tfstore 99\n");
            fprintf(fpout, "\tfcmpl\n");
        }
        if (type2->baseType == DOUBLE_t) {
            fprintf(fpout, "\tdstore 99\n");
            fprintf(fpout, "\tdcmpl\n");
        }
    }

    if (strcmp(op, "==") == 0)  fprintf(fpout, "\tifeq ");
    if (strcmp(op, "!=") == 0)  fprintf(fpout, "\tifne ");
    if (strcmp(op, "<") == 0)   fprintf(fpout, "\tiflt ");
    if (strcmp(op, "<=") == 0)  fprintf(fpout, "\tifle ");
    if (strcmp(op, ">") == 0)   fprintf(fpout, "\tifgt ");
    if (strcmp(op, ">=") == 0)  fprintf(fpout, "\tifge ");

    fprintf(fpout, "LTrue%d\n", branchNum);
    fprintf(fpout, "\ticonst_0\n");
    fprintf(fpout, "\tgoto LFalse%d\n", branchNum);
    fprintf(fpout, "LTrue%d:\n", branchNum);
    fprintf(fpout, "\ticonst_1\n");
    fprintf(fpout, "LFalse%d:\n", branchNum++);
}
