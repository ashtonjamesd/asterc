#ifndef compiler_h
#define compiler_h

#include <stddef.h>
#include <stdio.h>

#include "../parser/ast.h"

typedef struct {
    Ast ast;
    FILE *out;
}  Compiler;

Compiler newCompiler(Ast ast);
void freeCompiler(Compiler *compiler);

void compile(Compiler *compiler);

#endif