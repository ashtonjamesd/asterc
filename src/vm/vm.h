#ifndef vm_h
#define vm_h

#include <stdint.h>
#include <stdbool.h>

#include "../assembler/assembler.h"
#include "../assembler/object.h"

typedef struct {
    uint16_t *addresses;
    uint16_t top;
} CallStack;

typedef struct {
    Object *values;
    uint16_t top;
} Stack;

typedef struct {
    Program program;
    uint16_t pc;
    bool running;
    Stack stack;
    CallStack callStack;
} AVM;

AVM newAVM(Program program);
void freeAVM(AVM *vm);

void execute(AVM *vm);

#endif