#include "grammar.h"

Grammar *Grammar_grammar(void) {
    #define Grammar_new() (calloc(1, sizeof(Grammar)));
    #define fill(current, follow, first, last) \
        for (int i = first; i <= last; i++) { \
            current->next[i] = follow; \
        }

    Grammar *statement = Grammar_new();

    /* BRACES AND SEMICOLON */

    statement->next[LBRC] = Grammar_new();
    statement->next[LBRC]->next[ALT] = statement;
    statement->next[LBRC]->next[RBRC] = Grammar_new();
    statement->next[LBRC]->next[SCL] = statement->next[LBRC];

    /* EXPRESSION */

    Grammar *expression = Grammar_new();

    Grammar *operand = Grammar_new();
    fill(operand, expression, MUL, ASG);
    operand->next[LPAR] = Grammar_new();
    operand->next[LPAR]->next[ALT] = expression;
    operand->next[LPAR]->next[RPAR] = operand;
    operand->next[LPAR]->next[COM] = operand->next[LPAR];

    fill(expression, operand, IDENTIFIER, STRING_LITERAL);

    expression->next[NOT] = expression;

    expression->next[LPAR] = Grammar_new();
    expression->next[LPAR]->next[ALT] = expression;
    expression->next[LPAR]->next[RPAR] = operand;

    fill(statement, operand, IDENTIFIER, STRING_LITERAL);
    statement->next[LPAR] = expression->next[LPAR];
    statement->next[NOT] = expression->next[NOT];

    /* CONDITIONAL */

    Grammar *conditional = Grammar_new();
    conditional->next[LPAR] = Grammar_new();
    conditional->next[LPAR]->next[ALT] = expression;
    conditional->next[LPAR]->next[RPAR] = Grammar_new();
    conditional->next[LPAR]->next[RPAR]->next[ALT] = statement;
    conditional->next[LPAR]->next[RPAR]->next[ELSE] = Grammar_new();
    conditional->next[LPAR]->next[RPAR]->next[ELSE]->next[ALT] = statement;
    fill(statement, conditional, IF, WHILE);

    // this would work if you had 3 way if's
    // Grammar *conditional = Grammar_new();
    // conditional->next[ALT] = expression;
    // conditional->next[LBRC] = Grammar_new();
    // conditional->next[LBRC]->next[ALT] = statement;
    // conditional->next[LBRC]->next[SCL] = conditional->next[LBRC];
    // conditional->next[LBRC]->next[RBRC] = Grammar_new();
    // Grammar *alternative = Grammar_new();
    // conditional->next[LBRC]->next[RBRC]->next[ELSE] = alternative;
    // fill(alternative, conditional, IF, WHILE);
    // alternative->next[LBRC] = statement->next[LBRC];
    // fill(statement, conditional, IF, WHILE);

    /* DECLARATION */

    Grammar *declaration = Grammar_new();
    declaration->next[ALT] = Grammar_new();
    declaration->next[ALT]->next[IDENTIFIER] = Grammar_new();
    declaration->next[ALT]->next[IDENTIFIER]->next[ASG] = expression;
    declaration->next[COM] = declaration;
    fill(statement, declaration, NUM, STR);

    /* FUNCTION DECLARATION AND RETURN */

    Grammar *parameter = Grammar_new();
    parameter->next[IDENTIFIER] = Grammar_new();

    Grammar *function = Grammar_new();
    function->next[LPAR] = Grammar_new();
    function->next[LPAR]->next[ALT] = Grammar_new();
    fill(function->next[LPAR]->next[ALT], parameter, NUM, STR);
    function->next[LPAR]->next[COM] = function->next[LPAR];
    function->next[LPAR]->next[RPAR] = Grammar_new();
    function->next[LPAR]->next[RPAR]->next[LBRC] = statement->next[LBRC];

    statement->next[FUNC] = Grammar_new();
    statement->next[FUNC]->next[IDENTIFIER] = function;
    statement->next[FUNC]->next[IDENTIFIER]->next[LBRC] = statement->next[LBRC];

    statement->next[RET] = expression; // empty ret does not work

    return statement;

    #undef fill
    #undef Grammar_new
}
