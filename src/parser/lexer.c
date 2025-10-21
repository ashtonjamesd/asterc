#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "lexer.h"
#include "../util/alloc.h"

char *readFile(const char *path) {
    FILE *fptr = fopen(path, "r");
    if (!fptr) {
        fprintf(stderr, "unable to open file: %s\n", path);
        return NULL;
    }

    fseek(fptr, 0, SEEK_END);
    int sz = ftell(fptr);
    rewind(fptr);

    char *buff = malloc(sz + 1);
    if (!buff) {
        fclose(fptr);
        return NULL;
    }

    fread(buff, 1, sz, fptr);
    buff[sz] = '\0';
    fclose(fptr);

    return buff;
}

Lexer newLexer(const char *path) {
    char *source = readFile(path);
    if (!source) {
        fprintf(stderr, "failed to read source file: %s\n", path);
        exit(EXIT_FAILURE);
    }

    Lexer lexer = {
        .tokens = alloc(sizeof(Token)),
        .count = 0,
        .capacity = 1,
        .keywords = alloc(sizeof(KeywordEntry)),
        .keywordCount = 0,
        .keywordCapacity = 1,
        .path = strdup(path),
        .source = source,
        .sourceLength = strlen(source),
        .position = 0,
        .line = 1,
        .column = 1,
        .errors = alloc(sizeof(LexerError)),
        .errorCount = 0,
        .errorCapacity = 1
    };

    assertAlloc(lexer.source);

    return lexer;
}

void addKeyword(Lexer *lexer, const char *keyword, TokenType type) {
    if (lexer->keywordCount >= lexer->keywordCapacity) {
        lexer->keywordCapacity *= 2;
        lexer->keywords = realloc(lexer->keywords, lexer->keywordCapacity * sizeof(KeywordEntry));
        assertAlloc(lexer->keywords);
    }
    lexer->keywords[lexer->keywordCount++] = (KeywordEntry){ .keyword = strdup(keyword), .type = type };
}

void registerLexerKeywords(Lexer *lexer) {
    if (!lexer) return;

    addKeyword(lexer, "pub", TOKEN_PUB);
    addKeyword(lexer, "fn", TOKEN_FN);
    addKeyword(lexer, "ret", TOKEN_RET);
    addKeyword(lexer, "exec", TOKEN_EXEC);
}

void registerVmKeywords(Lexer *lexer) {
    if (!lexer) return;

    addKeyword(lexer, "define", TOKEN_DEFINE);
    addKeyword(lexer, "public", TOKEN_PUBLIC);
    addKeyword(lexer, "function", TOKEN_FUNCTION);
    addKeyword(lexer, "none", TOKEN_NONE);
    addKeyword(lexer, "push", TOKEN_PUSH);
    addKeyword(lexer, "pop", TOKEN_POP);
    addKeyword(lexer, "const", TOKEN_CONST);
    addKeyword(lexer, "ret", TOKEN_RET);
    addKeyword(lexer, "exec", TOKEN_EXEC);
}

void freeLexer(Lexer *lexer) {
    if (!lexer) return;

    FREE_ALLOC(lexer->path);
    FREE_ALLOC(lexer->source);

    for (size_t i = 0; i < lexer->count; i++) {
        FREE_ALLOC(lexer->tokens[i].lexeme);
    }
    FREE_ALLOC(lexer->tokens);

    for (size_t i = 0; i < lexer->errorCount; i++) {
        FREE_ALLOC(lexer->errors[i].message);
    }
    FREE_ALLOC(lexer->errors);

    for (size_t i = 0; i < lexer->keywordCount; i++) {
        FREE_ALLOC(lexer->keywords[i].keyword);
    }
    FREE_ALLOC(lexer->keywords);
}

static inline void advance(Lexer *lexer) {
    if (!lexer) return;

    lexer->position++;
    lexer->column++;
}

static inline void recede(Lexer *lexer) {
    if (!lexer) return;

    lexer->position--;
}

static inline bool isEnd(Lexer *lexer) {
    if (!lexer) return true;
    return lexer->position >= lexer->sourceLength;
}

static inline char currentChar(Lexer *lexer) {
    if (!lexer) return '\0';
    if (lexer->position >= lexer->sourceLength) return '\0';

    return lexer->source[lexer->position];
}

