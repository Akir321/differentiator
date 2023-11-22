#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
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

int nameTableCtor(NameTable *names)
{
    assert(names);

    for (size_t i = 0; i < NamesNumber; i++)
    {
        names->table[i].value = DataPoison;
        names->table[i].name  = NULL;
    }

    return EXIT_SUCCESS;
}

int nameTableDtor(NameTable *names)
{
    assert(names);

    for (size_t i = 0; i < NamesNumber; i++)
    {
        names->table[i].value = DataPoison;

        free(names->table[i].name);
        names->table[i].name = NULL;
    }

    return EXIT_SUCCESS;
}

int nameTableAdd(NameTable *names, const char *name, double value)
{
    assert(names);
    assert(name);

    if (names->count >= NamesNumber) return IndexPoison;

    names->table[names->count].name = strdup(name);
    if (!names->table[names->count].name) return IndexPoison;

    names->table[names->count].value = value;
    names->count++;

    return (int)(names->count - 1);
}

double nameTableFind(NameTable *names, const char *name)
{
    assert(names);
    assert(name);

    for (size_t i = 0; i < names->count; i++)
    {
        if (strcmp(name, names->table[i].name) == 0)
        {
            return (int)(names->table[i].value);
        }
    }

    return IndexPoison;
}

int nameTableDump(NameTable *names, FILE *f)
{
    assert(names);
    
    fprintf(f, "I'm NameTable\n");

    for (size_t i = 0; i < names->count; i++)
    {
        fprintf(f, "  [%lld] <%s> = %lg\n", i, names->table[i].name, names->table[i].value);
    }

    return EXIT_SUCCESS;
}

int treeCtor(Tree *tree, Node *root)
{
    CHECK_POISON_PTR(root);
    assert(root);

    tree->root = root;
    tree->size = treeSize(root);

    nameTableCtor(&tree->names);

    return EXIT_SUCCESS;
}

int treeSize(Node *root)
{
    CHECK_POISON_PTR(root);
    if (root == NULL) return 0;

    return treeSize(root->left) + treeSize(root->right);
}

int treeDtor(Tree *tree)
{
    subTreeDtor  ( tree->root);
    nameTableDtor(&tree->names);

    tree->size = -1;

    return EXIT_SUCCESS;
}

int subTreeDtor(Node *root)
{
    CHECK_POISON_PTR(root);
    if (root == NULL) return 0;

    subTreeDtor(root->left);
    subTreeDtor(root->right);
    destroyNode(&root);

    return EXIT_SUCCESS;
}

double expTreeEvaluate(Tree *tree, Node *root, ExpTreeErrors *error)
{
    CHECK_POISON_PTR(root);
    
    if (!root)                           return 0;
    if (root->type == EXP_TREE_NUMBER)   return root->data.number;
    if (root->type == EXP_TREE_VARIABLE) return tree->names.table[root->data.variableNum].value;

    double leftTree  = expTreeEvaluate(tree, root->left,  error);
    LOG("leftTree = %lg\n", leftTree);
    double rightTree = expTreeEvaluate(tree, root->right, error);
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
