#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"

int printSymTable(struct SymTable* table) {
    struct SymTableNode *entry;
    struct ArrayDimNode *dimNode;
    struct Attribute *attr;
    struct FuncAttrNode *funcAttrNode;
    char strBuffer[32];

    if (table == NULL)    return -1;
    if (table->head == NULL)    return 1; // no entry to output
    printf("======================================================================================\n");
    // Name [29 blanks] Kind [7 blanks] Level [7 blank] Type [15 blanks] Attribute [15 blanks]
    printf("%-33s%-11s%-12s%-19s%-24s\n", "Name", "Kind", "Level", "Type", "Attribute");
    printf("--------------------------------------------------------------------------------------\n");
    entry = table->head;
    while (entry != NULL) {
        // name
        printf("%-32s ",entry->name);
        // kind
        switch (entry->kind) {
            case VARIABLE_t:
                printf("%-11s", "variable");
                break;
            case CONSTANT_t:
                printf("%-11s", "constant");
                break;
            case FUNCTION_t:
                printf("%-11s", "function");
                break;
            case PARAMETER_t:
                printf("%-11s", "parameter");
                break;
            default:
                printf("%-11s", "unknown");
                break;
        }
        // level
        if (entry->level == 0) {
            printf("%-12s", "0(global) ");
        }
        else {
            sprintf(strBuffer, "%d(local)  ", entry->level);
            printf("%-12s", strBuffer);
        }
        // type
        printType(entry->type);
        // attribute
        attr = entry->attr;
        if (attr != NULL) {
            if (attr->constVal != NULL)
                printConstAttribute(attr->constVal);
            if (attr->funcParam != NULL)
                printParamAttribute(attr->funcParam);
        }
        entry = entry->next;
        printf("\n");
    }
    printf("======================================================================================\n");
}

int printType(struct ExtType* extType) {
    struct ArrayDimNode *dimNode;
    char strBuffer[20];
    char strTemp[20];
    if (extType == NULL) return -1;
    memset(strBuffer, 0, 20 * sizeof(char));
    switch (extType->baseType) {
        case INT_t:
            strncpy(strBuffer, "int", 3);
            break;
        case FLOAT_t:
            strncpy(strBuffer, "float", 5);
            break;
        case DOUBLE_t:
            strncpy(strBuffer, "double", 6);
            break;
        case BOOL_t:
            strncpy(strBuffer, "bool", 4);
            break;
        case STRING_t:
            strncpy(strBuffer, "string", 6);
            break;
        case VOID_t:
            strncpy(strBuffer, "void", 4);
            break;
        default:
            strncpy(strBuffer, "unknown", 7);
            break;
    }

    if (extType->isArray) {
        dimNode = extType->dimArray;
        while (dimNode != NULL) {
            memset(strTemp, 0, 20 * sizeof(char));
            sprintf(strTemp, "[%d]", dimNode->size);
            if(strlen(strBuffer) + strlen(strTemp) < 20) {
                strcat(strBuffer,strTemp);
            }
            else {
                strBuffer[16] = '.';
                strBuffer[17] = '.';
                strBuffer[18] = '.';
            }
            dimNode = dimNode->next;
        }
    }
    printf("%-19s", strBuffer);
    return 0;
}

int printConstAttribute(struct ConstAttr* constAttr) {
    switch (constAttr->type) {
        case INT_t:
            printf("%d", constAttr->value.integerVal);
            break;
        case FLOAT_t:
            printf("%f", constAttr->value.floatVal);
            break;
        case DOUBLE_t:
            printf("%lf", constAttr->value.doubleVal);
            break;
        case BOOL_t:
            if (constAttr->value.boolVal)
                printf("true");
            else
                printf("false");
            break;
        case STRING_t:
            printf("%s", constAttr->value.stringVal);
            break;
        default:
            printf("__ERROR__");
            break;
    }
    return 0;
}

