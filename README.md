# OP

OP is a small interpreted C-like toy programming language built from the ground
up, all the way from lexical to semantic analysis.

I consider this project proof-of-concept, the concept being that I can build a
programming language by myself. It is and will probably always be unfinished.

Run `make op` to compile the interpreter. To execute some code, run `op <src>`
and be amazed!

## Example code

Examples of code are available inside the files `sqrt.op` and `strings.op`. The
source code in `sqrt.op` computes the square root of 5 using divide-et-impera
and `strings.op` showcases string operations. Any language quirks may be
observed here.

As there is no way to do I/O, the assignment operator (`=`) will print its
rvalue to standard output, so you may check the results.

## Features and language quirks

- Functions (and nested ones, too!) are defined with `func`. Functions can
return with keyword `ret`. Functions return values can be of whatever type.
Recursion is possible.
- Two data types: `num` and `str` repesenting floats and strings respectively.
There are also literals for both of these types. Note: aside from function
return values, the language is strongly typed.
- Basic control flow statements: `if`/`else`, `while`. Statment `else` can also
be used after a `while`, in which case it will get executed only once, if the
condition of `while` is not initially true.
- Some operators are overloaded:
	- `str + str` will perform string concatenation
	- `str + num` and `str - num` will perform string slicing
	- `str * num` or `num * str` will repeat `str` `num` times
	- comparison operators `= != < > <= >=` work as expected on strings
- Compound statements can be opened and closed using `{` and `}` respectively.
These also define scopes and work as expected in regards to memory allocation
and name visibility.
- Language does not feature arrays nor custom types.
- Due to parsing limitations:
	- unless longer than one statement, any source must start by opening a
	scope and must end by closing it at the end
	- a `;` must be placed at the end of every statement in a compoud
	statement except the last one, which precedes the `}`
	- declarations and calls of/to functions without parameters must be made
	without parantheses
	- compound statements must befollowed by `;` where appropriate
	- `if`/`else` statements are, in fact, just one statement; do not
	separate the instruction of an `if` and the keyword `else` with a `;`
- The interpreter does not report parsing or semantic errors, so you better get
it right the first time!

## Project structure and implementation details

- `vocabulary.c/.h`: Everything lexer related. Lexing is done using a DFA
(deterministic finite automata).
- `grammar.c/.h`: Definition of a DFA which describes order of tokens.
Not a definition of a context-free grammar.
- `parser.c/.h`: Parsing implementation. Puts together lexical and grammar
analysis. Implements a form of precedence parsing (done using Dijktra's shunting
yard algorithm) mixed with the aformentiond grammar DFA and recursion to ensure
proper order of tokens. Builds and returns an abstract syntax tree.
- `contextualizer.c/.h`: Concers the creation of scopes.
- `runner.c/.h`: Runs the code according the abstract syntax tree built earlier
and performs type checking.
- `main.c`: Where everything is put togheter.
