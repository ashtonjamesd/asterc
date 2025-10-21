#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "ast.h"
#include "../util/alloc.h"

static AstNode *parseStatement(Parser *parser);

Parser newParser(Token *tokens, size_t count) {
    return (Parser){
        .tokens = tokens,
        .count = count,
        .ast = {
            .nodes = alloc(sizeof(AstNode *)),
            .count = 0,
            .capacity = 1
        },
        .position = 0
    };
}

void freeParser(Parser *parser) {
    if (!parser) return;

    for (size_t i = 0; i < parser->ast.count; i++) {
        freeAstNode(parser->ast.nodes[i]);
    }

    FREE_ALLOC(parser->ast.nodes);
}

static Token currentToken(Parser *parser) {
    if (parser->position >= parser->count) {
        return (Token){ .type = TOKEN_EOF, .lexeme = "", .line = 0, .column = 0 };
    }
    return parser->tokens[parser->position];
}

static bool match(Parser *parser, TokenType type) {
    if (parser->position >= parser->count) return false;
    if (currentToken(parser).type != type) return false;

    return true;
}

static void advance(Parser *parser) {
    parser->position++;
}

static void recede(Parser *parser) {
    parser->position--;
}

static bool expect(Parser *parser, TokenType type) {
    if (match(parser, type)) {
        advance(parser);
        return true;
    }
    return false;
}

static void addAstNode(Parser *parser, AstNode *node) {
    if (parser->ast.count >= parser->ast.capacity) {
        parser->ast.capacity *= 2;
        parser->ast.nodes = realloc(parser->ast.nodes, parser->ast.capacity * sizeof(AstNode *));
        assertAlloc(parser->ast.nodes);
    }
    
    parser->ast.nodes[parser->ast.count++] = node;
}

static AstNode *parseBlock(Parser *parser) {
    AstNode **nodes = alloc(sizeof(AstNode *));
    size_t bodyCount = 0;
    size_t bodyCapacity = 1;

    while (!match(parser, TOKEN_RIGHT_BRACE) && parser->position < parser->count) {
        AstNode *node = parseStatement(parser);
        
        if (bodyCount >= bodyCapacity) {
            bodyCapacity *= 2;
            nodes = realloc(nodes, bodyCapacity * sizeof(AstNode));
            assertAlloc(nodes);
        }
        nodes[bodyCount++] = node;

        if (parser->position >= parser->count) break;
    }
    if (!match(parser, TOKEN_RIGHT_BRACE)) recede(parser);

    AstNode *block = newBlockNode(nodes, bodyCount);
    return block;
}

static AstNode *parseFn(Parser *parser) {
    bool isPublic = false;
    if (match(parser, TOKEN_PUB)) {
        advance(parser);
        isPublic = true;
    }

    if (!match(parser, TOKEN_FN)) return newErrNode();
    advance(parser);

    Token nameToken = currentToken(parser);
    if (!expect(parser, TOKEN_IDENTIFIER)) return newErrNode();
    
    if (!expect(parser, TOKEN_COLON)) return newErrNode();

    Token returnTypeToken = currentToken(parser);
    if (!expect(parser, TOKEN_IDENTIFIER)) return newErrNode();

    if (!expect(parser, TOKEN_LEFT_BRACE)) return newErrNode();
    if (!expect(parser, TOKEN_NEWLINE)) return newErrNode();

    AstNode *blockNode = parseBlock(parser);
    if (blockNode->type == AST_NODE_ERR) return blockNode;

    if (!expect(parser, TOKEN_RIGHT_BRACE)) return newErrNode();

    while (match(parser, TOKEN_NEWLINE)) {
        advance(parser);
    }

    AstNode *fnNode = newFnNode(nameToken.lexeme, isPublic, returnTypeToken.lexeme, blockNode->asBlock);

    return fnNode;
}

static AstNode *parsePrimary(Parser *parser) {
    Token token = currentToken(parser);

    if (match(parser, TOKEN_INTEGER_LITERAL)) {
        AstNode *intNode = newIntegerNode(atoi(token.lexeme));
        advance(parser);

        return intNode;
    }

    advance(parser);
    return newErrNode();
}

static AstNode *parseRet(Parser *parser) {
    if (!expect(parser, TOKEN_RET)) return newErrNode();

    if (match(parser, TOKEN_NEWLINE)) {
        advance(parser);
        return newRetNode(NULL);
    }

    AstNode *value = parsePrimary(parser);
    AstNode *retNode = newRetNode(value);

    if (match(parser, TOKEN_NEWLINE)) {
        advance(parser);
    }

    return retNode;
}

static AstNode *parseExec(Parser *parser) {
    if (!expect(parser, TOKEN_EXEC)) return newErrNode();

    Token byteToken = currentToken(parser);
    if (!expect(parser, TOKEN_INTEGER_LITERAL)) return newErrNode();

    if (match(parser, TOKEN_NEWLINE)) {
        advance(parser);
    }

    return newExecNode(atoi(byteToken.lexeme));
}

static AstNode *parseStatement(Parser *parser) {
    switch (currentToken(parser).type) {
        case TOKEN_PUB: {
            return parseFn(parser);
        }
        case TOKEN_RET: {
            return parseRet(parser);
        }
        case TOKEN_EXEC: {
            return parseExec(parser);
        }
        default: {
            return parsePrimary(parser);
        }
    }
}

void parseAst(Parser * parser) {
    if (!parser) return;

    while (parser->position < parser->count) {
        AstNode *node = parseStatement(parser);
        addAstNode(parser, node);

        if (parser->position >= parser->count) break;
    }
}

void printParserAst(const Parser *parser) {
    if (!parser) {
        printf("Parser is null\n");
        return;
    }
    
    printAllAstNodes(parser->ast.nodes, parser->ast.count);
}