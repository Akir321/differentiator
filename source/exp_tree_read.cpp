#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "tree_of_expressions.h"
#include "html_logfile.h"
#include "exp_tree_read.h"

#define CHECK_POISON_PTR(ptr) \
    if (ptr == PtrPoison)     \
    {                         \
        LOG("ERROR: PoisonPtr detected in %s(%d) %s\n", __FILE__, __LINE__, __func__);   \
        return 0;                                                                        \
    }

int readTree(Tree *tree, const char *fileName, Node * (*readNode)(Tree *, FILE *f))
{
    assert(tree);
    assert(fileName);
    assert(readNode);

    nameTableCtor(&tree->names);

    FILE *f = fopen(fileName, "r");
    if (!f) return EXIT_FAILURE;

    Node *root = readNode(tree, f);
    fclose(f);

    if (!root) return EXIT_FAILURE;

    tree->root = root;
    tree->size = treeSize(tree->root);

    return EXIT_SUCCESS;
}

int readTreePrefix(Tree *tree,  const char *fileName)
{
    return readTree(tree, fileName, readNodePrefix);
}

int readTreeInfix(Tree *tree, const char *fileName)
{
    return readTree(tree, fileName, readNodeInfix);
}

const int CommandLength = 32;
#define CommandScanFormat "%31[^ (_)]"

Node *readNodePrefix(Tree *tree, FILE *f)
{
    assert(tree);
    assert(f);

    int c = getc(f);
    LOG("char = %c(%d)\n", c, c); 
    while (isspace(c)) { c = getc(f); LOG("char = %c(%d)\n", c, c); }
    
    if (c == '(')
    {
        ExpTreeNodeType type = {};
        ExpTreeData     data = {};
        if (readNodeData(tree, &type, &data, f, processStrExpTreeCommand)) return NULL;
        if (type == EXP_TREE_NOTHING)                                return NULL;

        Node *node  = createNode(type, data, NULL, NULL);
        node->left  = readNodePrefix(tree, f);
        node->right = readNodePrefix(tree, f);

        c = getc(f);
        if (c != ')') return (Node *) PtrPoison;

        return node;
    }
    ungetc(c, f);

    ExpTreeNodeType type = {};
    ExpTreeData     data = {};
    if (readNodeData(tree, &type, &data, f, processStrExpTreeCommand)) return (Node *) PtrPoison;
    if (type == EXP_TREE_NOTHING)                                return NULL;

    return (Node *)PtrPoison;
}

int readNodeData(Tree *tree, ExpTreeNodeType *type, ExpTreeData *data, FILE *f, 
                 int (*processCommand)(Tree *, char *, ExpTreeData *, ExpTreeNodeType *))
{
    assert(tree);
    assert(type);
    assert(data);
    assert(f);

    *type        = EXP_TREE_NOTHING;
    data->number = DataPoison;

    int c = getc(f);
    while (isspace(c)) { c = getc(f); }
    ungetc(c, f);

    char command[CommandLength] = "";
    fscanf(f, CommandScanFormat, command);

    double value = 0;
    char *commandEnd = command;
    value = strtod(command, &commandEnd);

    if (command != commandEnd) 
    {                                 
        *type        = EXP_TREE_NUMBER;
        data->number = value;

        return EXIT_SUCCESS;
    }

    if (processCommand(tree, command, data, type)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int processStrExpTreeCommand(Tree *tree, char *command, ExpTreeData *data, ExpTreeNodeType *type)
{
    assert(tree);
    assert(command);
    assert(data);

    if      (strcmp(command, "nil") == 0) return EXIT_SUCCESS;
    
    else if (strcmp(command, "add") == 0) data->operatorNum = ADD;
    else if (strcmp(command, "sub") == 0) data->operatorNum = SUB;
    else if (strcmp(command, "mul") == 0) data->operatorNum = MUL;
    else if (strcmp(command, "div") == 0) data->operatorNum = DIV;

    else 
    {
        return  addVariableToNameTable(tree, command, data, type);
    }      

    *type = EXP_TREE_OPERATOR;
    return EXIT_SUCCESS;
}

int processStrExpTreeCommandSymbol(Tree *tree, char *command, ExpTreeData *data, ExpTreeNodeType *type)
{
    assert(tree);
    assert(command);
    assert(data);
    assert(type);

    if      (strcmp(command, "_") == 0) return EXIT_SUCCESS;    // nil
    
    else if (strcmp(command, "+") == 0) data->operatorNum = ADD;
    else if (strcmp(command, "-") == 0) data->operatorNum = SUB;
    else if (strcmp(command, "*") == 0) data->operatorNum = MUL;
    else if (strcmp(command, "/") == 0) data->operatorNum = DIV;

    else 
    {   
        return  addVariableToNameTable(tree, command, data, type);
    }      

    *type = EXP_TREE_OPERATOR;
    return EXIT_SUCCESS;
}

int addVariableToNameTable(Tree *tree, char *command, ExpTreeData *data, ExpTreeNodeType *type)
{
    assert(tree);
    assert(command);
    assert(data);
    assert(type);

    if (strlen(command) > 1)
    {
        LOG("ERROR: incorrect variable name: %s\n", command);
        LOG("       only one-letter names allowed\n");
        return EXIT_FAILURE;
    }

    *type = EXP_TREE_VARIABLE;
    data->variableNum = nameTableAdd(&tree->names, command, DefaultVarValue);
    if (data->variableNum == IndexPoison) return EXIT_FAILURE;

    return EXIT_SUCCESS; 
}

Node *readNodeInfix(Tree *tree, FILE *f)
{
    assert(tree);
    assert(f);

    int c = getc(f);
    LOG("char = %c(%d)\n", c, c); 
    while (isspace(c)) { c = getc(f); LOG("char = %c(%d)\n", c, c); }

    if (c == '(')
    {
        Node *left = readNodeInfix(tree, f);

        ExpTreeNodeType type = {};
        ExpTreeData     data = {};
        if (readNodeData(tree, &type, &data, f, processStrExpTreeCommandSymbol)) return NULL;
        if (type == EXP_TREE_NOTHING)                                      return NULL;

        Node *right = readNodeInfix(tree, f);

        Node *node  = createNode(type, data, left, right);

        c = getc(f);
        LOG("char = %c(%d)\n", c, c); 
        if (c != ')') return (Node *) PtrPoison;

        return node;
    }

    if (c == '_') return NULL;
    ungetc(c, f);

    ExpTreeNodeType type = {};
    ExpTreeData     data = {};
    if (readNodeData(tree, &type, &data, f, processStrExpTreeCommandSymbol)) return (Node *) PtrPoison;
    if (type == EXP_TREE_NOTHING)                                      return NULL;

    return (Node *)PtrPoison;
}

#undef CommandScanFormat
