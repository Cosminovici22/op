#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include "vocabulary.h"
#include "grammar.h"

typedef struct SyntaxTree {
    Token token;
    int nextCount;
    struct SyntaxTree **next;
} SyntaxTree;

SyntaxTree *parse(Vocabulary *, Grammar *, FILE *);
SyntaxTree *SyntaxTree_free(SyntaxTree *);
void SyntaxTree_print(SyntaxTree *);

#endif
