#ifndef parser_h
#define parser_h

#include <stddef.h>

#include "token.h"
#include "ast.h"

typedef struct {
    Token *tokens;
    size_t count;

    Ast ast;
    size_t position;
} Parser;

Parser newParser(Token *tokens, size_t count);
void freeParser(Parser *parser);

void parseAst(Parser *parser);
void printParserAst(const Parser *parser);

#endif