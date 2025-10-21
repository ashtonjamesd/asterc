#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "assembler.h"
#include "../parser/lexer.h"
#include "../util/alloc.h"

Assembler newAssembler(bool debug) {
    Assembler assembler = {
        .program = {
            .code = alloc(sizeof(AvmInstruction)),
            .length = 0,
            .capacity = 1,
            .constants = {
                .values = alloc(sizeof(Object)),
                .count = 0,
                .capacity = 1
            },
            .functions = {
                .entries = alloc(sizeof(Function)),
                .count = 0,
                .capacity = 1
            }
        },
        .position = 0,
        .debug = debug
    };

    return assembler;
}

void freeAssembler(Assembler *assembler) {
    if (!assembler) return;

    FREE_ALLOC(assembler->program.code);
}

static void advance(Assembler *a) {
    a->position++;
}

static Token current(Assembler *a) {
    return a->tokens[a->position];
}

static bool match(Assembler *a, TokenType type) {
    return current(a).type == type;
}

static void advanceIfMatch(Assembler *a, TokenType type) {
    if (match(a, type)) advance(a);
}

static inline bool expect(Assembler *a, TokenType type) {
    if (match(a, type)) {
        advance(a);
        return true;
    }

    return false;
}

static inline Token expectOrErr(Assembler *a, TokenType type) {
    if (expect(a, type)) {
        return a->tokens[a->position - 1];
    }

    return (Token){.column = 0, .line = 0, .type = TOKEN_EOF, .lexeme = ""};
}

static bool isErr(Token t) {
    return t.type == TOKEN_EOF;
}

static void emit (Assembler *a, AvmInstruction instr) {
    if (a->program.length >= a->program.capacity) {
        a->program.capacity *= 2;
        a->program.code = realloc(a->program.code, sizeof(AvmInstruction) * a->program.capacity);
        assertAlloc(a->program.code);
    }
    a->program.code[a->program.length++] = instr;
}

static Object newObject(ObjectType type, int32_t i32) {
    return (Object){
        .type = type,
        .i32 = i32,
    };
}

static void addConstant(Assembler *a, Object obj) {
    if (a->program.constants.count >= a->program.constants.capacity) {
        a->program.constants.capacity *= 2;
        a->program.constants.values = realloc(
            a->program.constants.values,
            sizeof(Object) * a->program.constants.capacity
        );
        assertAlloc(a->program.constants.values);
    }

    a->program.constants.values[a->program.constants.count++] = obj;
}

static void emitPush(Assembler *a) {
    expect(a, TOKEN_PUSH);

    if (match(a, TOKEN_CONST)) {
        advance(a);

        Token type = expectOrErr(a, TOKEN_IDENTIFIER);
        if (isErr(type)) return;

        if (!expect(a, TOKEN_COLON)) return;

        Token constant = current(a);
        if (isErr(constant)) return;
        advance(a);

        emit(a, INSTR_PUSH_I32);

        Object obj = newObject(OBJ_I32, atoi(constant.lexeme));
        addConstant(a, obj);

        emit(a, a->program.constants.count - 1);
    }
}

static void emitExec(Assembler *a) {
    expect(a, TOKEN_EXEC);

    Token byte = current(a);
    AvmInstruction instr = atoi(byte.lexeme);

    advance(a);

    emit(a, instr);
}

static void emitRet(Assembler *a) {
    expect(a, TOKEN_RET);
    advance(a);

    emit(a, INSTR_RET);
}

static void emitHalt(Assembler *a) {
    expect(a, TOKEN_HALT);
    advance(a);

    emit(a, INSTR_HALT);
}

static void parseInstructions(Assembler *a) {
    switch (current(a).type) {
        case TOKEN_PUSH: {
            emitPush(a);
            break;
        }
        case TOKEN_RET: {
            emitRet(a);
            break;
        }
        case TOKEN_EXEC: {
            emitExec(a);
            break;
        }
        case TOKEN_HALT: {
            emitHalt(a);
            break;
        }
        default: {
            advance(a);
        }
    }
}

