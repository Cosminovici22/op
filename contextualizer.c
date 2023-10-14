#include "contextualizer.h"

inline static int hash(char *id) {
    int index = 0;
    while (*id) {
        index += *id++;
    }
    return index%(sizeof(SymbolTable)/sizeof(SymbolList **));
}

Scope *Scope_new(Scope *parent) {
    Scope *scope = calloc(1, sizeof(Scope));
    scope->parent = parent;
    return scope;
}

Scope *Scope_free(Scope *scope) {
    if (!scope) {
        return NULL;
    }
    for (int i = 0; i < sizeof(SymbolTable)/sizeof(SymbolList **); i++) {
        SymbolList *j = scope->table.list[i];
        while (j) {
            SymbolList *aux = j;
            j = j->next;
            free(aux);
        }
    }
    Scope *aux = scope->parent;
    free(scope);
    return aux;
}

Data *Scope_fetch(Scope *scope, char *id) {
    if (scope && id) {
        for (SymbolList *i = scope->table.list[hash(id)]; i; i = i->next) {
            if (!strcmp(i->id, id)) {
                return &i->data;
            }
        }
        return Scope_fetch(scope->parent, id);
    }
    return NULL;
}

int Scope_insert(Scope *scope, char *id, DataName name) {
    Scope *aux = scope->parent;
    scope->parent = NULL;
    if (!Scope_fetch(scope, id)) {
        scope->parent = aux;
        int index = hash(id);
    
        SymbolList *aux = malloc(sizeof(SymbolList));
        aux->id = id;
        aux->data = (Data){.name = name, .value = {0}};
        aux->next = scope->table.list[index];
        scope->table.list[index] = aux;

        return 1;
    }
    scope->parent = aux;
    return 0;
}