int printParamAttribute(struct FuncAttr* funcAttr) {
    struct FuncAttrNode* attrNode = funcAttr->head;
    struct ArrayDimNode* dimNode;
    if (attrNode != NULL) {
        switch (attrNode->value->baseType) {
            case INT_t:
                printf("int");
                break;
            case FLOAT_t:
                printf("float");
                break;
            case DOUBLE_t:
                printf("double");
                break;
            case BOOL_t:
                printf("bool");
                break;
            case STRING_t:
                printf("string");
                break;
            case VOID_t:
                printf("void");
                break;
            default:
                printf("unknown");
                break;
        }
        if (attrNode->value->isArray) {
            dimNode = attrNode->value->dimArray;
            while (dimNode != NULL) {
                printf("[%d]", dimNode->size);
                dimNode = dimNode->next;
            }
        }
        attrNode = attrNode->next;
        while (attrNode != NULL) {
            switch (attrNode->value->baseType) {
                case INT_t:
                    printf(",int");
                    break;
                case FLOAT_t:
                    printf(",float");
                    break;
                case DOUBLE_t:
                    printf(",double");
                    break;
                case BOOL_t:
                    printf(",bool");
                    break;
                case STRING_t:
                    printf(",string");
                    break;
                case VOID_t:
                    printf(",void");
                    break;
                default:
                    printf(",unknown");
                    break;
            }
            if (attrNode->value->isArray) {
                dimNode = attrNode->value->dimArray;
                while (dimNode != NULL) {
                    printf("[%d]", dimNode->size);
                    dimNode = dimNode->next;
                }
            }
            attrNode = attrNode->next;
        }
    }
    return 0;
}

int initSymTableList(struct SymTableList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->global = NULL;
    list->reference = 1;
    return 0;
}

int destroySymTableList(struct SymTableList *list) {
    list->reference -= 1; // derefence
    if (list->reference > 0)  return -1;
    while (list->head != NULL) {
        // delete head node
        list->head = deleteSymTable(list->head); // return new head
    }
    return 0;
}

int AddSymTable(struct SymTableList* list) { // enter a new scope
    if (list->head == NULL) {
        struct SymTable *newTable = (struct SymTable*)malloc(sizeof(struct SymTable));
        newTable->head = NULL;
        newTable->tail = NULL;
        newTable->next = NULL;
        newTable->prev = NULL;
        list->head = newTable;
        list->tail = list->head;
        list->global = list->head;
        newTable->reference = 1;
    }
    else {
        struct SymTable *newTable = (struct SymTable*)malloc(sizeof(struct SymTable));
        newTable->head = NULL;
        newTable->tail = NULL;
        newTable->next = NULL;
        newTable->prev = list->tail;
        list->tail->next = newTable;
        list->tail = newTable;
        newTable->reference = 1;
    }
    return 0;
}

int deleteLastSymTable(struct SymTableList* list) { // leave scope
    struct SymTable *temp = list->tail;
    if (temp == NULL)   return -1;
    temp->reference -= 1; // derefence
    if (temp->reference > 0) return -1;
    if (list->head != list->tail) {
        temp->prev->next = NULL;
        list->tail = temp->prev;
    }
    else {
        list->tail = NULL;
        list->head = NULL;
    }
    deleteSymTable(temp);
    return 0;
}

struct SymTable* deleteSymTable(struct SymTable* target) {
    struct SymTable *next;
    if (target == NULL) {
        next = NULL;
    }
    else {
        target->reference -= 1;//dereference
        if (target->reference > 0)  return NULL;
        next = target->next;
        while (target->head != NULL) {
            target->head = deleteTableNode(target->head);
        }
    }
    if (next != NULL)   next->prev = NULL;
    return next;
}

int insertTableNode(struct SymTable *table, struct SymTableNode* newNode) {
    if (findRepeatDeclaration(table, newNode->name) != NULL) {
        error = 1;
        printf("Error at Line #%d: '%s' is redeclared.\n", linenum, newNode->name);
        return -1;
    }
    if (table->tail == NULL) {
        table->head = newNode;
        table->tail = newNode;
    }
    else {
        table->tail->next = newNode;
        table->tail = newNode;
    }
    newNode->reference += 1;
    return 0;
}

struct SymTableNode* deleteTableNode(struct SymTableNode* target) { // return next node
    struct SymTableNode *next;
    if (target == NULL) {
        next = NULL;
    }
    else {
        target->reference -= 1; // defreference
        if (target->reference > 0) return NULL;
        next = target->next;
        deleteExtType(target->type);
        if (target->attr != NULL)  deleteAttribute(target->attr);
        free(target);
    }
    return next;
}

