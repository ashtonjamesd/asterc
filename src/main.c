#include <stdio.h>
#include <stdlib.h>

#include "runtime/runtime.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <source-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Runtime aster = newRuntime(argv[1], false);
    run(&aster);

    freeRuntime(&aster);

    return EXIT_SUCCESS;
}