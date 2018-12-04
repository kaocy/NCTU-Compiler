#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"

void PrintTable(SymbolTable *table) {
    if (!Opt_Symbol) return ;
    // Name [29 blanks] Kind [7 blanks] Level [7 blank] Type [15 blanks] Attribute [15 blanks]
    printf("=======================================================================================\n");
    printf("%-33s%-11s%-12s%-19s%-24s\n", "Name", "Kind", "Level", "Type", "Attribute");
    printf("---------------------------------------------------------------------------------------\n");
    
    SymbolTableEntry *cur = table->entry_head;
    while (cur != NULL) {
        PrintEntry(cur);
        cur = cur->next;
    }

    printf("=======================================================================================\n");
}

void PrintEntry(SymbolTableEntry *entry) {
    printf("%-33s%-11s", entry->name, entry->kind);
    PrintLevel(entry->level);
    PrintType(entry->type);
    PrintAttribute(entry->attribute);
    printf("\n");
}

void PrintLevel(int level) {
    char str_level[15];
    sprintf(str_level, "%d", level);
    if (level)  strcat(str_level, "(local)");
    else        strcat(str_level, "(global)");
    printf("%-12s", str_level);
}

void PrintType(Type *type) {
    char str_type[20];
    strcpy(str_type, type->name);

    ArraySignature *cur = type->array_signature;
    while (cur != NULL) {
        char size[20];
        sprintf(size, "%d", cur->size);
        strcat(str_type, "[");
        strcat(str_type, size);
        strcat(str_type, "]");
        cur = cur->next;
    }
    printf("%-19s", str_type);
}

void PrintAttribute(Attribute *attr) {
    if (attr == NULL)  return ;
    if (attr->value != NULL) {
        if (strcmp(attr->value->type->name, "int") == 0) {
            printf("%-24d", attr->value->ivalue);
        }
        else if (strcmp(attr->value->type->name, "float") == 0) {
            printf("%-24lf", attr->value->dvalue);
        }
        else if (strcmp(attr->value->type->name, "scientific") == 0) {
            printf("%-24s", attr->value->svalue);
        }
        else if (strcmp(attr->value->type->name, "string") == 0) {
            printf("%-24s", attr->value->svalue);
        }
        else if (strcmp(attr->value->type->name, "bool") == 0) {
            printf("%-24s", attr->value->svalue);
        }
        return ;
    }

    int count = 0;
    ParameterList *parameterlist = attr->parameterlist;
    while (parameterlist != NULL) {
        char str_type[20];
        strcpy(str_type, parameterlist->type->name);

        ArraySignature *cur = parameterlist->type->array_signature;
        while (cur != NULL) {
            char size[20];
            sprintf(size, "%d", cur->size);
            strcat(str_type, "[");
            strcat(str_type, size);
            strcat(str_type, "]");
            cur = cur->next;
        }

        if (count > 0)  printf(",");
        printf("%s", str_type);

        parameterlist = parameterlist->next;
        count++;
    }
}

SymbolTableEntry *FindEntry(SymbolTable *table, const char *name) {
    SymbolTableEntry *entry = table->entry_head;
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) return entry;
        entry = entry->next;
    }
    return NULL;
}

SymbolTable* InsertTable(SymbolTable *last, int level) {
    SymbolTable *table = (SymbolTable*) malloc(sizeof(SymbolTable));
    table->size = 0;
    table->level = level;
    table->entry_head = table->entry_tail = NULL;
    table->prev = last;
    table->next = NULL;

    if (last != NULL)   last->next = table;
    return table;
}

SymbolTableEntry *InsertEntryFromId(SymbolTable *table, IdList *idlist, char *kind, Type *type) {
    while (idlist != NULL) {
        if (FindEntry(table, idlist->name) != NULL) {
            printf("Error at Line #%d: '%s' is redeclared.\n", linenum, idlist->name);
            idlist = idlist->next;
            continue;
        }

        SymbolTableEntry *entry = (SymbolTableEntry*) malloc(sizeof(SymbolTableEntry));
        strcpy(entry->name, idlist->name);
        strcpy(entry->kind, kind);
        entry->level = table->level;
        entry->type = CreateType(type->name, idlist->array_signature);
        entry->attribute = CreateAttribute(NULL, idlist->value);

        entry->prev = table->entry_tail;
        entry->next = NULL;

        if (table->entry_tail == NULL) {
            table->entry_head = table->entry_tail = entry;
        }
        else {
            table->entry_tail->next = entry;
            table->entry_tail = table->entry_tail->next;
        }

        table->size++;
        idlist = idlist->next;
    }
    return table->entry_tail;
}

SymbolTableEntry *InsertEntryFromParameter(SymbolTable *table, ParameterList *parameterlist) {
    while (parameterlist != NULL) {
        if (FindEntry(table, parameterlist->name) != NULL) {
            printf("Error at Line #%d: '%s' is redeclared.\n", linenum, parameterlist->name);
            parameterlist = parameterlist->next;
            continue;
        }

        SymbolTableEntry *entry = (SymbolTableEntry*) malloc(sizeof(SymbolTableEntry));
        strcpy(entry->name, parameterlist->name);
        strcpy(entry->kind, "parameter");
        entry->level = table->level;
        // entry->type = parameterlist->type;
        entry->type = CopyType(parameterlist->type);
        entry->attribute = CreateAttribute(NULL, NULL);

        entry->prev = table->entry_tail;
        entry->next = NULL;

        if (table->entry_tail == NULL) {
            table->entry_head = table->entry_tail = entry;
        }
        else {
            table->entry_tail->next = entry;
            table->entry_tail = table->entry_tail->next;
        }

        table->size++;
        parameterlist = parameterlist->next;
    }
    return table->entry_tail;
}