struct SymTableNode* createVariableNode(const char* name, int level, struct ExtType* type) {
    struct SymTableNode *newNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));
    //set node
    strncpy(newNode->name, name, 32);
    newNode->kind = VARIABLE_t;
    newNode->level = level;

    newNode->type = type;
    newNode->type->reference += 1;

    newNode->attr = NULL;
    newNode->next = NULL;
    newNode->reference = 0;
    return newNode;
}

struct SymTableNode* createConstNode(const char* name, int level, struct ExtType* type, struct Attribute* attr) {
    struct SymTableNode *newNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));
    //set node
    strncpy(newNode->name, name, 32);
    newNode->kind = CONSTANT_t;
    newNode->level = level;

    newNode->type = type;
    newNode->type->reference += 1;

    newNode->attr = attr;
    if (attr != NULL)  newNode->attr->reference += 1;

    newNode->next = NULL;
    newNode->reference = 0;
    return newNode;
}

struct SymTableNode* createFunctionNode(const char* name, int level, struct ExtType* type, struct Attribute* attr, bool hasDefine) {
    struct SymTableNode *newNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));
    //set node
    strncpy(newNode->name, name, 32);
    newNode->kind = FUNCTION_t;
    newNode->level = level;

    newNode->type = type;
    newNode->type->reference += 1;

    newNode->attr = attr;
    if (attr != NULL)  newNode->attr->reference += 1;

    newNode->hasDefine = hasDefine;

    newNode->next = NULL;
    newNode->reference = 0;
    return newNode;
}

struct SymTableNode* createParameterNode(const char* name, int level, struct ExtType* type) {
    struct SymTableNode *newNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));
    //set node
    strncpy(newNode->name, name, 32);
    newNode->kind = PARAMETER_t;
    newNode->level = level;

    newNode->type = type;
    newNode->type->reference += 1;

    newNode->attr = NULL;
    newNode->next = NULL;
    newNode->reference = 0;
    return newNode;
}

struct VariableList* createVariableList(struct Variable* head) {
    struct VariableList *list;
    if (head == NULL) {
        list = NULL;
    }
    else  {
        list = (struct VariableList*)malloc(sizeof(struct VariableList));
        struct Variable* temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        list->head = head;
        head->reference += 1;
        list->tail = temp;
        if (head != temp) temp->reference += 1;
        list->reference = 0;
    }
    return list;
}

int deleteVariableList(struct VariableList* list) {
    list->reference -= 1;
    if (list->reference > 0) return -1;
    if (list->head != NULL) {
        // list->head = NULL
        list->head->reference -= 1;
        if (list->head != list->tail) {
            // list->tail = NULL
            list->tail->reference -= 1;
        }
        while (list->head != NULL) {
            list->head = deleteVariable(list->head);
        }
    }
    return 0;
}

int connectVariableList(struct VariableList* list, struct Variable* node) {
    if(list == NULL || node == NULL)
        return -1;
    if(node->next != NULL)
        return -2;
    if(list->tail != list->head)
        list->tail->reference -= 1;

    list->tail->next = node;
    list->tail->next->reference += 1;
    list->tail = node;
    list->tail->reference += 1;
    return 0;
}

struct Variable* createVariable(const char* name, struct ExtType* type, struct ExtType* valueType, struct ExtType* initArrayHead) {
    struct Variable* variable = (struct Variable*)malloc(sizeof(struct Variable));
    variable->name = strdup(name);
    variable->type = type;
    type->reference += 1;

    variable->valueType = valueType;
    if (variable->valueType != NULL)    variable->valueType->reference += 1;
    variable->initArrayHead = initArrayHead;
    if (variable->initArrayHead != NULL)    variable->initArrayHead->reference += 1;
    
    variable->next = NULL;
    variable->reference = 0;
    return variable;
}

struct Variable* deleteVariable(struct Variable* target) {
    struct Variable* next;
    if (target == NULL) {
        next = NULL;
    }
    else {
        target->reference -= 1;
        if (target->reference > 0) return NULL;
        free(target->name);
        deleteExtType(target->type);
        deleteExtType(target->valueType);
        deleteExtTypeList(target->initArrayHead);
        next = target->next;
        free(target);
    }
    return next;
}

