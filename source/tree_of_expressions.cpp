#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
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

int printNode(Node *node, FILE *f)
{
    CHECK_POISON_PTR(node);
    assert(node);
    assert(f);

    switch (node->type)
    {
        case EXP_TREE_NOTHING:
            fprintf(f, "nil");
            return EXIT_FAILURE;
        
        case EXP_TREE_NUMBER:
            fprintf(f, ElemNumberFormat, node->data.number);
            return EXIT_SUCCESS;

        case EXP_TREE_OPERATOR:
            return printTreeOperator(node->data.operatorNum, f);

        default:
            LOG("ERROR: unknown NodeType: %d\n", node->type);
            return EXIT_FAILURE;
    }
}

int printTreeOperator(ExpTreeOperators operatorType, FILE *f)
{
    assert(f);

    switch (operatorType)
    {
        case ADD:
            fprintf(f, "add");
            return EXIT_SUCCESS;
        
        case SUB:
            fprintf(f, "sub");
            return EXIT_SUCCESS;

        case MUL:
            fprintf(f, "mul");
            return EXIT_SUCCESS;

        case DIV:
            fprintf(f, "div");
            return EXIT_SUCCESS;

        default:
            LOG("ERROR: unknown ExpTree operator type: %d", operatorType);
            return EXIT_FAILURE;
    }
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

int printTreePrefix(Node *root, FILE *f)
{
    CHECK_POISON_PTR(root);
    assert(f);

    if (!root) 
    {
        fprintf(f, "nil");
        return EXIT_SUCCESS;
    }

    putc('(', f);

    printNode(root, f);  putc(' ', f);

    printTreePrefix(root->left, f);
    putc(' ', f);
    printTreePrefix(root->right, f);

    putc(')', f);
    return EXIT_SUCCESS;
}

int printTreeInfix(Node *root, FILE *f)
{
    CHECK_POISON_PTR(root);
    assert(f);

    if (!root) 
    {
        fprintf(f, "nil");
        return EXIT_SUCCESS;
    }

    putc('(', f);

    printTreeInfix(root->left, f);
    putc(' ', f);
    
    printNode(root, f);
    putc(' ', f);

    printTreeInfix(root->right, f);
    putc(')', f);

    return EXIT_SUCCESS;
}

int printTreePostfix(Node *root, FILE *f)
{
    CHECK_POISON_PTR(root);
    assert(f);

    if (!root) 
    {
        fprintf(f, "nil");
        return EXIT_SUCCESS;
    }

    putc('(', f);

    printTreePostfix(root->left, f);
    putc(' ', f);

    printTreePostfix(root->right, f);
    putc(' ', f);

    printNode(root, f);
    putc(')', f);

    return EXIT_SUCCESS;
}

Node *readTreePrefix(const char *fileName)
{
    assert(fileName);

    FILE *f = fopen(fileName, "r");
    if (!f) return NULL;

    Node *root = readNodePrefix(f);
    fclose(f);

    return root;
}

const int CommandLength = 32;
#define CommandScanFormat "%31[^ ()]"

Node *readNodePrefix(FILE *f)
{
    assert(f);

    int c = getc(f);
    LOG("char = %c(%d)\n", c, c); 
    while (isspace(c)) { c = getc(f); LOG("char = %c(%d)\n", c, c); }
    
    if (c == '(')
    {
        ExpTreeNodeType type = {};
        ExpTreeData     data = {};
        if (readNodeData(&type, &data, f)) return NULL;
        if (type == EXP_TREE_NOTHING)      return NULL;

        Node *node  = createNode(type, data, NULL, NULL);

        LOG("type = %d\ndata = ", type);
        printNode(node, LogFile);
        LOG("\n");

        node->left  = readNodePrefix(f);
        node->right = readNodePrefix(f);

        c = getc(f);
        LOG(" from inside (if c == '(') char = %c(%d)\n", c, c);
        if (c != ')') return (Node *) PtrPoison;

        return node;
    }

    ungetc(c, f);
    c = getc(f);
    LOG("one more: char = %c(%d)\n", c, c); 
    ungetc(c, f);

    ExpTreeNodeType type = {};
    ExpTreeData     data = {};
    if (readNodeData(&type, &data, f)) return (Node *) PtrPoison;
    if (type == EXP_TREE_NOTHING)      return NULL;

    return (Node *)PtrPoison;
}

int readNodeData(ExpTreeNodeType *type, ExpTreeData *data, FILE *f)
{
    assert(type);
    assert(data);
    assert(f);

    *type        = EXP_TREE_NOTHING;
    data->number = DataPoison;

    char command[CommandLength] = "";
    fscanf(f, CommandScanFormat, command);

    double value = 0;
    char *commandEnd = command;
    value = strtod(command, &commandEnd);

    LOG("command[0]    = %c\n", *command);
    LOG("commandEnd[0] = %c\n", *commandEnd);

    if (command != commandEnd) 
    {                                 
        LOG("value = %lg\n", value);

        *type        = EXP_TREE_NUMBER;
        data->number = value;

        return EXIT_SUCCESS;
    }

    LOG("command = %s\n", command);
    if (processStrExpTreeCommand(command, data)) return EXIT_FAILURE;

    if (!equalDouble(data->number, DataPoison)) *type = EXP_TREE_OPERATOR;
    return EXIT_SUCCESS;
}

int processStrExpTreeCommand(char *command, ExpTreeData *data)
{
    assert(command);
    assert(data);

    if      (strcmp(command, "nil") == 0) return EXIT_SUCCESS;
    
    else if (strcmp(command, "add") == 0) data->operatorNum = ADD;
    else if (strcmp(command, "sub") == 0) data->operatorNum = SUB;
    else if (strcmp(command, "mul") == 0) data->operatorNum = MUL;
    else if (strcmp(command, "div") == 0) data->operatorNum = DIV;

    else                                  return EXIT_FAILURE;

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

/*
int readNodeData(ExpTreeNodeType *type, ExpTreeData *data, FILE *f)
{
    assert(type);
    assert(data);
    assert(f);

    *type        = EXP_TREE_NOTHING;
    data->number = DataPoison;

    int c = getc(f);
    LOG("from read data: char = %c(%d)\n", c, c); 
    ungetc(c, f);

    LOG("file pos = %ld\n", ftell(f));
//////////////////// я ничего не понимаю... почему fscanf смещает позицию в файле в этом случае????
    double value = 0;
    if (fscanf(f, "%lf", &value) == 1) //// возможо важно, но первый символ в потокеэто n, затем i и l 
    {                                  ///  на l он останавливается
        LOG("value = %lg\n", value);

        *type        = EXP_TREE_NUMBER;
        data->number = value;

        return EXIT_SUCCESS;
    }

    LOG("file pos = %ld\n", ftell(f));

    char command[CommandLength] = "";

    c = getc(f);
    LOG("from read data: char = %c(%d)\n", c, c); 
    ungetc(c, f);

    fscanf(f, CommandFormat, command);

    LOG("command = %s\n", command);

    if      (strcmp(command, "nil") == 0) return EXIT_SUCCESS;
    
    else if (strcmp(command, "add") == 0) data->operatorNum = ADD;
    else if (strcmp(command, "sub") == 0) data->operatorNum = SUB;
    else if (strcmp(command, "mul") == 0) data->operatorNum = MUL;
    else if (strcmp(command, "div") == 0) data->operatorNum = DIV;

    else                             return EXIT_FAILURE;

    *type = EXP_TREE_OPERATOR;
    return EXIT_SUCCESS;
}*/
