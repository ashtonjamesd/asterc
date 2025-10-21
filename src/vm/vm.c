#include <stdio.h>
#include <string.h>

#include "vm.h"
#include "../util/alloc.h"

AVM newAVM(Program program) {
    AVM vm = {
        .program = program,
        .pc = 0,
        .running = true,
        .stack = {
            .values = alloc(1024 * sizeof(Object)),
            .top = 0
        },
        .callStack = {
            .addresses = alloc(1024 * sizeof(uint16_t)),
            .top = 0
        }
    };
    
    return vm;
}

void freeAVM(AVM *vm) {
    if (!vm) return;
}

static void tick(AVM *vm) {
    vm->pc++;
}

static void avmInternalError(AVM *vm) {
    fprintf(stderr, "An internal error occurred in the AVM\n");
    vm->running = false;
}

// static void avmUnknownError(AVM *vm) {
//     fprintf(stderr, "An unknown error occurred in the AVM\n");
//     vm->running = false;
// }

static void avmStackoverflow(AVM *vm) {
    fprintf(stderr, "A stackoverflow error occurred in the AVM\n");
    vm->running = false;
}

// static void avmRuntimeError(AVM *vm) {
//     fprintf(stderr, "A runtime error occurred in the AVM\n");
//     vm->running = false;
// }

static void execPush(AVM *vm) {
    if (!vm) return;
    tick(vm);

    if (vm->stack.top >= 1024) {
        avmStackoverflow(vm);
        return;
    }

    uint8_t value = vm->program.code[vm->pc];
    tick(vm);

    Object constant = vm->program.constants.values[value];

    Object obj = {
        .type = OBJ_I32,
        .i32 = constant.i32,
    };

    vm->stack.values[vm->stack.top++] = obj;
}

static void execPrint(AVM *vm) {
    if (!vm) return;
    tick(vm);

    if (vm->stack.top == 0) {
        fprintf(stderr, "Stack underflow on PRINT\n");
        vm->running = false;
        return;
    }

    Object obj = vm->stack.values[vm->stack.top - 1];
    if (obj.type == OBJ_I32) {
        printf("%d\n", obj.i32);
    } else {
        printf("Unknown object type on PRINT\n");
    }
}

static void execCall(AVM *vm) {
    if (!vm) return;
    tick(vm);

    uint8_t funcIndex = vm->program.code[vm->pc];
    tick(vm);

    Function func = vm->program.functions.entries[funcIndex];

    if (vm->callStack.top >= 1024) {
        avmStackoverflow(vm);
        return;
    }

    vm->callStack.addresses[vm->callStack.top++] = vm->pc;
    vm->pc = func.address;
}

static void execRet(AVM *vm) {
    if (!vm) return;
    tick(vm);

    if (vm->callStack.top == 0) {
        fprintf(stderr, "Call stack underflow on RET\n");
        vm->running = false;
        return;
    }

    vm->pc = vm->callStack.addresses[--vm->callStack.top];
}

static void execInstr(AVM *vm, AvmInstruction instr) {
    switch (instr) {
        case INSTR_PUSH_I32: {
            execPush(vm);
            break;
        }
        case INSTR_RET: {
            execRet(vm);
            break;
        }
        case INSTR_EXEC: {
            tick(vm);
            break;
        }
        case INSTR_HALT: {
            vm->running = false;
            tick(vm);
            printf("Program halted.\n");
            break;
        }
        case INSTR_PRINT: {
            execPrint(vm);
            break;
        }
        case INSTR_CALL: {
            execCall(vm);
            break;
        }
        default: {
            avmInternalError(vm);
        }
    }
}

void execute(AVM *vm) {
    if (!vm) return;

    for (size_t i = 0; i < vm->program.functions.count; i++) {
        Function func = vm->program.functions.entries[i];
        if (strcmp(func.name, "main") == 0) {
            vm->callStack.addresses[vm->callStack.top++] = vm->program.length;
            vm->pc = func.address;
            break;
        }
    }

    while (vm->running && vm->pc < vm->program.length) {
        execInstr(vm, vm->program.code[vm->pc]);
    }

    Object topObj = vm->stack.values[vm->stack.top - 1];
    if (topObj.type == OBJ_I32) {
        printf("\nVM execution finished: %d\n", topObj.i32);
    }
}