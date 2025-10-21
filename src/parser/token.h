#ifndef token_h
#define token_h

#include <stdint.h>

typedef enum {
    // keywords
    TOKEN_PUB,
    TOKEN_FN,
    TOKEN_RET,
    TOKEN_DEFINE,
    TOKEN_PUBLIC,
    TOKEN_FUNCTION,
    TOKEN_NONE,
    TOKEN_PUSH,
    TOKEN_POP,
    TOKEN_CONST,
    TOKEN_EXEC,
    TOKEN_HALT,

    // symbols
    TOKEN_COLON,
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_AT,

    // others
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER_LITERAL,

    TOKEN_NEWLINE,
    TOKEN_EOF,
} TokenType;

typedef struct {
    TokenType type;
    const char* lexeme;
    uint16_t line;
    uint16_t column;
} Token;

const char *getTokenTypeName(TokenType type);

#endif