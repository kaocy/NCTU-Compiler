#include <stdio.h>
#include <string.h>
#include "codeGenerator.h"

void programInitialization() {
    fprintf(fpout, ".class public output\n");
    fprintf(fpout, ".super java/lang/Object\n");
    fprintf(fpout, ".field public static _sc Ljava/util/Scanner;\n\n"); // for read instruction
    
    compareNum = 0;
    ifNum = ifStackPointer = 0;
    memset(ifStack, -1, sizeof(ifStackPointer));
    loopNum = loopStackPointer =0;
    memset(loopStack, -1, sizeof(loopStackPointer));
}

void generateVariableDeclaration(struct SymTable* table, const char* name, struct ExtType* varType) {
    // only for global variable
    if (table->prev == NULL) {
        switch (varType->baseType) {
            case INT_t:
                fprintf(fpout, ".field public static %s I\n", name);
                break;
            case FLOAT_t:
                fprintf(fpout, ".field public static %s F\n", name);
                break;
            case DOUBLE_t:
                fprintf(fpout, ".field public static %s D\n", name);
                break;
            case BOOL_t:
                fprintf(fpout, ".field public static %s Z\n", name);
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
                                fprintf(fpout, "\tgetstatic output/%s I\n", entry->name);
                                return ;
                            case FLOAT_t:
                                fprintf(fpout, "\tgetstatic output/%s F\n", entry->name);
                                return ;
                            case DOUBLE_t:
                                fprintf(fpout, "\tgetstatic output/%s D\n", entry->name);
                                return ;
                            case BOOL_t:
                                fprintf(fpout, "\tgetstatic output/%s Z\n", entry->name);
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
        fprintf(fpout, "\tfstore 98\n");
        convertType(base, type1);
        fprintf(fpout, "\tfload 98\n");
        
        if (op == '+')  fprintf(fpout, "\tfadd\n");
        if (op == '-')  fprintf(fpout, "\tfsub\n");
        if (op == '*')  fprintf(fpout, "\tfmul\n");
        if (op == '/')  fprintf(fpout, "\tfdiv\n");
    }
    if (base->baseType == DOUBLE_t) {
        // may need to convert type
        convertType(base, type2);
        fprintf(fpout, "\tdstore 98\n");
        convertType(base, type1);
        fprintf(fpout, "\tdload 98\n");
        
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

            fprintf(fpout, "Ltrue_%d\n", compareNum);
            fprintf(fpout, "\ticonst_0\n");
            fprintf(fpout, "\tgoto Lfalse_%d\n", compareNum);
            fprintf(fpout, "Ltrue_%d:\n", compareNum);
            fprintf(fpout, "\ticonst_1\n");
            fprintf(fpout, "Lfalse_%d:\n", compareNum++);
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
        if (type2->baseType == FLOAT_t)     fprintf(fpout, "\tfstore 98\n");
        if (type2->baseType == DOUBLE_t)    fprintf(fpout, "\tdstore 98\n");
        convertType(type2, type1);
        if (type2->baseType == INT_t) {
            fprintf(fpout, "\tisub\n");
        }
        if (type2->baseType == FLOAT_t) {
            fprintf(fpout, "\tfstore 98\n");
            fprintf(fpout, "\tfcmpl\n");
        }
        if (type2->baseType == DOUBLE_t) {
            fprintf(fpout, "\tdstore 98\n");
            fprintf(fpout, "\tdcmpl\n");
        }
    }

    if (strcmp(op, "==") == 0)  fprintf(fpout, "\tifeq ");
    if (strcmp(op, "!=") == 0)  fprintf(fpout, "\tifne ");
    if (strcmp(op, "<") == 0)   fprintf(fpout, "\tiflt ");
    if (strcmp(op, "<=") == 0)  fprintf(fpout, "\tifle ");
    if (strcmp(op, ">") == 0)   fprintf(fpout, "\tifgt ");
    if (strcmp(op, ">=") == 0)  fprintf(fpout, "\tifge ");

    fprintf(fpout, "Ltrue_%d\n", compareNum);
    fprintf(fpout, "\ticonst_0\n");
    fprintf(fpout, "\tgoto Lfalse_%d\n", compareNum);
    fprintf(fpout, "Ltrue_%d:\n", compareNum);
    fprintf(fpout, "\ticonst_1\n");
    fprintf(fpout, "Lfalse_%d:\n", compareNum++);
}

void generateVariableInitialization(struct SymTable* table, const char* name, struct ExtType* varType, struct ExtType* valueType, int varNumOffset) {
    // global
    if (table->prev == NULL) {
        switch (varType->baseType) {
            case INT_t:
                fprintf(fpout, "\tputstatic output/%s I\n", name);
                break;
            case FLOAT_t:
                convertType(varType, valueType);
                fprintf(fpout, "\tputstatic output/%s F\n", name);
                break;
            case DOUBLE_t:
                convertType(varType, valueType);
                fprintf(fpout, "\tputstatic output/%s D\n", name);
                break;
            case BOOL_t:
                fprintf(fpout, "\tputstatic output/%s Z\n", name);
                break;
            default:
                break;
        }
    }
    // local
    else {
        switch (varType->baseType) {
            case INT_t:
                fprintf(fpout, "\tistore %d\n", table->nextVarNum + varNumOffset);
                break;
            case FLOAT_t:
                convertType(varType, valueType);
                fprintf(fpout, "\tfstore %d\n", table->nextVarNum + varNumOffset);
                break;
            case DOUBLE_t:
                convertType(varType, valueType);
                fprintf(fpout, "\tdstore %d\n", table->nextVarNum + varNumOffset);
                break;
            case BOOL_t:
                fprintf(fpout, "\tistore %d\n", table->nextVarNum + varNumOffset);
                break;
            default:
                break;
        }
    }
}

void generateVariableAssignment(struct SymTable* table, const char* name, struct ExtType* valueType) {
    // find symbol table again due to level and varNum
    while (table != NULL) {
        struct SymTableNode *entry = table->head;
        while (entry != NULL) {
            if (entry->kind == PARAMETER_t || entry->kind == VARIABLE_t) {
                if (strcmp(entry->name, name) == 0 &&
                    checkType(entry->type, valueType, false) == 1) {
                    if (entry->level == 0) {
                        switch (entry->type->baseType) {
                            case INT_t:
                                fprintf(fpout, "\tputstatic output/%s I\n", entry->name);
                                return ;
                            case FLOAT_t:
                                convertType(entry->type, valueType);
                                fprintf(fpout, "\tputstatic output/%s F\n", entry->name);
                                return ;
                            case DOUBLE_t:
                                convertType(entry->type, valueType);
                                fprintf(fpout, "\tputstatic output/%s D\n", entry->name);
                                return ;
                            case BOOL_t:
                                fprintf(fpout, "\tputstatic output/%s Z\n", entry->name);
                                return ;
                            default:
                                return ;
                        }
                    }
                    else {
                        switch (entry->type->baseType) {
                            case INT_t:
                                fprintf(fpout, "\tistore %d\n", entry->varNum);
                                return ;
                            case FLOAT_t:
                                convertType(entry->type, valueType);
                                fprintf(fpout, "\tfstore %d\n", entry->varNum);
                                return ;
                            case DOUBLE_t:
                                convertType(entry->type, valueType);
                                fprintf(fpout, "\tdstore %d\n", entry->varNum);
                                return ;
                            case BOOL_t:
                                fprintf(fpout, "\tistore %d\n", entry->varNum);
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

void generatePrintStart() {
    fprintf(fpout, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
}

void generatePrintEnd(struct ExtType* type) {
    switch (type->baseType) {
        case INT_t:
            fprintf(fpout, "\tinvokevirtual java/io/PrintStream/print(I)V\n");
            break;
        case FLOAT_t:
            fprintf(fpout, "\tinvokevirtual java/io/PrintStream/print(F)V\n");
            break;
        case DOUBLE_t:
            fprintf(fpout, "\tinvokevirtual java/io/PrintStream/print(D)V\n");
            break;
        case BOOL_t:
            fprintf(fpout, "\tinvokevirtual java/io/PrintStream/print(Z)V\n");
            break;
        case STRING_t:
            fprintf(fpout, "\tinvokevirtual java/io/PrintStream/print(Ljava/lang/String;)V\n");
            break;
        default:
            break;
    }
}

void generateReadStart(struct SymTable* table, const char* name, struct ExtType* varType) {
    fprintf(fpout, "\tgetstatic output/_sc Ljava/util/Scanner;\n");
    switch (varType->baseType) {
        case INT_t:
            fprintf(fpout, "\tinvokevirtual java/util/Scanner/nextInt()I\n");
            break;
        case FLOAT_t:
            fprintf(fpout, "\tinvokevirtual java/util/Scanner/nextFloat()F\n");
            break;
        case DOUBLE_t:
            fprintf(fpout, "\tinvokevirtual java/util/Scanner/nextDouble()D\n");
            break;
        case BOOL_t:
            fprintf(fpout, "\tinvokevirtual java/util/Scanner/nextBoolean()Z\n");
            break;
        default:
            break;
    }
    // find symbol table again due to level and varNum
    while (table != NULL) {
        struct SymTableNode *entry = table->head;
        while (entry != NULL) {
            if (entry->kind == PARAMETER_t || entry->kind == VARIABLE_t) {
                if (strcmp(entry->name, name) == 0) {
                    if (entry->level == 0) {
                        switch (entry->type->baseType) {
                            case INT_t:
                                fprintf(fpout, "\tputstatic output/%s I\n", entry->name);
                                return ;
                            case FLOAT_t:
                                convertType(entry->type, varType);
                                fprintf(fpout, "\tputstatic output/%s F\n", entry->name);
                                return ;
                            case DOUBLE_t:
                                convertType(entry->type, varType);
                                fprintf(fpout, "\tputstatic output/%s D\n", entry->name);
                                return ;
                            case BOOL_t:
                                fprintf(fpout, "\tputstatic output/%s Z\n", entry->name);
                                return ;
                            default:
                                return ;
                        }
                    }
                    else {
                        switch (entry->type->baseType) {
                            case INT_t:
                                fprintf(fpout, "\tistore %d\n", entry->varNum);
                                return ;
                            case FLOAT_t:
                                convertType(entry->type, varType);
                                fprintf(fpout, "\tfstore %d\n", entry->varNum);
                                return ;
                            case DOUBLE_t:
                                convertType(entry->type, varType);
                                fprintf(fpout, "\tdstore %d\n", entry->varNum);
                                return ;
                            case BOOL_t:
                                fprintf(fpout, "\tistore %d\n", entry->varNum);
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

void generateIfStart() {
    fprintf(fpout, "\tifeq Lelse_%d\n", ifNum);
    ifStack[ifStackPointer] = ifNum;
    ifStackPointer++;
    ifNum++;
}

void generateElseStart() {
    fprintf(fpout, "\tgoto Lexit_%d\n", ifStack[ifStackPointer - 1]);
    fprintf(fpout, "Lelse_%d:\n", ifStack[ifStackPointer - 1]);
}

void generateIfEnd() {
    fprintf(fpout, "Lexit_%d:\n", ifStack[ifStackPointer - 1]);
    ifStackPointer--;
    ifStack[ifStackPointer] = -1;
}

void generateLoopStart() {
    fprintf(fpout, "Lbegin_%d:\n", loopNum);
    loopStack[loopStackPointer] = loopNum;
    loopStackPointer++;
    loopNum++;
}

void generateLoopControl() {
    fprintf(fpout, "\tifeq Lend_%d\n", loopNum - 1);
    fprintf(fpout, "\tgoto Lcontent_%d\n", loopNum - 1);
    fprintf(fpout, "Lincrement_%d:\n", loopNum - 1);
}

void generateLoopIncrement() {
    fprintf(fpout, "\tgoto Lbegin_%d\n", loopNum - 1);
    fprintf(fpout, "Lcontent_%d:\n", loopNum - 1);
}

void generateLoopEnd() {
    fprintf(fpout, "\tgoto Lincrement_%d\n", loopStack[loopStackPointer - 1]);
    fprintf(fpout, "Lend_%d:\n", loopStack[loopStackPointer - 1]);
    loopStackPointer--;
    loopStack[loopStackPointer] = -1;
}

void generateDoWhileControl() {
    fprintf(fpout, "\tifeq Lend_%d\n", loopStack[loopStackPointer - 1]);
    fprintf(fpout, "\tgoto Lbegin_%d\n", loopStack[loopStackPointer - 1]);
    fprintf(fpout, "Lend_%d:\n", loopStack[loopStackPointer - 1]);
    loopStackPointer--;
    loopStack[loopStackPointer] = -1;
}

void generateFunctionStart(struct ExtType* returnType, struct FuncAttrNode* paramHead, const char* name, bool isEnrtyFunc) {
    fprintf(fpout, ".method public static %s(", name);

    if (paramHead != NULL) {
        while (paramHead != NULL) {
            switch (paramHead->value->baseType) {
                case INT_t:
                    fprintf(fpout, "I");
                    break;
                case FLOAT_t:
                    fprintf(fpout, "F");
                    break;
                case DOUBLE_t:
                    fprintf(fpout, "D");
                    break;
                case BOOL_t:
                    fprintf(fpout, "Z");
                    break;
                default:
                    break;
            }
            paramHead = paramHead->next;
        }
    }
    else if (isEnrtyFunc) {
        fprintf(fpout, "[Ljava/lang/String;");
    }

    fprintf(fpout, ")");

    if (isEnrtyFunc) {
        fprintf(fpout, "V\n");
    }
    else {
        switch (returnType->baseType) {
            case INT_t:
                fprintf(fpout, "I\n");
                break;
            case FLOAT_t:
                fprintf(fpout, "F\n");
                break;
            case DOUBLE_t:
                fprintf(fpout, "D\n");
                break;
            case BOOL_t:
                fprintf(fpout, "Z\n");
                break;
            case VOID_t:
                fprintf(fpout, "V\n");
                break;
            default:
                break;
        }
    }

    fprintf(fpout, "\t.limit stack 100\n");
    fprintf(fpout, "\t.limit locals 100\n");

    // for read instruction
    fprintf(fpout, "\tnew java/util/Scanner\n");
    fprintf(fpout, "\tdup\n");
    fprintf(fpout, "\tgetstatic java/lang/System/in Ljava/io/InputStream;\n");
    fprintf(fpout, "\tinvokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V\n");
    fprintf(fpout, "\tputstatic output/_sc Ljava/util/Scanner;\n");
}

void generateFunctionEnd(bool isVoid) {
    if (isVoid) fprintf(fpout, "\treturn\n");
    fprintf(fpout, ".end method\n");
}
