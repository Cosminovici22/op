#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <stdlib.h>
#include "vocabulary.h"
#define ALT INVALID

typedef struct Grammar {
    struct Grammar *next[TokenType_count];
} Grammar;

Grammar *Grammar_grammar(void);
// free function needed

#endif