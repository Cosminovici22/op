#include "runner.h"
// isn't this whole string shit a bit antithetical to what you want to do here?

static Data log_not(SyntaxTree *right) {
    Data rightData = run(right);
    if (rightData.name != ERROR) {
        double value = !rightData.value.num;
        return (Data){.name = NUMERIC, .value.num = value};
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data mul(SyntaxTree *left, SyntaxTree *right) {
    Data leftData = run(left);
    if (leftData.name == NUMERIC) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC) {
            double value = leftData.value.num * rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        } else if (rightData.name == STRING) {
            long rightLen = strlen(rightData.value.str);
            if (rightLen > 0 && leftData.value.num > 0) {
                long valueLen = leftData.value.num*rightLen + 1;
                char *value = calloc(valueLen, sizeof(char));
                for (long i = 0; i < leftData.value.num; i++) {
                    strcat(value, rightData.value.str);
                }
                return (Data){.name = STRING, .value.str = value};
            }
        }
    } else if (leftData.name == STRING) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC) {
            long leftLen = strlen(leftData.value.str);
            if (leftLen > 0 && rightData.value.num > 0) {
                long valueLen = rightData.value.num*leftLen + 1;
                char *value = calloc(valueLen, sizeof(char));
                for (long i = 0; i < rightData.value.num; i++) {
                    strcat(value, leftData.value.str);
                }
                return (Data){.name = STRING, .value.str = value};
            }
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data divide(SyntaxTree *left, SyntaxTree *right) {
    Data leftData = run(left);
    if (leftData.name == NUMERIC) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC) {
            double value = leftData.value.num / rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data add(SyntaxTree *left, SyntaxTree *right) {
    Data leftData = run(left);
    if (leftData.name == NUMERIC) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC) {
            double value = leftData.value.num + rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    } else if (leftData.name == STRING) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC && rightData.value.num > 0) {
            long leftLen = strlen(leftData.value.str);
            leftLen -= rightData.value.num;
            char *value = calloc(leftLen + 1, sizeof(char));
            strcat(value, leftData.value.str + (int)rightData.value.num);
            // this and sub should enforce rightData.value.num to be an int
            //
            // also, don't forget about % operator at some point
            //
            // actually i think i can just have ints and floats separately,
            // that wouldn't be that hard lol
            return (Data){.name = STRING, .value.str = value};
        } else if (rightData.name == STRING) {
            long leftLen = strlen(leftData.value.str);
            long rightLen = strlen(rightData.value.str);
            char *value = calloc(leftLen + rightLen + 1, sizeof(char));
            strcat(value, leftData.value.str);
            strcat(value, rightData.value.str);
            return (Data){.name = STRING, .value.str = value};
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data sub(SyntaxTree *left, SyntaxTree *right) {
    Data leftData = run(left);
    if (leftData.name == NUMERIC) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC) {
            double value = leftData.value.num - rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    } else if (leftData.name == STRING) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC && rightData.value.num > 0) {
            long leftLen = strlen(leftData.value.str);
            leftLen -= rightData.value.num;
            if (leftLen >= 0) {
                char *value = calloc(leftLen + 1, sizeof(char));
                strncat(value, leftData.value.str, leftLen);
                return (Data){.name = STRING, .value.str = value};
            }
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data eq(SyntaxTree *left, SyntaxTree *right) {
    // try again some time
    // or change grammar rules
    Data leftData = run(left);
    if (leftData.name == NUMERIC) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC) {
            double value = leftData.value.num == rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    } else if (leftData.name == STRING) {
        Data rightData = run(right);
        if (rightData.name == STRING) {
            double value = strcmp(leftData.value.str, rightData.value.str) == 0;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data dif(SyntaxTree *left, SyntaxTree *right) {
    Data leftData = run(left);
    if (leftData.name == NUMERIC) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC) {
            double value = leftData.value.num != rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    } else if (leftData.name == STRING) {
        Data rightData = run(right);
        if (rightData.name == STRING) {
            double value = strcmp(leftData.value.str, rightData.value.str) != 0;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data l(SyntaxTree *left, SyntaxTree *right) {
    Data leftData = run(left);
    if (leftData.name == NUMERIC) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC) {
            double value = leftData.value.num < rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    } else if (leftData.name == STRING) {
        Data rightData = run(right);
        if (rightData.name == STRING) {
            double value = strcmp(leftData.value.str, rightData.value.str) < 0;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data le(SyntaxTree *left, SyntaxTree *right) {
    Data leftData = run(left);
    if (leftData.name == NUMERIC) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC) {
            double value = leftData.value.num <= rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    } else if (leftData.name == STRING) {
        Data rightData = run(right);
        if (rightData.name == STRING) {
            double value = strcmp(leftData.value.str, rightData.value.str) <= 0;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data g(SyntaxTree *left, SyntaxTree *right) {
    Data leftData = run(left);
    if (leftData.name == NUMERIC) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC) {
            double value = leftData.value.num > rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    } else if (leftData.name == STRING) {
        Data rightData = run(right);
        if (rightData.name == STRING) {
            double value = strcmp(leftData.value.str, rightData.value.str) > 0;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data ge(SyntaxTree *left, SyntaxTree *right) {
    Data leftData = run(left);
    if (leftData.name == NUMERIC) {
        Data rightData = run(right);
        if (rightData.name == NUMERIC) {
            double value = leftData.value.num >= rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    } else if (leftData.name == STRING) {
        Data rightData = run(right);
        if (rightData.name == STRING) {
            double value = strcmp(leftData.value.str, rightData.value.str) >= 0;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data log_and(SyntaxTree *left, SyntaxTree *right) {
    Data leftData = run(left);
    if (leftData.name != ERROR) {
        Data rightData = run(right);
        if (rightData.name != ERROR) {
            double value = leftData.value.num && rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data log_or(SyntaxTree *left, SyntaxTree *right) {
    Data leftData = run(left);
    if (leftData.name != ERROR) {
        Data rightData = run(right);
        if (rightData.name != ERROR) {
            double value = leftData.value.num || rightData.value.num;
            return (Data){.name = NUMERIC, .value.num = value};
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data asg(Scope *scope, SyntaxTree *left, SyntaxTree *right) {
    Data rightData = run(right);
    if (scope && left->token.name == IDENTIFIER && rightData.name != ERROR) {
        Data *data = Scope_fetch(scope, left->token.value);
        if (data && data->name == rightData.name) {
            data->value = rightData.value;
            if (rightData.name == NUMERIC) {
                printf("%lf\n", rightData.value.num);
            } else if (rightData.name == STRING) {
                printf("%s\n", rightData.value.str);
            }
            return *data;
        }
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data scl(SyntaxTree *left, SyntaxTree *right) {
    return run(left).name != ERROR ?
        run(right) :
        (Data){.name = ERROR, .value = {0}};
}

static Data declaration(Scope *scope, SyntaxTree *dec) {
    static DataName name;
    switch (dec->token.name) {
        case NUM:
            name = NUMERIC;
            return declaration(scope, dec->next[0]);
        case STR:
            name = STRING;
            return declaration(scope, dec->next[0]);
        case IDENTIFIER:
            return Scope_insert(scope, dec->token.value, name) ?
                (Data){.name = SUCCESS, .value = {0}} :
                (Data){.name = ERROR, .value = {0}};
        case COM:
            return declaration(scope, dec->next[0]).name != ERROR
                && declaration(scope, dec->next[1]).name != ERROR ?
                (Data){.name = SUCCESS, .value = {0}} :
                (Data){.name = ERROR, .value = {0}};
        case ASG:
            return declaration(scope, dec->next[0]).name != ERROR
                && asg(scope, dec->next[0], dec->next[1]).name != ERROR ?
                (Data){.name = SUCCESS, .value = {0}} :
                (Data){.name = ERROR, .value = {0}};
        default:
            return (Data){.name = ERROR, .value = {0}};
    }
}

static Data function_dec(Scope *scope, SyntaxTree *function) {
    if (Scope_insert(scope, function->next[0]->token.value, FUNCTION)) {
        Data *data = Scope_fetch(scope, function->next[0]->token.value);
        data->value.func = function;
        return (Data){.name = SUCCESS, .value = {0}};
    }
    return (Data){.name = ERROR, .value = {0}};
}

static Data identifier(Scope **scope, SyntaxTree *id) {
    Data *data = Scope_fetch(*scope, id->token.value);
    if (data && data->name != FUNCTION) {
        return *data;
    } else if (!data) {
        return (Data){.name = ERROR, .value = {0}};
    }

    SyntaxTree *f_dec = data->value.func->next[0];
    SyntaxTree *f_def = data->value.func->next[1];
    Scope *new_scope;
    if (f_dec->next && id->next) {
        new_scope = Scope_new(*scope);
        declaration(new_scope, f_dec->next[0]);
    } else if (!f_dec->next && !id->next) {
        return scl(f_def->next[0], f_def->next[1]);
    } else {
        return (Data){.name = ERROR, .value = {0}};
    }

    SyntaxTree *params = f_dec->next[0];
    SyntaxTree *args = id->next[0];
    while (params->token.name == COM && args->token.name == COM) {
        if (!asg(new_scope, params->next[1]->next[0], args->next[1]).name) {
            return (Data){.name = ERROR, .value = {0}};
        }
        params = params->next[0];
        args = args->next[0];
    }
    if (!asg(new_scope, params->next[0], args).name) {
        return (Data){.name = ERROR, .value = {0}};
    }

    *scope = new_scope;
    return scl(f_def->next[0], f_def->next[1]);
}

Data run(SyntaxTree *syntaxTree) {
    static Scope *scope = NULL;
    static Data ret = (Data){.name = ERROR, .value = {0}};
    switch (syntaxTree->token.name) {
        case IDENTIFIER:
            Data data = identifier(&scope, syntaxTree);
            if (ret.name != ERROR) {
                Data aux = ret;
                ret = (Data){.name = ERROR, .value = {0}};
                return aux;
            } else {
                return data;
            }
        case NUMERIC_LITERAL:
            if (!syntaxTree->nextCount) {
                double num = atof(syntaxTree->token.value);
                return (Data){.name = NUMERIC, .value.num = num};
            }
            return (Data){.name = ERROR, .value = {0}};
        case STRING_LITERAL:
            if (!syntaxTree->nextCount) {
                char *str = syntaxTree->token.value;
                return (Data){.name = STRING, .value.str = str};
            }
            return (Data){.name = ERROR, .value = {0}};
        case NOT:
            return log_not(syntaxTree->next[0]);
        case MUL:
            return mul(syntaxTree->next[0], syntaxTree->next[1]);
        case DIV:
            return divide(syntaxTree->next[0], syntaxTree->next[1]);
        case ADD:
            return add(syntaxTree->next[0], syntaxTree->next[1]);
        case SUB:
            return sub(syntaxTree->next[0], syntaxTree->next[1]);
        case EQ:
            return eq(syntaxTree->next[0], syntaxTree->next[1]);
        case DIF:
            return dif(syntaxTree->next[0], syntaxTree->next[1]);
        case L:
            return l(syntaxTree->next[0], syntaxTree->next[1]);
        case LE:
            return le(syntaxTree->next[0], syntaxTree->next[1]);
        case G:
            return g(syntaxTree->next[0], syntaxTree->next[1]);
        case GE:
            return ge(syntaxTree->next[0], syntaxTree->next[1]);
        case AND:
            return log_and(syntaxTree->next[0], syntaxTree->next[1]);
        case OR:
            return log_or(syntaxTree->next[0], syntaxTree->next[1]);
        case ASG:
            return asg(scope, syntaxTree->next[0], syntaxTree->next[1]);
        case SCL:
            return scl(syntaxTree->next[0], syntaxTree->next[1]);
        case LBRC:
            scope = Scope_free(scope);
            return (Data){.name = SUCCESS, .value = {0}};
        case RBRC:
            scope = Scope_new(scope);
            return scl(syntaxTree->next[0], syntaxTree->next[1]);
        case NUM: case STR:
            return declaration(scope, syntaxTree);
        case FUNC:
            return function_dec(scope, syntaxTree);
        case RET:
            ret = run(syntaxTree->next[0]);
            return (Data){.name = ERROR, .value = {0}};
        case IF:
            if (run(syntaxTree->next[0]).value.num) {
                // faulty semantics accepted inside instructions for
                // conditionals with false conditions
                return run(syntaxTree->next[1]);
            }
            return (Data){.name = SUCCESS, .value = {0}};
        case WHILE:
            while (run(syntaxTree->next[0]).value.num) {
                if (run(syntaxTree->next[1]).name == ERROR) {
                    return (Data){.name = ERROR, .value = {0}};
                }
            }
            return (Data){.name = SUCCESS, .value = {0}};
        case ELSE:
            if (!run(syntaxTree->next[0]->next[0]).value.num) {
                return run(syntaxTree->next[1]);
            }
            return run(syntaxTree->next[0]);
        default:
            return (Data){.name = ERROR, .value = {0}};
    }
}