struct Attribute* createConstantAttribute(BTYPE type, void* value) {
    struct Attribute *newAttr = (struct Attribute*)malloc(sizeof(struct Attribute));
    struct ConstAttr *newConstAttr = (struct ConstAttr*)malloc(sizeof(struct ConstAttr));
    newAttr->funcParam = NULL;
    newAttr->constVal = newConstAttr;
    newConstAttr->reference = 1;
    newConstAttr->hasMinus = false;
    newConstAttr->type = type;

    switch (type) {
        case INT_t:
            newConstAttr->value.integerVal = *(int*)value;
            if (*(int*)value < 0) newConstAttr->hasMinus = true;
            break;
        case FLOAT_t:
            newConstAttr->value.floatVal = *(float*)value;
            if (*(float*)value < 0.0) newConstAttr->hasMinus = true;
            break;
        case DOUBLE_t:
            newConstAttr->value.doubleVal = *(double*)value;
            if (*(double*)value < 0.0) newConstAttr->hasMinus = true;
            break;
        case BOOL_t:
            newConstAttr->value.boolVal = *(bool*)value;
            break;
        case STRING_t:
            newConstAttr->value.stringVal = strdup((char*)value);
            break;
        default:
            break;
    }
    newAttr->reference = 0;
    return newAttr;
}

struct Attribute* createFunctionAttribute(struct FuncAttrNode* list) {
    int num = 0;
    struct Attribute *newAttr = (struct Attribute*)malloc(sizeof(struct Attribute));
    newAttr->constVal = NULL;
    newAttr->funcParam = (struct FuncAttr*)malloc(sizeof(struct FuncAttr));
    newAttr->funcParam->reference = 1;

    newAttr->funcParam->head = list;
    newAttr->funcParam->head->reference += 1;

    while (list != NULL) {
        list = list->next;
        ++num;
    }
    newAttr->funcParam->paramNum = num;
    newAttr->reference = 0;
    return newAttr;
}

int deleteAttribute(struct Attribute* target) {
    target->reference -= 1;
    if (target->reference > 0) return -1;
    if (target->constVal != NULL) {
        target->constVal->reference -= 1;
        if (target->constVal->reference > 0)
            return -1;
        if (target->constVal->type == STRING_t)
            free(target->constVal->value.stringVal);
        free(target->constVal);
    }
    if (target->funcParam != NULL) {
        target->funcParam->reference -= 1;
        if (target->funcParam->reference > 0)
            return -1;
        target->funcParam->head = deleteFuncAttrNode(target->funcParam->head);
        free(target->funcParam);
    }
    free(target);
    return 0;
}

struct FuncAttrNode* createFuncAttrNode(struct ExtType* type, const char* name) {
    struct FuncAttrNode *newNode = (struct FuncAttrNode*)malloc(sizeof(struct FuncAttrNode));
    /*reference*/
    newNode->value = type;
    type->reference += 1;

    newNode->name = strdup(name);
    newNode->next = NULL;
    newNode->reference = 0;
    return newNode;
}

int connectFuncAttrNode(struct FuncAttrNode* head, struct FuncAttrNode* newNode) {
    // connect node to tail of function attribute list
    if (head == NULL || newNode == NULL || head == newNode) return -1;
    struct FuncAttrNode *temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
    newNode->reference += 1;
    return 0;
}

struct FuncAttrNode* deleteFuncAttrNode(struct FuncAttrNode* target) {
    struct FuncAttrNode *next;
    if (target == NULL) {
        next = NULL;
    }
    else {
        target->reference -= 1;
        if (target->reference > 0)  return NULL;
        next = target->next;
        deleteExtType(target->value);
        free(target->name);
        free(target);
    }
    return next;
}

struct ExtType* createExtType(BTYPE baseType, bool isArray, struct ArrayDimNode* dimArray) {
    int dimNum = 0;
    struct ArrayDimNode *temp = dimArray;
    struct ExtType *newExtType = (struct ExtType*)malloc(sizeof(struct ExtType));
    newExtType->baseType = baseType;
    newExtType->isArray = isArray;

