#ifndef assembler_h
#define assembler_h

#include <stdint.h>
#include <stddef.h>

#include "../parser/token.h"
#include "object.h"

typedef struct {
    char *name;
    size_t address;
} Function;

typedef struct {
    Function *entries;
    size_t count;
    size_t capacity;
} FunctionTable;

typedef struct {
    Object *values;
    size_t count;
    size_t capacity;
} ConstantPool;

typedef enum {
    INSTR_PUSH_I32,
    INSTR_RET,
    INSTR_EXEC,
    INSTR_HALT,
    INSTR_PRINT,
    INSTR_CALL,
    INSTR_JMP,
} AvmInstruction;

typedef struct {
    AvmInstruction *code;
    size_t length;
    size_t capacity;

    ConstantPool constants;
    FunctionTable functions;
} Program;

typedef struct {
    Program program;
    Token *tokens;
    size_t tokenCount;

    size_t position;
    bool debug;
} Assembler;

Assembler newAssembler(bool debug);
void freeAssembler(Assembler *assembler);

void assemble(Assembler *assembler);

#endif