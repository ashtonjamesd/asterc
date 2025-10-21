#ifndef ast_h
#define ast_h

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    AST_NODE_FN,
    AST_NODE_ERR,
    AST_NODE_RET,
    AST_NODE_BLOCK,
    AST_NODE_EXEC,
    AST_NODE_INTEGER_LITERAL,
} AstNodeType;

typedef struct AstNode AstNode;

typedef struct {
    uint8_t byte;
} AstExec;

typedef struct {
    AstNode **statements;
    size_t statementCount;
} AstBlock;

typedef struct {
    int value;
} AstIntegerLiteral;

typedef struct {
    char *fnName;
    bool isPublic;
    char *returnType;
    AstBlock block;
} AstFnNode;

typedef struct {
    AstNode *expression;
} AstRet;

typedef struct {
    int dummy;
} AstErrNode;

struct AstNode {
    AstNodeType type;

    union {
        AstFnNode asFn;
        AstErrNode err;
        AstRet asRet;
        AstBlock asBlock;
        AstExec asExec;
        AstIntegerLiteral asInt;
    };
};

typedef struct {
    AstNode **nodes;
    size_t count;
    size_t capacity;
} Ast;

AstNode *newFnNode(const char *name, bool isPublic, const char *returnType, AstBlock block);
AstNode *newRetNode(AstNode *expression);
AstNode *newBlockNode(AstNode **statements, size_t statementCount);
AstNode *newIntegerNode(int value);
AstNode *newExecNode(uint8_t byte);
AstNode *newErrNode(void);

void freeAstNode(AstNode *node);

void printAstNode(const AstNode *node, int indent);
void printAllAstNodes(AstNode **nodes, size_t count);

#endif