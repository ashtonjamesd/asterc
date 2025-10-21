#include "token.h"

const char *getTokenTypeName(TokenType type) {
    switch (type) {
        case TOKEN_PUB: return "PUB";
        case TOKEN_FN: return "FN";
        case TOKEN_RET: return "RET";
        case TOKEN_COLON: return "COLON";
        case TOKEN_LEFT_PAREN: return "LEFT_PAREN";
        case TOKEN_RIGHT_PAREN: return "RIGHT_PAREN";
        case TOKEN_LEFT_BRACE: return "LEFT_BRACE";
        case TOKEN_RIGHT_BRACE: return "RIGHT_BRACE";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_INTEGER_LITERAL: return "INTEGER_LITERAL";
        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_EOF: return "EOF";
        case TOKEN_DEFINE: return "TOKEN_DEFINE";
        case TOKEN_PUBLIC: return "TOKEN_PUBLIC";
        case TOKEN_FUNCTION: return "TOKEN_FUNCTION";
        case TOKEN_NONE: return "TOKEN_NONE";
        case TOKEN_PUSH: return "TOKEN_PUSH";
        case TOKEN_POP: return "TOKEN_POP";
        case TOKEN_CONST: return "TOKEN_CONST";
        case TOKEN_AT: return "TOKEN_AT";
        case TOKEN_EXEC: return "TOKEN_EXEC";
        default: return "UNKNOWN";
    }
}