static void skipWhitespace(Lexer *lexer) {
    while (currentChar(lexer) == ' ' || currentChar(lexer) == '\t' ||
           currentChar(lexer) == '\r') {
        advance(lexer);
    }
}

static inline Token newToken(Lexer *lexer, TokenType type, const char *lexeme) {
    return (Token) {
        .type = type,
        .lexeme = strdup(lexeme),
        .line = lexer->line,
        .column = lexer->column
    };
}

static TokenType getKeywordType(Lexer *lexer, const char *lexeme) {
    for (size_t i = 0; i < lexer->keywordCount; i++) {
        if (strcmp(lexeme, lexer->keywords[i].keyword) == 0) {
            return lexer->keywords[i].type;
        }
    }
    
    return TOKEN_IDENTIFIER;
}

static Token tokenizeSymbol(Lexer *lexer) {
    char ch = currentChar(lexer);
    switch (ch) {
        case ':':
            return newToken(lexer, TOKEN_COLON, ":");
        case '(':
            return newToken(lexer, TOKEN_LEFT_PAREN, "(");
        case ')':
            return newToken(lexer, TOKEN_RIGHT_PAREN, ")");
        case '{':
            return newToken(lexer, TOKEN_LEFT_BRACE, "{");
        case '}':
            return newToken(lexer, TOKEN_RIGHT_BRACE, "}");
        case '@':
            return newToken(lexer, TOKEN_AT, "@");
        default:
            return newToken(lexer, TOKEN_EOF, "");
    }
}

static int isAlnumOrUnderscore(int c) {
    return isalnum(c) || c == '_';
}

static Token tokenizeIdentifier(Lexer *lexer) {
    size_t start = lexer->position;
    while (isAlnumOrUnderscore(currentChar(lexer)) && !isEnd(lexer)) {
        advance(lexer);
    }

    size_t len = lexer->position - start;
    char *lexeme = malloc(len + 1);
    assertAlloc(lexeme);

    strncpy(lexeme, lexer->source + start, len);
    lexeme[len] = '\0';

    recede(lexer);
    
    TokenType type = getKeywordType(lexer, lexeme);
    Token token = newToken(lexer, type, lexeme);
    free(lexeme);

    return token;
}

static Token tokenizeInteger(Lexer *lexer) {
    size_t start = lexer->position;
    while (isdigit(currentChar(lexer)) && !isEnd(lexer)) {
        advance(lexer);
    }

    size_t len = lexer->position - start;
    char *lexeme = malloc(len + 1);
    assertAlloc(lexeme);

    strncpy(lexeme, lexer->source + start, len);
    lexeme[len] = '\0';

    recede(lexer);
    
    Token token = newToken(lexer, TOKEN_INTEGER_LITERAL, lexeme);
    free(lexeme);

    return token;
}

static Token tokenize(Lexer *lexer) {
    char ch = currentChar(lexer);
    
    if (isalpha(ch) || ch == '_') {
        return tokenizeIdentifier(lexer);
    } else if (isdigit(ch)) {
        return tokenizeInteger(lexer);
    } else if (ch == '\n') {
        return newToken(lexer, TOKEN_NEWLINE, "\\n");
    }

    return tokenizeSymbol(lexer);
}

static void addToken(Lexer *lexer, Token token) {
    if (lexer->count >= lexer->capacity) {
        lexer->capacity *= 2;
        lexer->tokens = realloc(lexer->tokens, lexer->capacity * sizeof(Token));
        assertAlloc(lexer->tokens);
    }
    lexer->tokens[lexer->count++] = token;
}

void lexerTokenize(Lexer *lexer) {
    if (!lexer) return;

    while (!isEnd(lexer)) {
        skipWhitespace(lexer);
        
        if (isEnd(lexer)) break;
        
        Token token = tokenize(lexer);
        if (token.type == TOKEN_NEWLINE) {
            lexer->line++;
            lexer->column = 0;
        }

        addToken(lexer, token);

        advance(lexer);
    }
}

void printTokens(Lexer *lexer) {
    if (!lexer) return;
    
    printf("=== Lexer Output (%zu tokens) ===\n", lexer->count);
    for (size_t i = 0; i < lexer->count; i++) {
        Token token = lexer->tokens[i];
        const char *tokenName = getTokenTypeName(token.type);
        printf("[%zu] %s: '%s' (Line: %d, Column: %d)\n",
               i, tokenName, token.lexeme, token.line, token.column);
    }
    printf("=== End Lexer Output ===\n");
}