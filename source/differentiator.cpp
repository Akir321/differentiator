#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "tree_of_expressions.h"
#include "exp_tree_write.h"
#include "tree_graphic_dump.h"
#include "html_logfile.h"
#include "differentiator.h"

#define CHECK_POISON_PTR(ptr) \
    if (ptr == PtrPoison)     \
    {                         \
        LOG("ERROR: PoisonPtr detected in %s(%d) %s\n", __FILE__, __LINE__, __func__);\
        return 0;                                                                     \
    }

#define NEW_NODE(type, value, left, right)                     \
    createNode(type, createNodeData(type, value), left, right)

#define cL     copy      (eval, node->left)
#define cR     copy      (eval, node->right)

#define dL     derivative(eval, node->left)
#define dR     derivative(eval, node->right)

#define _ADD(left, right) NEW_NODE(EXP_TREE_OPERATOR, ADD, left, right)

#define _SUB(left, right) NEW_NODE(EXP_TREE_OPERATOR, SUB, left, right)

#define _MUL(left, right) NEW_NODE(EXP_TREE_OPERATOR, MUL, left, right)

#define _DIV(left, right) NEW_NODE(EXP_TREE_OPERATOR, DIV, left, right)


Node *copy(Evaluator *eval, Node *node)
{
    assert(eval);
    CHECK_POISON_PTR(node);
    if (!node) return NULL;

    return createNode(node->type, node->data, cL, cR);
}

Node *derivative(Evaluator *eval, Node *node)
{
    assert(eval);
    assert(node);
    CHECK_POISON_PTR(node);

    switch (node->type)
    {
        case EXP_TREE_NUMBER:       return NEW_NODE(EXP_TREE_NUMBER, 0, NULL, NULL);
    
        case EXP_TREE_VARIABLE:     return NEW_NODE(EXP_TREE_NUMBER, 1, NULL, NULL);
    
        case EXP_TREE_OPERATOR:     return processDifOperator(eval, node);

        case EXP_TREE_NOTHING:      printf("ERROR: node type: nothing\n");
                                    return PtrPoison;
    
        default:                    printf("ERROR: unknown node type: %d\n", node->type);
                                    return PtrPoison;
        
    }
}

Node *processDifOperator(Evaluator *eval, Node *node)
{
    assert(eval);
    assert(node);

    switch (node->data.operatorNum)
    {
        case ADD:   return _ADD(dL, dR);

        case SUB:   return _SUB(dL, dR);
        
        case MUL:   return _ADD(_MUL(dL, cR), _MUL(cL, dR));

        case DIV:   return _DIV(_SUB(_MUL(dL, cR), _MUL(cL, dR)), _MUL(cR, cR));
        
        default:    printf("ERROR: unknown operator: %d\n", node->data.operatorNum);
                    return PtrPoison;
    }
}
