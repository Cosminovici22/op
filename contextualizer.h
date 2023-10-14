#ifndef CONTEXTUALIZER_H
#define CONTEXTUALIZER_H

#include <stdlib.h>
#include <string.h>
#include "parser.h"

typedef enum {
    ERROR,
    SUCCESS,
    NUMERIC,
    STRING,
    FUNCTION
} DataName;

typedef struct Data {
    DataName name;
    union {
        double num;
        char *str;
        SyntaxTree *func;
    } value;
} Data;

typedef struct SymbolList {
    char *id;
    Data data;
    struct SymbolList *next;
} SymbolList;

typedef struct SymbolTable {
    SymbolList *list[20];
} SymbolTable;

typedef struct Scope {
    SymbolTable table;
    struct Scope *parent;
} Scope;

Scope *Scope_new(Scope *);
Scope *Scope_free(Scope *);
Data *Scope_fetch(Scope *, char *);
int Scope_insert(Scope *, char *, DataName);

#endif