    newExtType->dimArray = dimArray;
    if (dimArray != NULL)  dimArray->reference += 1;

    newExtType->reference = 0;
    while (temp != NULL) {
        temp = temp->next;
        ++dimNum;
    }
    newExtType->dim = dimNum;
    newExtType->next = NULL;
    return newExtType;
}

int connectExtType(struct ExtType* head, struct ExtType* newType) {
    // connect dimension node to tail of list
    if (head == NULL || newType == NULL || head == newType) return -1;
    struct ExtType *temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newType;
    newType->reference += 1;
    return 0;
}

int deleteExtType(struct ExtType* target) {
    if (target == NULL) return -1;
    target->reference -= 1;
    if (target->reference > 0) return -1;
    if (target->isArray) {
        while (target->dimArray != NULL) {
            target->dimArray = deleteArrayDimNode(target->dimArray);
        }
    }
    free(target);
    return 0;
}

int deleteExtTypeList(struct ExtType* head) {
    while (head != NULL) {
        struct ExtType* next = head->next;
        deleteExtType(head);
        head = next;
    }
    return 0;
}

struct ArrayDimNode* createArrayDimNode(int size) {
    struct ArrayDimNode *newNode = (struct ArrayDimNode*)malloc(sizeof(struct ArrayDimNode));
    newNode->size = size;
    newNode->next = NULL;
    newNode->reference = 0;
    if (size <= 0) {
        error = 1;
        printf("Error at Line #%d: The index must be greater than zero in array declaration.\n", linenum);
    }
    return newNode;
}

int connectArrayDimNode(struct ArrayDimNode* head, struct ArrayDimNode* newNode) {
    // connect dimension node to tail of list
    if (head == NULL || newNode == NULL || head == newNode) return -1;
    struct ArrayDimNode *temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
    newNode->reference += 1;
    return 0;
}

struct ArrayDimNode* deleteArrayDimNode(struct ArrayDimNode* target) {
    struct ArrayDimNode *next;
    if (target == NULL) {
        next = NULL;
    }
    else {
        target->reference -= 1;
        if (target->reference > 0) return NULL;
        next = target->next;
        free(target);
    }
    return next;
}

struct SymTableNode* findRepeatDeclaration(struct SymTable* table, const char* name) {
    struct SymTableNode *temp = table->head;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) return temp;
        temp = temp->next;
    }
    return NULL;
}

struct SymTableNode* findFuncDeclaration(struct SymTable* table, const char* name, struct ExtType* type, struct Attribute* attr) {
    struct SymTableNode *temp = table->head;
    while (temp != NULL) {
        if (temp->kind == FUNCTION_t) {
            if (strcmp(temp->name, name) == 0) {
                if (temp->hasDefine) {
                    error = 1;
                    printf("Error at Line #%d: '%s' is redeclared.\n", linenum, name);
                }
                if (!checkType(temp->type, type, true)) {
                    error = 1;
                    printf("Error at Line #%d: Function '%s' has inconsistent type to its declaration.\n", linenum, name);
                }
                if (temp->attr == NULL && attr == NULL) {
                    temp->hasDefine = true;
                    return temp;
                }
                if ((temp->attr == NULL || attr == NULL) ||
                    !checkParameterForDefinition(temp->attr->funcParam, attr->funcParam)) {
                    error = 1;
                    printf("Error at Line #%d: Function '%s' has inconsistent parameter to its declaration.\n", linenum, name);
                }
                temp->hasDefine = true;
                return temp;
            }
        }
        temp = temp->next;
    }
    return NULL;
}

