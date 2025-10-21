#ifndef error_h
#define error_h

typedef enum {
    PARSER_ERROR_UNEXPECTED_TOKEN,
} LexerErrorType;

typedef struct {
    LexerErrorType type;
    int line;
    int column;
    const char *message;
} LexerError;

LexerError newLexerError(LexerErrorType type, int line, int column, const char *message);

#endif