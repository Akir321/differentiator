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

#define EV_L canBeEvaluated(node->left)
#define EV_R canBeEvaluated(node->right)

#define VALUE_0(node) (canBeEvaluated(node) && equalDouble(expTreeEvaluate(eval, node, &error), 0))
#define VALUE_1(node) (canBeEvaluated(node) && equalDouble(expTreeEvaluate(eval, node, &error), 1))


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

int expTreeSimplify(Evaluator *eval, Node *node)
{
    assert(eval);

    int changeCount = 0;
    int prevCount   = -1;

    while (changeCount > prevCount)
    {
        prevCount = changeCount;

        changeCount += expTreeSimplifyConsts     (eval, node);
        changeCount += expTreeSimplifyNeutralElem(eval, node);
    }

    return EXIT_SUCCESS;
}

#define CHANGED 1

int expTreeSimplifyConsts(Evaluator *eval, Node *node)
{
    CHECK_POISON_PTR(node);
    
    if (!node)  return EXIT_SUCCESS;

    if (node->type == EXP_TREE_NUMBER)   return EXIT_SUCCESS;
    if (node->type == EXP_TREE_VARIABLE) return EXIT_SUCCESS;
    if (node->type == EXP_TREE_NOTHING)  return EXIT_FAILURE;

    int count = 0;
    count += expTreeSimplifyConsts(eval, node->left);
    count += expTreeSimplifyConsts(eval, node->right);

    if (EV_L && EV_R)
    {
        ExpTreeErrors error = NO_ERROR;
        double left  = expTreeEvaluate(eval, node->left,  &error);
        double right = expTreeEvaluate(eval, node->right, &error);
        if (error) return error;

        subTreeDtor(node->left);
        subTreeDtor(node->right);
        node->data.number  = NodeCalculate(left, right, node->data.operatorNum, &error);
        if (error) return error;

        node->type = EXP_TREE_NUMBER;
        node->left  = NULL;
        node->right = NULL;

        return CHANGED;
    }
    return EXIT_SUCCESS;
}

int expTreeSimplifyNeutralElem(Evaluator *eval, Node *node)
{
    CHECK_POISON_PTR(node);
    
    if (!node)  return EXIT_SUCCESS;

    if (node->type == EXP_TREE_NUMBER)   return EXIT_SUCCESS;
    if (node->type == EXP_TREE_VARIABLE) return EXIT_SUCCESS;
    if (node->type == EXP_TREE_NOTHING)  return NODE_TYPE_NOTHING;

    int count = 0;

    count += tryNodeSimplify(eval, node);

    count += expTreeSimplifyNeutralElem(eval, node->left);
    count += expTreeSimplifyNeutralElem(eval, node->right);

    return count;
}

#define COUNT(func) if (func > 0) return CHANGED

int tryNodeSimplify(Evaluator *eval, Node *node)
{
    assert(eval);
    CHECK_POISON_PTR(node);

    switch (node->data.operatorNum)
    {
        case ADD: case SUB:
        {
            COUNT(casePlus0(eval, node, node->left,  node->right));
            COUNT(casePlus0(eval, node, node->right, node->left));
            return EXIT_SUCCESS;
        }
        case MUL:
        {
            COUNT(caseTimes0(eval, node, node->left));
            COUNT(caseTimes0(eval, node, node->right));
            COUNT(caseTimes1(eval, node, node->left,  node->right));
            COUNT(caseTimes1(eval, node, node->right, node->left));
            return EXIT_SUCCESS;
        }
        case DIV:
        {
            COUNT(caseTimes0(eval, node, node->left));
            COUNT(caseTimes1(eval, node, node->left,  node->right));
            COUNT(caseTimes1(eval, node, node->right, node->left));
            return EXIT_SUCCESS;
        }
        default:
            printf("ERROR: unknown operator: %d\n", node->data.operatorNum);
            return UNKNOWN_OPERATOR;
    }
}

#undef COUNT

int casePlus0(Evaluator *eval, Node *node, Node *zero, Node *savedNode) 
{
    CHECK_POISON_PTR(zero);
    CHECK_POISON_PTR(savedNode);

    ExpTreeErrors error = NO_ERROR;

    if (VALUE_0(zero))                           
    {   
        if (error) return error;                                       
        subTreeDtor(zero);               
        *node = *savedNode;               
        destroyNode(&savedNode);

        return CHANGED;         
    }

    return EXIT_SUCCESS;
}

int caseTimes0(Evaluator *eval, Node *node, Node *zero)
{   
    ExpTreeErrors error = NO_ERROR;

    if (VALUE_0(zero)) 
    {   if (error) return error;                                             
        subTreeDtor(node->left);                     
        subTreeDtor(node->right);                    
                                                     
        node->type = EXP_TREE_NUMBER;                
        node->data.number = 0;                       
        node->left  = NULL;                          
        node->right = NULL;

        return CHANGED;                        
    }

    return EXIT_SUCCESS;
}

int caseTimes1(Evaluator *eval, Node *node, Node *one, Node *savedNode) 
{
    CHECK_POISON_PTR(one);
    CHECK_POISON_PTR(savedNode);

    ExpTreeErrors error = NO_ERROR;

    if (VALUE_1(one))                           
    {   
        if (error) return error;                                       
        subTreeDtor(one);               
        *node = *savedNode;               
        destroyNode(&savedNode);

        return CHANGED;       
    }

    return EXIT_SUCCESS;
}

#undef CHANGED