struct ExtType* findFuncForInvocation(struct SymTable* table, const char* name, struct ExtType* head) {
    struct SymTableNode *temp = table->head;
    while (temp != NULL) {
        if (temp->kind == FUNCTION_t) {
            if (strcmp(temp->name, name) == 0) {
                // for those functions with no parameters
                if (temp->attr == NULL) {
                    if (head != NULL) {
                        error = 1;
                        printf("Error at Line #%d: The number of parameters must be identical to the function declaration/definition.\n", linenum);
                    }
                }
                else {
                    struct FuncAttrNode* param = temp->attr->funcParam->head;
                    while (param != NULL && head != NULL) {
                        if (!checkType(param->value, head, false)) {
                            error = 1;
                            printf("Error at Line #%d: The type of parameters must be identical to the function declaration/definition.\n", linenum);
                        }
                        param = param->next;
                        head = head->next;
                    }
                    if (param != NULL || head != NULL) {
                        error = 1;
                        printf("Error at Line #%d: The number of parameters must be identical to the function declaration/definition.\n", linenum);
                    }
                }
                return createExtType(temp->type->baseType, false, NULL);
            }
        }
        temp = temp->next;
    }
    error = 1;
    printf("Error at Line #%d: '%s' has not been declared.\n", linenum, name);
    return createExtType(ERROR_t, false, NULL);
}

struct ExtType* findVariable(struct SymTable* table, const char* name, int dimension_num, bool* isConst) {
    while (table != NULL) {
        struct SymTableNode *temp = table->head;
        while (temp != NULL) {
            if (temp->kind == CONSTANT_t) {
                if (strcmp(temp->name, name) == 0) {
                    *(isConst) = true;
                    struct ExtType* type = temp->type;
                    return createExtType(type->baseType, false, NULL);
                }
            }
            if (temp->kind == PARAMETER_t || temp->kind == VARIABLE_t) {
                if (strcmp(temp->name, name) == 0) {
                    *(isConst) = false;
                    struct ExtType* type = temp->type;
                    if (type->isArray) {
                        struct ArrayDimNode* dim = type->dimArray;
                        // add reference for new type
                        while (dim != NULL) {
                            dim->reference += 1;
                            dim = dim->next;
                        }
                        // check array dimension number
                        dim = type->dimArray;
                        while (dim != NULL && dimension_num > 0) {
                            dim->reference -= 1;
                            dim = dim->next;
                            --dimension_num;
                        }

                        if (dim != NULL) {
                            return createExtType(type->baseType, true, dim);
                        }
                        else {
                            return createExtType(type->baseType, false, NULL);
                        }
                    }
                    return createExtType(type->baseType, false, NULL);
                }
            }
            temp = temp->next;
        }
        table = table->prev;
    }
    error = 1;
    printf("Error at Line #%d: '%s' has not been declared.\n", linenum, name);
    return createExtType(ERROR_t, false, NULL);
}

struct ExtType* arithmeticOP(struct ExtType* type1, struct ExtType* type2, const char op) {
    if (type1->isArray || type2->isArray) {
        error = 1;
        printf("Error at Line #%d: Array arithmetic is not allowed.\n", linenum);
        return createExtType(ERROR_t, false, NULL);
    }
    if (type1->baseType == BOOL_t || type1->baseType == STRING_t || type1->baseType == VOID_t || type1->baseType == ERROR_t ||
        type2->baseType == BOOL_t || type2->baseType == STRING_t || type2->baseType == VOID_t || type2->baseType == ERROR_t) {
        error = 1;
        printf("Error at Line #%d: The operand's type of '%c' is not valid.\n", linenum, op);
        return createExtType(ERROR_t, false, NULL);
    }
    if (checkType(type1, type2, false)) {
        return createExtType(type1->baseType, type1->isArray, type1->dimArray); 
    }
    if (checkType(type2, type1, false)) {
        return createExtType(type2->baseType, type2->isArray, type2->dimArray); 
    }
    error = 1;
    printf("Error at Line #%d: The operand's type of '%c' is not valid.\n", linenum, op);
    return createExtType(ERROR_t, false, NULL);
}

struct ExtType* moduloOP(struct ExtType* type1, struct ExtType* type2) {
    if (type1->isArray || type2->isArray) {
        error = 1;
        printf("Error at Line #%d: Array arithmetic is not allowed.\n", linenum);
        return createExtType(ERROR_t, false, NULL);
    }
    if (type1->baseType != INT_t || type2->baseType != INT_t) {
        error = 1;
        printf("Error at Line #%d: The operand's type of '%%' is not valid.\n", linenum);
        return createExtType(ERROR_t, false, NULL);
    }
    return createExtType(INT_t, false, NULL);
}

