#ifndef __DATATYPE__
#define __DATATYPE__
#include <stdbool.h>

typedef enum{ FUNCTION_t, PARAMETER_t, VARIABLE_t, CONSTANT_t } KIND;
typedef enum{ INT_t, FLOAT_t, DOUBLE_t, BOOL_t, STRING_t, VOID_t } BTYPE;

struct ExtType { // All type, such as scalar and array
    int reference;
    BTYPE baseType;
    bool isArray;
    int dim; // Array type dimension
    struct ArrayDimNode* dimArray;
};

struct Variable {
    int reference;
    char* name;
    struct ExtType* type;
    struct Variable* next;
};

struct VariableList {
    int reference;
    struct Variable* head;
    struct Variable* tail;
};

struct ArrayDimNode {
    int reference;
    int size;
    struct ArrayDimNode* next;
};

struct ConstAttr {
    int reference;
    BTYPE type;
    union {
        int integerVal;
        float floatVal;
        double doubleVal;
        bool boolVal;
        char* stringVal;
    } value;
    bool hasMinus; // negative value;
};

struct FuncAttrNode {
    int reference;
    struct ExtType* value; // parameter type
    char* name;
    struct FuncAttrNode* next; // single link list
};

struct FuncAttr {
    int reference;
    int paramNum; // parameter numbers
    struct FuncAttrNode* head;
};

struct Attribute {
    int reference;
    struct ConstAttr *constVal; // store const value in attribute
    struct FuncAttr *funcParam; // store function parameter in attribute
};

struct SymTableNode {
    int reference;
    char name[33]; // less than 33 character
    KIND kind;
    int level;
    struct ExtType* type;
    struct Attribute* attr;
    struct SymTableNode* next;
    bool hasDefine; // for function
};

struct SymTable {
    int reference;
    struct SymTableNode* head;
    struct SymTableNode* tail;
    struct SymTable* next;
    struct SymTable* prev;
};

struct SymTableList {
    int reference;
    struct SymTable* head;
    struct SymTable* tail;
    struct SymTable* global; // always same as head
};

#endif
