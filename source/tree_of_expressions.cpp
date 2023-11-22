#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "tree_of_expressions.h"
#include "tree_graphic_dump.h"
#include "html_logfile.h"

#define CHECK_POISON_PTR(ptr) \
    assert(ptr != PtrPoison)
#undef  CHECK_POISON_PTR

#define CHECK_POISON_PTR(ptr) \
    if (ptr == PtrPoison)     \
    {                         \
        LOG("ERROR: PoisonPtr detected in %s(%d) %s\n", __FILE__, __LINE__, __func__);\
        return 0;                                                                        \
    }

Node *createNode(ExpTreeNodeType type, ExpTreeData data, Node *left, Node *right)
{
    Node *node = (Node *)calloc(1, sizeof(Node));
    if (!node) return NULL;

    node->type  = type;
    node->data  = data;

    node->left  = left;
    node->right = right;

    return node;
}

int destroyNode(Node **nodePtr)
{
    assert(nodePtr);

    Node *node = *nodePtr;
    CHECK_POISON_PTR(node);

    node->type        = EXP_TREE_NOTHING;
    node->data.number = DataPoison;
    node->left        = PtrPoison;
    node->right       = PtrPoison;

    free(node);
    *nodePtr = PtrPoison;

    return EXIT_SUCCESS;
}

int treeCtor(Tree *tree, Node *root)
{
    CHECK_POISON_PTR(root);
    assert(root);

    tree->root = root;
    tree->size = treeSize(root);

    return EXIT_SUCCESS;
}

int treeSize(Node *root)
{
    CHECK_POISON_PTR(root);
    if (root == NULL) return 0;

    return treeSize(root->left) + treeSize(root->right);
}

int treeDtor(Node *root)
{
    CHECK_POISON_PTR(root);
    if (root == NULL) return 0;

    treeDtor(root->left);
    treeDtor(root->right);
    destroyNode(&root);

    return EXIT_SUCCESS;
}

double expTreeEvaluate(Node *root, ExpTreeErrors *error)
{
    CHECK_POISON_PTR(root);
    
    if (!root)                         return 0;
    if (root->type == EXP_TREE_NUMBER) return root->data.number;

    double leftTree  = expTreeEvaluate(root->left,  error);
    LOG("leftTree = %lg\n", leftTree);
    double rightTree = expTreeEvaluate(root->right, error);
    LOG("rightTree = %lg\n", rightTree);

    if (*error) return DataPoison;

    double nodeValue = NodeCalculate(leftTree, rightTree, root->data.operatorNum, error);
    LOG("nodeValue = %lg\n", nodeValue);
    return nodeValue;
}

double NodeCalculate(double leftTree, double rightTree, 
                     ExpTreeOperators operatorType, ExpTreeErrors *error)
{
    switch (operatorType)
    {
        case ADD:
            return leftTree + rightTree;

        case SUB:
            return leftTree - rightTree;

        case MUL:
            return leftTree * rightTree;

        case DIV:
            if (equalDouble(rightTree, 0)) 
            { 
                *error = DIVISION_BY_ZERO; 
                return DataPoison; 
            }
            return leftTree / rightTree;

        default:
            *error = UNKNOWN_OPERATOR;
            return DataPoison;
    }
    return DataPoison;
}

bool equalDouble(double a, double b)
{
    return fabs(a - b) < PrecisionConst;
}
