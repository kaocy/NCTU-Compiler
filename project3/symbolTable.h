
// from scanner.l
extern int Opt_Symbol;
extern int linenum;

typedef struct SymbolTable SymbolTable;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct Type Type;
typedef struct TypeList TypeList;
typedef struct Attribute Attribute;
typedef struct ArraySignature ArraySignature;
typedef struct Value Value;
typedef struct IdList IdList;

struct SymbolTable {
    int size, level;
    SymbolTableEntry *entry_head, *entry_tail;
    SymbolTable *prev, *next;
};

struct SymbolTableEntry {
    char name[33];         // length between 1 to 32
    char kind[10];         // function, parameter, variable, and constant
    int level;             // scope level of symbol
    Type *type;            // int, float, double, bool, string, void, signature of array
    Attribute *attribute;  // value of constant or type of parameters
    SymbolTableEntry *prev, *next;
};

struct Type {
    char name[10];
    ArraySignature *array_signature;
};

struct TypeList {
    Type *type;
    TypeList *prev, *next;
};

struct Attribute {
    TypeList *typelist; // for function parameters
    Value *value;
};

struct ArraySignature {
    int size;
    ArraySignature *prev, *next;
};

struct Value {
    Type *type;
    int ivalue;
    double dvalue;
    char *svalue;
};

struct IdList {
    char *name;
    ArraySignature *array_signature;
    Value *value;
    IdList *prev, *next;
};

void PrintTable(SymbolTable*);
void PrintEntry(SymbolTableEntry*);
void PrintLevel(int);
void PrintType(Type*);
void PrintAttribute(Attribute*);
SymbolTableEntry *FindEntry(SymbolTable*, const char*);
SymbolTable* InsertTable(SymbolTable*, int);
SymbolTableEntry *InsertEntryFromId(SymbolTable*, IdList*, char*, Type*);
IdList *InsertIdList(IdList*, char*, ArraySignature*, Value*);
ArraySignature *InsertArraySignature(ArraySignature*, const int);
void DeleteTable(SymbolTable*);
void DeleteEntry(SymbolTableEntry*);
void DeleteType(Type*);
void DeleteAttribute(Attribute*);
void DeleteIdList(IdList*);
Type *CreateType(const char*, ArraySignature*);
Attribute *CreateAttribute(TypeList*, Value*);
Value *CreateValue(const char*, char*);
