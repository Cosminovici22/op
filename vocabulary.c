#include "vocabulary.h"

#define isWhite(c) (c == ' ' || c == '\t' || c == '\v' || c == '\n' || \
                    c == '\r' || c == '\f')

#define isValid(c) ((c >= ' ' && c <= '~') || isWhite(c))

#define hash(c) (!isWhite(c) ? c - 32 : 0)

static inline void fill(Vocabulary *node, Vocabulary *next, int first, int last) {
    for (int i = first; i <= last; i++) {
        node->next[i] = next;
    }
}

static Vocabulary *Vocabulary_new(TokenType name) {
    Vocabulary *node = calloc(1, sizeof(Vocabulary));
    node->name = name;
    return node;
}

static void Vocabulary_keyword(Vocabulary *node, Token keyword, Vocabulary *identifier) {
    if (*keyword.value == '\0') {
        node->name = keyword.name;
        return;
    }
    int index = hash(*keyword.value);
    if (node->next[index] == identifier) {
        node->next[index] = Vocabulary_new(IDENTIFIER);
        fill(node->next[index], identifier, hash('A'), hash('Z'));
        fill(node->next[index], identifier, hash('a'), hash('z'));
        fill(node->next[index], identifier, hash('0'), hash('9'));
        node->next[hash('_')] = identifier;
    }
    keyword.value += 1;
    Vocabulary_keyword(node->next[index], keyword, identifier);
}

static void Vocabulary_symbol(Vocabulary *node, Token symbol) {
    if (*symbol.value == '\0') {
        node->name = symbol.name;
        return;
    }
    int index = hash(*symbol.value);
    if (node->next[index] == NULL) {
        node->next[index] = Vocabulary_new(INVALID);
    }
    symbol.value += 1;
    Vocabulary_symbol(node->next[index], symbol);
}

Vocabulary *Vocabulary_vocabulary(void) {
    Token keywords[] = {
        {"num", NUM}, {"str", STR},
        {"func", FUNC}, {"ret", RET},
        {"if", IF}, {"else", ELSE}, {"while", WHILE}
    }, symbols[] = {
        {"!", NOT},
        {"*", MUL}, {"/", DIV},
        {"+", ADD}, {"-", SUB},
        {"==", EQ}, {"!=", DIF}, {">", G}, {">=", GE}, {"<", L}, {"<=", LE},
        {"&&", AND}, {"||", OR},
        {"=", ASG},
        {"(", LPAR}, {")", RPAR}, {"{", LBRC}, {"}", RBRC},
        {";", SCL}, {",", COM}
    };

    Vocabulary *hub = Vocabulary_new(INVALID);

    /* WHITESPACE */

    hub->next[hash(' ')] = hub;

    /* IDENTIFIER */

    Vocabulary *identifier = Vocabulary_new(IDENTIFIER);
    fill(identifier, identifier, hash('A'), hash('Z'));
    fill(identifier, identifier, hash('a'), hash('z'));
    fill(identifier, identifier, hash('0'), hash('9'));
    identifier->next[hash('_')] = identifier;

    fill(hub, identifier, hash('A'), hash('Z'));
    fill(hub, identifier, hash('a'), hash('z'));
    hub->next[hash('_')] = identifier;

    /* NUMERIC_LITERAL */

    Vocabulary *numeric2 = Vocabulary_new(NUMERIC_LITERAL);
    fill(numeric2, numeric2, hash('0'), hash('9'));

    Vocabulary *dot = Vocabulary_new(INVALID);
    fill(dot, numeric2, hash('0'), hash('9'));

    Vocabulary *numeric1 = Vocabulary_new(NUMERIC_LITERAL);
    fill(numeric1, numeric1, hash('0'), hash('9'));
    numeric1->next[hash('.')] = dot;

    fill(hub, numeric1, hash('0'), hash('9'));

    /* STRING_LITERAL */

    Vocabulary *string = Vocabulary_new(INVALID);
    fill(string, string, hash(' '), hash('~'));
    string->next[hash('"')] = Vocabulary_new(STRING_LITERAL);

    Vocabulary *escape = Vocabulary_new(INVALID);
    string->next[hash('\\')] = escape;
    char escapees[] = { 'n', 'f', 'r', 't', 'v', '\\', '"' };
    for (int i = 0; escapees[i]; i++) {
        escape->next[hash(escapees[i])] = Vocabulary_new(INVALID);
        fill(escape->next[hash(escapees[i])], string, hash(' '), hash('~'));
        escape->next[hash(escapees[i])]->next[hash('"')] = string->next[hash('"')];
    }

    hub->next[hash('"')] = Vocabulary_new(INVALID);
    fill(hub->next[hash('"')], string, hash(' '), hash('~'));
    hub->next[hash('"')]->next[hash('"')] = string->next[hash('"')];
    hub->next[hash('"')]->next[hash('\\')] = escape;

    /* SYMBOLS */

    for (int i = 0; i < sizeof(keywords)/sizeof(Token); i++) {
        Vocabulary_keyword(hub, keywords[i], identifier);
    }

    for (int i = 0; i < sizeof(symbols)/sizeof(Token); i++) {
        Vocabulary_symbol(hub, symbols[i]);
    }

    return hub;
}

