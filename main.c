#include <stdio.h>
#include "vocabulary.h"
#include "grammar.h"
#include "parser.h"
#include "runner.h"

int main(int argCount, char **args) {
    FILE *stream = fopen(args[1], "rt");
    if (!stream) {
        fprintf(stderr, "ERROR: inaccessible or absent file given as input\n");
        return -1;
    }

    Vocabulary *vocabulary = Vocabulary_vocabulary();
    Grammar *grammar = Grammar_grammar();

    SyntaxTree *syntaxTree = parse(vocabulary, grammar, stream);
    if (!syntaxTree) goto end;

    Data assessment = run(syntaxTree);
    printf("%d ", assessment.name);
    switch (assessment.name) {
        case ERROR: case FUNCTION: case SUCCESS: break;
        case NUMERIC: printf("%lf", assessment.value.num); break;
        case STRING: printf("%s", assessment.value.str); break;
    }
    printf("\n");

    syntaxTree = SyntaxTree_free(syntaxTree);

end:
    vocabulary = Vocabulary_freeHub(vocabulary);
    // grammar = Grammar_free(grammar);

    fclose(stream);
    stream = NULL;
    return 0;
}
