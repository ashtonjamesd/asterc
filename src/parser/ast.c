#include <string.h>
#include <stdio.h>

#include "../util/alloc.h"
#include "ast.h"

static AstNode *newAstNode(AstNodeType type) {
    AstNode *node = alloc(sizeof(AstNode));
    node->type = type;

    return node;
}

AstNode *newFnNode(const char *name, bool isPublic, const char *returnType, AstBlock block) {
    AstNode *node = newAstNode(AST_NODE_FN);
    node->asFn.fnName = strdup(name);
    node->asFn.isPublic = isPublic;
    node->asFn.returnType = strdup(returnType);
    node->asFn.block = block;

    assertAlloc(node->asFn.fnName);
    assertAlloc(node->asFn.returnType);

    return node;
}

AstNode *newRetNode(AstNode *expression) {
    AstNode *node = newAstNode(AST_NODE_RET);
    node->asRet.expression = expression;

    return node;
}

AstNode *newBlockNode(AstNode **statements, size_t statementCount) {
    AstNode *node = newAstNode(AST_NODE_BLOCK);
    node->asBlock.statements = statements;
    node->asBlock.statementCount = statementCount;

    return node;
}

AstNode *newIntegerNode(int value) {
    AstNode *node = newAstNode(AST_NODE_INTEGER_LITERAL);
    node->asInt.value = value;

    return node;
}

AstNode *newExecNode(uint8_t byte) {
    AstNode *node = newAstNode(AST_NODE_EXEC);
    node->asExec.byte = byte;

    return node;
}

AstNode *newErrNode(void) {
    AstNode *node = newAstNode(AST_NODE_ERR);
    
    return node;
}

void freeAstNode(AstNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_NODE_FN:
            FREE_ALLOC(node->asFn.fnName);
            break;
        case AST_NODE_RET:
            freeAstNode(node->asRet.expression);
            break;
        case AST_NODE_BLOCK:
            for (size_t i = 0; i < node->asBlock.statementCount; i++) {
                freeAstNode(node->asBlock.statements[i]);
            }
            FREE_ALLOC(node->asBlock.statements);
            break;
        case AST_NODE_EXEC:
            break;
        case AST_NODE_INTEGER_LITERAL:
            break;
        case AST_NODE_ERR:
            break;
    }

    FREE_ALLOC(node);
}

static void printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

void printAstNode(const AstNode *node, int indent) {
    if (!node) {
        printIndent(indent);
        printf("(null node)\n");
        return;
    }

    printIndent(indent);
    
    switch (node->type) {
        case AST_NODE_FN: {
            printf("FnNode: %s\n", node->asFn.fnName ? node->asFn.fnName : "(unnamed)");
            for (size_t i = 0; i < node->asFn.block.statementCount; i++) {
                printAstNode(node->asFn.block.statements[i], indent + 1);
            }
            break;
        }
        case AST_NODE_RET: {
            printf("RetNode:\n");
            if (node->asRet.expression) {
                printAstNode(node->asRet.expression, indent + 1);
            }
            break;
        }
        case AST_NODE_BLOCK: {
            printf("BlockNode (%zu statements):\n", node->asBlock.statementCount);
            for (size_t i = 0; i < node->asBlock.statementCount; i++) {
                printAstNode(node->asBlock.statements[i], indent + 1);
            }
            break;
        }
        case AST_NODE_INTEGER_LITERAL: {
            printf("IntegerLiteral: %d\n", node->asInt.value);
            break;
        }
        case AST_NODE_ERR: {
            printf("ErrorNode\n");
            break;
        }
        case AST_NODE_EXEC: {
            printf("Exec: %d\n", node->asExec.byte);
            break;
        }
        default: {
            printf("UnknownNode (type: %d)\n", node->type);
            break;
        }
    }
}

void printAllAstNodes(AstNode **nodes, size_t count) {
    if (!nodes) return;

    printf("=== AST Output (%zu nodes) ===\n", count);
    
    for (size_t i = 0; i < count; i++) {
        printf("[%zu] ", i);
        printAstNode(nodes[i], 0);
    }
    
    printf("=== End AST Output ===\n");
}