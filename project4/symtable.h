#include "datatype.h"

extern int linenum;
extern int error;

int printSymTable(struct SymTable* table);
int printType(struct ExtType* extType);
int printConstAttribute(struct ConstAttr* constAttr);
int printParamAttribute(struct FuncAttr* funcAttr);

int initSymTableList(struct SymTableList *list);
int destroySymTableList(struct SymTableList *list);

int AddSymTable(struct SymTableList* list);
int deleteLastSymTable(struct SymTableList* list);
struct SymTable* deleteSymTable(struct SymTable* target);

int insertTableNode(struct SymTable *table, struct SymTableNode* newNode);
struct SymTableNode* deleteTableNode(struct SymTableNode* target);

struct SymTableNode* createVariableNode(const char* name, int level, struct ExtType* type);
struct SymTableNode* createFunctionNode(const char* name, int level, struct ExtType* type, struct Attribute* attr, bool hasDefine);
struct SymTableNode* createConstNode(const char* name, int level, struct ExtType* type, struct Attribute* attr);
struct SymTableNode* createParameterNode(const char* name, int level, struct ExtType* type);

struct VariableList* createVariableList(struct Variable* head);
int deleteVariableList(struct VariableList* list);
int connectVariableList(struct VariableList* list, struct Variable* node);
struct Variable* createVariable(const char* name, struct ExtType* type, struct ExtType* valueType, struct ExtType* initArrayHead);
struct Variable* deleteVariable(struct Variable* target);

struct Attribute* createConstantAttribute(BTYPE type,void* value);
struct Attribute* createFunctionAttribute(struct FuncAttrNode* list);
struct FuncAttrNode* deleteFuncAttrNode(struct FuncAttrNode* target);
int deleteAttribute(struct Attribute* target);
struct FuncAttrNode* createFuncAttrNode(struct ExtType* type, const char* name);
int connectFuncAttrNode(struct FuncAttrNode* head, struct FuncAttrNode* newNode);

struct ExtType* createExtType(BTYPE baseType, bool isArray, struct ArrayDimNode* dimArray, KIND varKind);
int connectExtType(struct ExtType* head, struct ExtType* newType);
int deleteExtType(struct ExtType* target);
int deleteExtTypeList(struct ExtType* head);

struct ArrayDimNode* createArrayDimNode(int size);
int connectArrayDimNode(struct ArrayDimNode* head, struct ArrayDimNode* newNode);
struct ArrayDimNode* deleteArrayDimNode(struct ArrayDimNode* target);

struct SymTableNode* findRepeatDeclaration(struct SymTable* table, const char* name);
struct SymTableNode* findFuncDeclaration(struct SymTable* table, const char* name, struct ExtType* type, struct Attribute* attr);
struct ExtType* findFuncForInvocation(struct SymTable* table, const char* name, struct ExtType* head);
struct ExtType* findVariable(struct SymTable* table, const char* name, int dimension_num);

struct ExtType* arithmeticOP(struct ExtType* type1, struct ExtType* type2, const char op);
struct ExtType* moduloOP(struct ExtType* type1, struct ExtType* type2);
struct ExtType* logicalOP(struct ExtType* type1, struct ExtType* type2, const char* op);
struct ExtType* logicalNotOP(struct ExtType* type1);
struct ExtType* relationalOP(struct ExtType* type1, struct ExtType* type2, const char* op);

int checkType(struct ExtType* type1, struct ExtType* type2, bool exact);
int checkParameterForDefinition(struct FuncAttr* param1, struct FuncAttr* param2);

void checkFunctionReturn(bool hasReturn);
void checkFunctionReturnType(struct ExtType* type1, struct ExtType* type2);
void checkUndeclaraFunction(struct SymTable* table);
void checkInLoop(int inLoop, char *statement);
void checkConditionalExpression(struct ExtType* type);
void checkControlExpression(struct ExtType* type);
void checkScalarType(struct ExtType* type);
void checkAssignType(struct ExtType* type1, struct ExtType* type2);
void checkVariableInitialization(struct ExtType* type1, struct ExtType* type2);
void checkConstantInitialization(BTYPE baseType1, BTYPE baseType2);
void checkArrayInitialization(struct ExtType* type, struct ExtType* typehead);
void checkArrayIndex(struct ExtType* type);
