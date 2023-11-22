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
        node->left  = readNodePrefix(f);
        node->right = readNodePrefix(f);

        c = getc(f);
        if (c != ')') return (Node *) PtrPoison;

        return node;
    }
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

    if (command != commandEnd) 
    {                                 
        *type        = EXP_TREE_NUMBER;
        data->number = value;

        return EXIT_SUCCESS;
    }

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
