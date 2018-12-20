#include "datatype.h"

extern int linenum;

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
struct SymTableNode* findRepeatDeclaration(struct SymTable* table, const char* name);
struct SymTableNode* findFuncDeclaration(struct SymTable* table, const char* name);

struct SymTableNode* createVariableNode(const char* name, int level, struct ExtType* type);
struct SymTableNode* createFunctionNode(const char* name, int level, struct ExtType* type, struct Attribute* attr);
struct SymTableNode* createConstNode(const char* name, int level, struct ExtType* type, struct Attribute* attr);
struct SymTableNode* createParameterNode(const char* name, int level, struct ExtType* type);

struct VariableList* createVariableList(struct Variable* head);
int deleteVariableList(struct VariableList* list);
int connectVariableList(struct VariableList* list, struct Variable* node);
struct Variable* createVariable(const char* name, struct ExtType* type);
struct Variable* deleteVariable(struct Variable* target);

struct Attribute* createConstantAttribute(BTYPE type,void* value);
struct Attribute* createFunctionAttribute(struct FuncAttrNode* list);
struct FuncAttrNode* deleteFuncAttrNode(struct FuncAttrNode* target);
int deleteAttribute(struct Attribute* target);
struct FuncAttrNode* createFuncAttrNode(struct ExtType* type, const char* name);
int connectFuncAttrNode(struct FuncAttrNode* head, struct FuncAttrNode* newNode);

struct ExtType* createExtType(BTYPE baseType, bool isArray, struct ArrayDimNode* dimArray);
int deleteExtType(struct ExtType* target);

struct ArrayDimNode* createArrayDimNode(int size);
int connectArrayDimNode(struct ArrayDimNode* head, struct ArrayDimNode* newNode);
struct ArrayDimNode* deleteArrayDimNode(struct ArrayDimNode* target);
