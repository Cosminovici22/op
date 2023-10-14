#ifndef VOCABULARY_H
#define VOCABULARY_H

#include <stdio.h>
#include <stdlib.h>

typedef enum TokenType {
    INVALID,
    IDENTIFIER, NUMERIC_LITERAL, STRING_LITERAL,
    NUM, STR, FUNC,
    RET, IF, WHILE, ELSE,
    NOT,
    MUL, DIV,
    ADD, SUB,
    EQ, DIF, G, GE, L, LE,
    AND,
    OR,
    ASG,
    COM, LPAR, RPAR,
    SCL, LBRC, RBRC,
    TokenType_count
} TokenType;

typedef struct Token {
    char *value;
    TokenType name;
} Token;

typedef struct Vocabulary {
    TokenType name;
    struct Vocabulary *next[95];
} Vocabulary;

Vocabulary *Vocabulary_vocabulary(void);
Vocabulary *Vocabulary_freeHub(Vocabulary *);
Token scan(Vocabulary *, FILE *);

#endif
