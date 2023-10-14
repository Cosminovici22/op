#include "parser.h"

SyntaxTree *SyntaxTree_node(Token token) {
    static char nextCount[] = {
        0, // INVALID
        0, // IDENTIFIER
        0, // NUMERIC_LITERAL
        0, // STRING_LITERAL
        1, // NUM
        1, // STR
        2, // FUNC
        1, // RET
        2, // IF
        2, // WHILE
        2, // ELSE
        1, // NOT
        2, // MUL
        2, // DIV
        2, // ADD
        2, // SUB
        2, // EQ
        2, // DIF
        2, // G
        2, // GE
        2, // L
        2, // LE
        2, // AND
        2, // OR
        2, // ASG
        2, // COM
        0, // LPAR
        0, // RPAR
        2, // SCL
        0, // LBRC
        2 // RBRC
    };
    SyntaxTree *syntaxTree = malloc(sizeof(SyntaxTree));
    syntaxTree->token = token;
    syntaxTree->nextCount = nextCount[token.name];
    syntaxTree->next = NULL;
    if (syntaxTree->nextCount) {
        syntaxTree->next = calloc(syntaxTree->nextCount, sizeof(SyntaxTree *));
    }
    return syntaxTree;
}

SyntaxTree *parse(Vocabulary *vocabulary, Grammar *grammar, FILE *stream) {
    // this shit sucks
    #define clean() do { \
        while (bufferCap) { \
            SyntaxTree_free(buffer[--bufferCap]); \
        } \
        while (outputCap) { \
            SyntaxTree_free(output[--outputCap]); \
        } \
    } while(0);
    #define insert() do { \
        for (int i = buffer[bufferCap - 1]->nextCount - 1; i >= 0;) { \
            if (outputCap == 0) { \
                clean(); \
                return NULL; \
            } \
            buffer[bufferCap - 1]->next[i--] = output[--outputCap]; \
        } \
        output[outputCap++] = buffer[--bufferCap]; \
    } while(0);

    static char prior[] = {
        // beware of order when adding new vocabulary
        0, // INVALID
        9, // IDENTIFIER
        9, // NUMERIC_LITERAL
        9, // STRING_LITERAL
        1, // NUM
        1, // STR
        1, // FUNC
        1, // RET
        1, // IF
        1, // WHILE
        1, // ELSE
        8, // NOT
        7, // MUL
        7, // DIV
        6, // ADD
        6, // SUB
        5, // EQ
        5, // DIF
        5, // G
        5, // GE
        5, // L
        5, // LE
        4, // AND
        3, // OR
        2, // ASG
        10, // COM
        0, // LPAR
        0, // RPAR
        10, // SCL
        9, // LBRC
        9 // RBRC
    };
    static Token token;
    SyntaxTree *buffer[10] = {0}, *output[10] = {0}; // maintain size up to date
    int bufferCap = 0, outputCap = 0;

    // for unary plus and minus have some ALT maybe, idfk

    token = scan(vocabulary, stream);
    while (token.name != INVALID && grammar->next[token.name]) {
        grammar = grammar->next[token.name];

        if (token.name == LPAR) {
            if (bufferCap && buffer[bufferCap - 1]->token.name == IDENTIFIER) {
                buffer[bufferCap - 1]->nextCount = 1;
                buffer[bufferCap - 1]->next = calloc(1, sizeof(SyntaxTree *));
            }
        } else if (token.name != RPAR) {
            while (bufferCap &&
            prior[token.name] <= prior[buffer[bufferCap - 1]->token.name]) {
                insert();
            }
            buffer[bufferCap++] = SyntaxTree_node(token);
        }

        if (grammar->next[ALT]) {
            output[outputCap++] = parse(vocabulary, grammar->next[ALT], stream);
            if (!output[outputCap - 1]) {
                clean();
                return NULL;
            }
        } else {
            token = scan(vocabulary, stream);
        }
    }

    while (bufferCap) {
        insert();
    }

    if (outputCap == 1) {
        // insufficient
        // last token returned by scan should be eof (or invalid?) for parsing
        // to be correct
        return output[outputCap - 1];
    } else {
        clean();
        return NULL;
    }

    #undef insert
    #undef clean
}

SyntaxTree *SyntaxTree_free(SyntaxTree *syntaxTree) {
    if (syntaxTree) {
        for (int i = 0; i < syntaxTree->nextCount; i++) {
            SyntaxTree_free(syntaxTree->next[i]);
        }
        if (syntaxTree->token.value) {
            free(syntaxTree->token.value);
        }
        free(syntaxTree);
    }
    return NULL;
}

void SyntaxTree_print(SyntaxTree *syntaxTree) {
    if (!syntaxTree) return;
    SyntaxTree *arr[256];
    int curr = 0;
    arr[curr++] = syntaxTree;
    for (int i = 0; i < curr; i++) {
        syntaxTree = arr[i];
        if (syntaxTree->token.value) {
            printf("%s ", syntaxTree->token.value);
        } else {
            printf("%d ", syntaxTree->token.name);
        }
        for (int i = 0; i < syntaxTree->nextCount; i++) {
            arr[curr++] = syntaxTree->next[i];
        }
    }
    printf("\n");
}
