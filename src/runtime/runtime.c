#include "runtime.h"

#include "../parser/lexer.h"
#include "../parser/parser.h"
#include "../ir/compiler.h"
#include "../assembler/assembler.h"
#include "../vm/vm.h"

Runtime newRuntime(const char *path, bool debug) {
    Runtime runtime = {
        .path = path,
        .debug = debug,
    };
    
    return runtime;
}

void run(Runtime *runtime) {
    if (!runtime) return;

    Lexer lexer = newLexer(runtime->path);
    registerLexerKeywords(&lexer);
    lexerTokenize(&lexer);
    if (runtime->debug) printTokens(&lexer);

    Parser parser = newParser(lexer.tokens, lexer.count);
    parseAst(&parser);
    if (runtime->debug) printParserAst(&parser);

    Compiler compiler = newCompiler(parser.ast);
    compile(&compiler);
    freeCompiler(&compiler);

    Assembler assembler = newAssembler(runtime->debug);
    assemble(&assembler);

    AVM vm = newAVM(assembler.program);
    execute(&vm);

    freeAVM(&vm);
    freeAssembler(&assembler);
    freeParser(&parser);
    freeLexer(&lexer);
}

void freeRuntime(Runtime *runtime) {
    if (!runtime) return;
}