Vocabulary *Vocabulary_free(Vocabulary *node, Vocabulary *identifier, Vocabulary *numeric, Vocabulary *string) {
    if (node && node != identifier && node != numeric && node != string) {
        for (int i = 1; i < 95; i++) { // starts at 1 to avoid free'ing empty space cell in hub
            node->next[i] = Vocabulary_free(node->next[i], identifier, numeric, string);
        }
        free(node);
    }
    return NULL;
}

Vocabulary *Vocabulary_freeHub(Vocabulary *hub) {
    Vocabulary *identifier = hub->next[hash('a')];
    Vocabulary *numeric = hub->next[hash('0')];
    Vocabulary *string = hub->next[hash('"')];

    hub = Vocabulary_free(hub, identifier, numeric, string);

    free(identifier);

    free(numeric->next[hash('.')]->next[hash('0')]);
    free(numeric->next[hash('.')]);
    free(numeric);

    free(string->next[hash('a')]->next[hash('"')]);
    free(string->next[hash('a')]);
    char escapees[] = { 'n', 'f', 'r', 't', 'v', '\\', '"' };
    for (int i = 0; escapees[i]; i++) {
        free(string->next[hash('\\')]->next[hash(escapees[i])]);
    }
    free(string->next[hash('\\')]);
    free(string);

    return NULL;
}

Token scan(Vocabulary *hub, FILE *stream) {
    // this should be in parser.c / .h
    // make each token remember the line and column to display errors nicely
    Vocabulary *current = hub;
    int c = fgetc(stream), lexLen = 0, escapees = 0;
    while (isValid(c) && current->next[hash(c)]) {
        current = current->next[hash(c)];
        c = fgetc(stream);
        lexLen += (current != hub);
        escapees += (c == '\\');
    }

    Token token = {.name = current->name, .value = NULL};
    if (c != EOF) {
        fseek(stream, -1, SEEK_CUR);
    }
    if (current->name == IDENTIFIER || current->name == NUMERIC_LITERAL) {
        fseek(stream, -lexLen, SEEK_CUR);
        token.value = malloc((lexLen + 1)*sizeof(char));
        for (int i = 0; i < lexLen; i++) {
            token.value[i] = fgetc(stream);
        }
        token.value[lexLen] = '\0';
    } else if (current->name == STRING_LITERAL) {
        fseek(stream, 1 - lexLen, SEEK_CUR);
        token.value = malloc((lexLen - 2 - escapees + 1)*sizeof(char));
        for (int i = 0; i < lexLen - 2 - escapees; i++) {
            char c = fgetc(stream);
            if (c == '\\') {
                switch (fgetc(stream)) {
                    case 'n': token.value[i] = '\n'; break;
                    case 'f': token.value[i] = '\f'; break;
                    case 'r': token.value[i] = '\r'; break;
                    case 't': token.value[i] = '\t'; break;
                    case 'v': token.value[i] = '\v'; break;
                    case '\\': token.value[i] = '\\'; break;
                    case '"': token.value[i] = '"'; break;
                }
            } else {
                token.value[i] = c;
            }
        }
        token.value[lexLen - 2 - escapees] = '\0';
        fseek(stream, 1, SEEK_CUR);
    }

    return token;
}