static Function newFunctionEntry(const char *name, size_t address) {
    return (Function){
        .name = strdup(name),
        .address = address,
    };
}

static void addFunctionEntry(Assembler *a, Function func) {
    if (a->program.functions.count >= a->program.functions.capacity) {
        a->program.functions.capacity *= 2;
        a->program.functions.entries = realloc(
            a->program.functions.entries,
            sizeof(Function) * a->program.functions.capacity
        );
        assertAlloc(a->program.functions.entries);
    }

    a->program.functions.entries[a->program.functions.count++] = func;
}

static void parseFunction(Assembler *a) {
    if (!expect(a, TOKEN_AT)) return;

    Token name = expectOrErr(a, TOKEN_IDENTIFIER);
    if (isErr(name)) return;

    if (!expect(a, TOKEN_LEFT_PAREN)) return;
    advanceIfMatch(a, TOKEN_NONE);

    if (!expect(a, TOKEN_RIGHT_PAREN)) return;
    if (!expect(a, TOKEN_COLON)) return;

    Token returnType = expectOrErr(a, TOKEN_IDENTIFIER);
    if (isErr(returnType)) return;

    Function function = newFunctionEntry(name.lexeme, a->program.length);
    addFunctionEntry(a, function);

    if (!expect(a, TOKEN_LEFT_BRACE)) return;
    if (!expect(a, TOKEN_NEWLINE)) return;

    while (!match(a, TOKEN_RIGHT_BRACE)) {
        parseInstructions(a);
    }

    if (!expect(a, TOKEN_RIGHT_BRACE)) return;
    if (!expect(a, TOKEN_NEWLINE)) return;
}

static void emitDefine(Assembler *a) {
    expect(a, TOKEN_DEFINE);
    advanceIfMatch(a, TOKEN_PUBLIC);
    
    if (match(a, TOKEN_FUNCTION)) {
        advance(a);
        parseFunction(a);
    }
}

static void parseIrTokens(Assembler *a) {
    switch (current(a).type) {
        case TOKEN_DEFINE: {
            emitDefine(a);
            break;
        }
        default: {
            advance(a);
        }
    }
}

void printBytecode(Program *b) {
    printf("=== Assembler Output (%ld) ===\n", b->length);
    for (size_t i = 0; i < b->length; i++) {
        switch (b->code[i]) {
            case INSTR_PUSH_I32: {
                printf("PUSH CONST I32: %d", b->code[++i]);
                break;
            }
            case INSTR_RET: {
                printf("RET");
                break;
            }
            case INSTR_EXEC: {
                printf("EXEC");
                break;
            }
            case INSTR_HALT: {
                printf("HALT");
                break;
            }
            case INSTR_PRINT: {
                printf("PRINT");
                break;
            }
            case INSTR_CALL: {
                printf("CALL: %d", b->code[++i]);
                break;
            }
            case INSTR_JMP: {
                printf("JMP: %d", b->code[++i]);
                break;
            }
            default: {
                printf("%s %d", "Unknown program op: ", b->code[i]);
            }
        }
        printf("\n");
    }
    printf("=== End Assembler Output (%ld) ===\n", b->length);
}

static void printFunctionTable(Program *p) {
    printf("=== Function Table (%ld) ===\n", p->functions.count);
    for (size_t i = 0; i < p->functions.count; i++) {
        Function func = p->functions.entries[i];
        printf("Function: '%s' at address: %zu\n", func.name, func.address);
    }
    printf("=== End Function Table (%ld) ===\n", p->functions.count);
}

void assemble(Assembler *a) {
    if (!a) return;

    Lexer lexer = newLexer("out.air");
    registerVmKeywords(&lexer);
    lexerTokenize(&lexer);

    a->tokens = lexer.tokens;
    a->tokenCount = lexer.count;

    while (a->position < a->tokenCount) {
        parseIrTokens(a);
    }

    if (a->debug) printTokens(&lexer);
    if (a->debug) printBytecode(&a->program);
    if (a->debug) printFunctionTable(&a->program);

    freeLexer(&lexer);
}