#ifndef runtime_h
#define runtime_h

#include <stdbool.h>

#include "../parser/lexer.h"

typedef struct {
    const char *path;
    bool debug;
} Runtime;

Runtime newRuntime(const char *path, bool debug);
void run(Runtime *runtime);

void freeRuntime(Runtime *runtime);

#endif