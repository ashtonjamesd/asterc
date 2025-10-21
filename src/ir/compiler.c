#include "compiler.h"
#include "../util/alloc.h"

static void compileNode(Compiler *c, AstNode *node);

Compiler newCompiler(Ast ast) {
    Compiler c = {
        .ast = ast,
        .out = fopen("out.air", "w")
    };

    assertAlloc(c.out);

    return c;
}

void freeCompiler(Compiler *c) {
    fclose(c->out);
}

static inline void emit(Compiler *c, char *emit) {
    fprintf(c->out, "%s", emit);
}

static inline void emitSpace(Compiler *c) {
    emit(c, " ");
}

static inline void emitSemicolon(Compiler *c) {
    emit(c, ";");
}

static inline void emitTab(Compiler *c) {
    emit(c, "\t");
}

static inline void emitNewline(Compiler *c) {
    emit(c, "\n");
}

static inline void emitIdentifier(Compiler *c, char *ident) {
    fprintf(c->out, "@%s", ident);
}

static inline void emitColon(Compiler *c) {
    emit(c, ":");
}

static inline void emitLeftBrace(Compiler *c) {
    emit(c, "{");
}

static inline void emitRightBrace(Compiler *c) {
    emit(c, "}");
}

static inline void emitLeftParen(Compiler *c) {
    emit(c, "(");
}

static inline void emitRightParen(Compiler *c) {
    emit(c, ")");
}

static void compileFnNode(Compiler *c, AstFnNode *fnNode) {
    emit(c, "define");
    emitSpace(c);

    if (fnNode->isPublic) {
        emit(c, "public");
        emitSpace(c);
    }

    emit(c, "function");
    emitSpace(c);
    emitIdentifier(c, fnNode->fnName);
    
    emitLeftParen(c);
    
    if (1) {
        emit(c, "none");
    }

    emitRightParen(c);

    emitColon(c);
    emitSpace(c);

    emit(c, fnNode->returnType);
    emitSpace(c);
    
    emitLeftBrace(c);
    emitNewline(c);

    for (size_t i = 0; i < fnNode->block.statementCount; i++) {
        AstNode *stmt = fnNode->block.statements[i];
        compileNode(c, stmt);
    }

    emitRightBrace(c);
    emitNewline(c);
}

static void compileIntegerNode(Compiler *c, AstIntegerLiteral *intNode) {
    emitTab(c);

    emit(c, "push");
    emitSpace(c);

    emit(c, "const");
    emitSpace(c);

    emit(c, "i32");
    emitColon(c);
    emitSpace(c);

    fprintf(c->out, "%d", intNode->value);
    emitNewline(c);
}

static void compileExpression(Compiler *c, AstNode *expression) {
    switch (expression->type) {
        case AST_NODE_INTEGER_LITERAL:
            compileIntegerNode(c, &expression->asInt);
            break;
        default:
            break;
    }
}

static void compileRetNode(Compiler *c, AstRet *retNode) {
    if (retNode->expression) {
        compileExpression(c, retNode->expression);
    }
    
    emitTab(c);
    emit(c, "ret");
    emitNewline(c);
}

static void compileExecNode(Compiler *c, AstExec *execNode) {
    emitTab(c);

    emit(c, "exec");
    emitSpace(c);

    fprintf(c->out, "%d", execNode->byte);
    emitNewline(c);
}

static void compileNode(Compiler *c, AstNode *node) {
    switch (node->type) {
        case AST_NODE_FN: {
            compileFnNode(c, &node->asFn);
            break;
        }
        case AST_NODE_RET: {
            compileRetNode(c, &node->asRet);
            break;
        }
        case AST_NODE_INTEGER_LITERAL: {
            compileIntegerNode(c, &node->asInt);
            break;
        }
        case AST_NODE_EXEC: {
            compileExecNode(c, &node->asExec);
        }
        case AST_NODE_ERR: {
            break;
        }
        default: {
            break;
        }
    }
}

void compile(Compiler *c) {
    for (size_t i = 0; i < c->ast.count; i++) {
        AstNode *node = c->ast.nodes[i];
        compileNode(c, node);
    }
}