struct ExtType* logicalOP(struct ExtType* type1, struct ExtType* type2, const char* op) {
    if (type1->isArray || type2->isArray) {
        error = 1;
        printf("Error at Line #%d: Array arithmetic is not allowed.\n", linenum);
        return createExtType(ERROR_t, false, NULL);
    }
    if (type1->baseType != BOOL_t || type2->baseType != BOOL_t) {
        error = 1;
        printf("Error at Line #%d: The operand's type of '%s' is not valid.\n", linenum, op);
        return createExtType(ERROR_t, false, NULL);
    }
    return createExtType(BOOL_t, false, NULL);
}

struct ExtType* logicalNotOP(struct ExtType* type1) {
    if (type1->isArray) {
        error = 1;
        printf("Error at Line #%d: Array arithmetic is not allowed.\n", linenum);
        return createExtType(ERROR_t, false, NULL);
    }
    if (type1->baseType != BOOL_t) {
        error = 1;
        printf("Error at Line #%d: The operand's type of '!' is not valid.\n", linenum);
        return createExtType(ERROR_t, false, NULL);
    }
    return createExtType(BOOL_t, false, NULL);
}

struct ExtType* relationalOP(struct ExtType* type1, struct ExtType* type2, const char* op) {
    if (type1->isArray || type2->isArray) {
        error = 1;
        printf("Error at Line #%d: Array arithmetic is not allowed.\n", linenum);
        return createExtType(ERROR_t, false, NULL);
    }
    if (type1->baseType == STRING_t || type1->baseType == VOID_t || type1->baseType == ERROR_t ||
        type2->baseType == STRING_t || type2->baseType == VOID_t || type2->baseType == ERROR_t) {
        error = 1;
        printf("Error at Line #%d: The operand's type of '%s' is not valid.\n", linenum, op);
        return createExtType(ERROR_t, false, NULL);
    }
    if (type1->baseType == BOOL_t && type2->baseType == BOOL_t) {
        if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
            return createExtType(BOOL_t, false, NULL);
        }
        else {
            error = 1;
            printf("Error at Line #%d: The operand's type of '%s' is not valid.\n", linenum, op);
            return createExtType(ERROR_t, false, NULL);
        }
    }
    if (checkType(type1, type2, false)) {
        return createExtType(BOOL_t, false, NULL);
    }
    if (checkType(type2, type1, false)) {
        return createExtType(BOOL_t, false, NULL); 
    }
    error = 1;
    printf("Error at Line #%d: The operand's type of '%s' is not valid.\n", linenum, op);
    return createExtType(ERROR_t, false, NULL);
}

// exact is for exactly equal or need type coercion
int checkType(struct ExtType* type1, struct ExtType* type2, bool exact) {
    if (type1->baseType == ERROR_t || type2->baseType == ERROR_t)   return 0;
    if (type1->isArray != type2->isArray)   return 0;

    struct ArrayDimNode* head1 = type1->dimArray;
    struct ArrayDimNode* head2 = type2->dimArray;
    while(head1 != NULL && head2 != NULL) {
        if (head1->size != head2->size) return 0;
        head1 = head1->next;
        head2 = head2->next;
    }
    if (head1 != NULL || head2 != NULL) return 0;

    if (exact) {
        if (type1->baseType != type2->baseType) return 0;
    }
    else {
        if (type1->baseType == DOUBLE_t) {
            if (type2->baseType != DOUBLE_t &&
                type2->baseType != FLOAT_t &&
                type2->baseType != INT_t)   return 0;
        }
        else if (type1->baseType == FLOAT_t) {
            if (type2->baseType != FLOAT_t &&
                type2->baseType != INT_t)   return 0;
        }
        else {
            if (type1->baseType != type2->baseType) return 0;
        }
    }

    return 1;
}

int checkParameterForDefinition(struct FuncAttr* param1, struct FuncAttr* param2) {
    if (param1 == NULL && param2 == NULL)   return 1;
    if (param1 == NULL || param2 == NULL)   return 0;

    if (param1->paramNum != param2->paramNum)   return 0;
    struct FuncAttrNode* head1 = param1->head;
    struct FuncAttrNode* head2 = param2->head;
    while(head1 != NULL && head2 != NULL) {
        if (!checkType(head1->value, head2->value, true)) return 0;
        head1 = head1->next;
        head2 = head2->next;
    }
    if (head1 != NULL || head2 != NULL) return 0;
    return 1;
}