SymbolTableEntry *InsertEntryFromFunction(SymbolTable *table, ParameterList *parameterlist, char *name, char *typename) {
    if (FindEntry(table, name) != NULL) {
        // printf("Error at Line #%d: '%s' is redeclared.\n", linenum, name);
        return table->entry_tail;
    }

    SymbolTableEntry *entry = (SymbolTableEntry*) malloc(sizeof(SymbolTableEntry));
    strcpy(entry->name, name);
    strcpy(entry->kind, "function");
    entry->level = table->level;
    entry->type = CreateType(typename, NULL);
    entry->attribute = CreateAttribute(parameterlist, NULL);

    // printf("%s\n", parameterlist->type->name);

    entry->prev = table->entry_tail;
    entry->next = NULL;
    table->size++;

    if (table->entry_tail == NULL) {
        table->entry_head = table->entry_tail = entry;
    }
    else {
        table->entry_tail->next = entry;
        table->entry_tail = table->entry_tail->next;
    }

    return table->entry_tail;
}

IdList *InsertIdList(IdList *last, char *name, ArraySignature *array_signature, Value *value) {
    IdList *list = (IdList*) malloc(sizeof(IdList));
    list->name = strdup(name);
    list->array_signature = array_signature;
    list->value = value;
    list->prev = last;
    list->next = NULL;

    if (last != NULL)   last->next = list;
    return list;
}

ParameterList *InsertParameterList(ParameterList *last, Type *type, char *name, ArraySignature *array_signature) {
    ParameterList *parameterlist = (ParameterList*) malloc(sizeof(ParameterList));
    type->array_signature = array_signature;
    parameterlist->type = type;
    parameterlist->name = strdup(name);
    parameterlist->prev = last;
    parameterlist->next = NULL;

    if (last != NULL)   last->next = parameterlist;
    return parameterlist;
}

ArraySignature *InsertArraySignature(ArraySignature *last, const int size) {
    ArraySignature *array_signature = (ArraySignature*) malloc(sizeof(ArraySignature));
    array_signature->size = size;
    array_signature->prev = last;
    array_signature->next = NULL;

    if (last != NULL)   last->next = array_signature;
    return array_signature;
}

void DeleteTable(SymbolTable *table) {
    while (table->entry_head != NULL) {
        if (table->entry_head->next == NULL) {
            DeleteEntry(table->entry_head);
            table->entry_head = table->entry_tail = NULL;
        }
        else {
            table->entry_head = table->entry_head->next;
            DeleteEntry(table->entry_head->prev);
            table->entry_head->prev = NULL;
        }       
    }
    free(table);
}

void DeleteEntry(SymbolTableEntry *entry) {
    DeleteType(entry->type);
    DeleteAttribute(entry->attribute);
}

void DeleteType(Type *type) {
    ArraySignature *cur = type->array_signature;
    while(cur != NULL) {
        if (cur->next == NULL) {
            free(cur);
            cur = NULL;
        }
        else {
            cur = cur->next;
            free(cur->prev);
            cur->prev = NULL;
        }
    }
    free(type);
}

void DeleteAttribute(Attribute *attr) {
    if (attr->value != NULL) {
        if (attr->value->svalue != NULL)
            free(attr->value->svalue);
        free(attr->value);
    }

    if (attr->parameterlist != NULL)
        free(attr->parameterlist);

    free(attr);
}

void DeleteIdList(IdList *head) {
    while (head != NULL) {
        if (head->next == NULL) {
            free(head->name);
            free(head);
            head = NULL;
        }
        else {
            head = head->next;
            free(head->prev->name);
            free(head->prev);
            head->prev = NULL;
        }
    }
}

void DeleteParameterList(ParameterList *head) {
    while (head != NULL) {
        if (head->next == NULL) {
            DeleteType(head->type);
            free(head->name);
            free(head);
            head = NULL;
        }
        else {
            head = head->next;
            DeleteType(head->prev->type);
            free(head->prev->name);
            free(head->prev);
            head->prev = NULL;
        }
    }
}

Type *CopyType(Type *type) {
    Type *copy = (Type*) malloc(sizeof(Type));
    strcpy(copy->name, type->name);

    ArraySignature *head = NULL, *tail = NULL;
    ArraySignature *cur = type->array_signature;

    if (cur != NULL) {
        head = tail = InsertArraySignature(tail, cur->size);
        cur = cur->next;
    }

    while (cur != NULL) {
        tail = InsertArraySignature(tail, cur->size);
        cur = cur->next;
    }
    copy->array_signature = head;
    return copy;
}

Type *CreateType(const char *typename, ArraySignature *array_signature) {
    Type *type = (Type*) malloc(sizeof(Type));
    strcpy(type->name, typename);
    type->array_signature = array_signature;
    return type;
}

Attribute *CreateAttribute(ParameterList *parameterlist, Value *value) {
    Attribute *attribute = (Attribute*) malloc(sizeof(Attribute));
    attribute->parameterlist = parameterlist;
    attribute->value = value;
    return attribute;
}

Value *CreateValue(const char *typename, char *svalue) {
    Value *value = (Value*) malloc(sizeof(Value));
    value->type = CreateType(typename, NULL);
    value->svalue = NULL;

    if (strcmp(typename, "int") == 0) {
        value->ivalue = atoi(svalue);
    }
    else if (strcmp(typename, "float") == 0) {
        value->dvalue = atof(svalue);
        value->svalue = strdup(svalue);
    }
    else if (strcmp(typename, "scientific") == 0) {
        value->svalue = strdup(svalue);
    }
    else if (strcmp(typename, "string") == 0) {
        value->svalue = strdup(svalue);
    }
    else if (strcmp(typename, "bool") == 0) {
        value->svalue = strdup(svalue);
    }
    return value;
}
