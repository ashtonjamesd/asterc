#ifndef lexer_h
#define lexer_h

#include <stddef.h>

#include "token.h"
#include "error.h"

typedef struct {
    const char *keyword;
    TokenType type;
} KeywordEntry;

typedef struct {
    Token *tokens;
    size_t count;
    size_t capacity;

    KeywordEntry *keywords;
    size_t keywordCount;
    size_t keywordCapacity;

    char *path;
    char *source;
    size_t sourceLength;
    size_t position;
    uint16_t line;
    uint16_t column;

    LexerError *errors;
    size_t errorCount;
    size_t errorCapacity;
} Lexer;

Lexer newLexer(const char *source);
void freeLexer(Lexer *lexer);

void lexerTokenize(Lexer *lexer);
void printTokens(Lexer *lexer);

void registerLexerKeywords(Lexer *lexer);
void registerVmKeywords(Lexer *lexer);

#endif