void checkFunctionReturn(bool hasReturn) {
    if (!hasReturn) {
        error = 1;
        printf("Error at Line #%d: Non-void type function should have return in the last line.\n", linenum);
    }
}

void checkFunctionReturnType(struct ExtType* type1, struct ExtType* type2) {
    if (!checkType(type1, type2, false)) {
        error = 1;
        printf("Error at Line #%d: The type of return statement must match the function type.\n", linenum);
    }
}

void checkUndeclaraFunction(struct SymTable* table) {
    struct SymTableNode *temp = table->head;
    while (temp != NULL) {
        if (temp->kind == FUNCTION_t) {
            if (!temp->hasDefine) {
                error = 1;
                printf("Error at Line #%d: Function '%s' is not defined.\n", linenum, temp->name);
            }
        }
        temp = temp->next;
    }
}

void checkInLoop(int inLoop, char *statement) {
    if (inLoop <= 0) {
        error = 1;
        printf("Error at Line #%d: '%s' can only appear in loop statements.\n", linenum, statement);
    }
}

void checkConditionalExpression(struct ExtType* type) {
    if (type->baseType != BOOL_t) {
        error = 1;
        printf("Error at Line #%d: Conditional expression must be Boolean type.\n", linenum);
    }
}

void checkControlExpression(struct ExtType* type) {
    if (type->baseType != BOOL_t) {
        error = 1;
        printf("Error at Line #%d: Control expression must be Boolean type.\n", linenum);
    }
}

void checkScalarType(struct ExtType* type) {
    // for print and read statement
    if (type->isArray) {
        error = 1;
        printf("Error at Line #%d: Variable references in print and read statements must be scalar type.\n", linenum);
    }
}

void checkAssignType(struct ExtType* type1, struct ExtType* type2, bool isConst) {
    if (isConst) {
        error = 1;
        printf("Error at Line #%d: Re-assignment to constant is not allowed.\n", linenum);
    }
    if (type1->isArray || type2->isArray) {
        error = 1;
        printf("Error at Line #%d: Array assignment is not allowed.\n", linenum);
    }
    else if (!checkType(type1, type2, false)) {
        error = 1;
        printf("Error at Line #%d: The operand's type of '=' is not valid.\n", linenum);
    }
}

void checkVariableInitialization(struct ExtType* type1, struct ExtType* type2) {
    if (!checkType(type1, type2, false)) {
        error = 1;
        printf("Error at Line #%d: The type of variable initial value in not valid.\n", linenum);
    }
}

void checkConstantInitialization(BTYPE baseType1, BTYPE baseType2) {
    struct ExtType* type1 = createExtType(baseType1, false, NULL);
    struct ExtType* type2 = createExtType(baseType2, false, NULL);
    type1->reference += 1;
    type2->reference += 1;
    if (!checkType(type1, type2, false)) {
        error = 1;
        printf("Error at Line #%d: The type of constant initial value in not valid.\n", linenum);
    }
    deleteExtType(type1);
    deleteExtType(type2);
}

void checkArrayInitialization(struct ExtType* type, struct ExtType* typehead) {
    int element_num = 1;
    struct ArrayDimNode* temp = type->dimArray;
    // get the max element number of array
    while (temp != NULL) {
        element_num *= temp->size;
        temp = temp->next;
    }

    temp = type->dimArray;
    // temporarily change due to array variable with isArray and dimArray
    type->isArray = false;
    type->dimArray = NULL;
    while (typehead != NULL) {
        if (!checkType(type, typehead, false)) {
            error = 1;
            printf("Error at Line #%d: The type of element in initial array in not valid.\n", linenum);
        }
        --element_num;
        if (element_num < 0) {
            error = 1;
            printf("Error at Line #%d: The number of element in initial array must be equal to or less than the array size.\n", linenum);
        }
        typehead = typehead->next;
    }
    // recover
    type->isArray = true;
    type->dimArray = temp;
}

void checkArrayIndex(struct ExtType* type) {
    if (type->baseType != INT_t) {
        error = 1;
        printf("Error at Line #%d: The index of array reference must be integer type.\n", linenum);
    }
}
