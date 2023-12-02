#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "tree_of_expressions.h"
#include "html_logfile.h"
#include "exp_tree_read.h"
#include "tree_graphic_dump.h"

#define CHECK_POISON_PTR(ptr) \
    if (ptr == PtrPoison)     \
    {                         \
        LOG("ERROR: PoisonPtr detected in %s(%d) %s\n", __FILE__, __LINE__, __func__);   \
        return 0;                                                                        \
    }

int readTree(Evaluator *eval, const char *fileName, Node * (*readNode)(Evaluator *, FILE *f))
{
    assert(eval);
    assert(fileName);
    assert(readNode);

    nameTableCtor(&eval->names);

    FILE *f = fopen(fileName, "r");
    if (!f) return EXIT_FAILURE;

    Node *root = readNode(eval, f);
    fclose(f);

    if (!root) return EXIT_FAILURE;

    eval->tree.root = root;
    eval->tree.size = treeSize(eval->tree.root);

    return EXIT_SUCCESS;
}

int readTreePrefix(Evaluator *eval,  const char *fileName)
{
    return readTree(eval, fileName, readNodePrefix);
}

int readTreeInfix(Evaluator *eval, const char *fileName)
{
    return readTree(eval, fileName, readNodeInfix);
}

int skipSpaces(FILE *f)
{
    assert(f);

    int c = getc(f);
    LOG("char = %c(%d)\n", c, c);

    while (isspace(c) && c != EOF) 
    { 
        c = getc(f); 
        LOG("char = %c(%d)\n", c, c); 
    }

    if (c == EOF) return EOF;

    ungetc(c, f);
    return EXIT_SUCCESS;
}

const int CommandLength = 32;
#define CommandScanFormat "%31[^ (_)]"

#define READ_NODE_DATA(processCommand)                                                   \
    ExpTreeNodeType type = {};                                                           \
    ExpTreeData     data = {};                                                           \
    if (readNodeData(eval, &type, &data, f, processCommand)) return (Node *) PtrPoison;  \
    if (type == EXP_TREE_NOTHING)                            return NULL;               

Node *readNodePrefix(Evaluator *eval, FILE *f)
{
    assert(eval);
    assert(f);

    skipSpaces(f);
    int c = getc(f);
    
    if (c == '(')
    {
        READ_NODE_DATA(processStrExpTreeCommand);

        Node *node  = createNode(type, data, NULL, NULL);
        node->left  = readNodePrefix(eval, f);
        node->right = readNodePrefix(eval, f);

        c = getc(f);
        if (c != ')') return (Node *) PtrPoison;

        return node;
    }
    ungetc(c, f);

    READ_NODE_DATA(processStrExpTreeCommand);

    return (Node *)PtrPoison;
}

int readNodeData(Evaluator *eval, ExpTreeNodeType *type, ExpTreeData *data, FILE *f, 
                 int (*processCommand)(Evaluator *, char *, ExpTreeData *, ExpTreeNodeType *))
{
    assert(eval);
    assert(type);
    assert(data);
    assert(f);

    *type        = EXP_TREE_NOTHING;
    data->number = DataPoison;

    skipSpaces(f);

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

    if (processCommand(eval, command, data, type)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

#define OPER(str, number) else if (strcmp(command, str) == 0) data->operatorNum = number

int processStrExpTreeCommand(Evaluator *eval, char *command, ExpTreeData *data, ExpTreeNodeType *type)
{
    assert(eval);
    assert(command);
    assert(data);

    if      (strcmp(command, "nil") == 0) return EXIT_SUCCESS;
    
    OPER("add", ADD);
    OPER("sub", SUB);
    OPER("mul", MUL);
    OPER("div", DIV);
    OPER("ln",  LN);
    OPER("log", LOGAR);
    OPER("pow", POW);
    OPER("sin", SIN);
    OPER("cos", COS);

    else 
    {
        return  addVariableToNameTable(eval, command, data, type);
    }      

    *type = EXP_TREE_OPERATOR;
    return EXIT_SUCCESS;
}

int processStrExpTreeCommandSymbol(Evaluator *eval, char *command, ExpTreeData *data, ExpTreeNodeType *type)
{
    assert(eval);
    assert(command);
    assert(data);
    assert(type);

    if      (strcmp(command, "_") == 0) return EXIT_SUCCESS;    // nil
    
    OPER("+", ADD);
    OPER("-", SUB);
    OPER("*", MUL);
    OPER("-", DIV);
    OPER("ln",  LN);
    OPER("log", LOGAR);
    OPER("^", POW);
    OPER("sin", SIN);
    OPER("cos", COS);

    else 
    {   
        return  addVariableToNameTable(eval, command, data, type);
    }      

    *type = EXP_TREE_OPERATOR;
    return EXIT_SUCCESS;
}

int addVariableToNameTable(Evaluator *eval, char *command, ExpTreeData *data, ExpTreeNodeType *type)
{
    assert(eval);
    assert(command);
    assert(data);
    assert(type);

    if (strlen(command) > 1)
    {
        LOG("ERROR: incorrect variable name: %s\n", command);
        LOG("       only one-letter names allowed\n");
        return EXIT_FAILURE;
    }

    int nameIndex = nameTableFind(&eval->names, command);

    *type = EXP_TREE_VARIABLE;
    if (nameIndex == IndexPoison) 
    {
         data->variableNum = nameTableAdd(&eval->names, command, DefaultVarValue);
    }
    else data->variableNum = nameIndex;
    
    if (data->variableNum == IndexPoison) return EXIT_FAILURE;

    return EXIT_SUCCESS; 
}

Node *readNodeInfix(Evaluator *eval, FILE *f)
{
    assert(eval);
    assert(f);

    skipSpaces(f);
    int c = getc(f);

    if (c == '(')
    {
        Node *left = readNodeInfix(eval, f);

        READ_NODE_DATA(processStrExpTreeCommandSymbol);

        Node *right = readNodeInfix(eval, f);

        Node *node  = createNode(type, data, left, right);
        treeGraphicDump(eval, node);

        c = getc(f);    LOG("char = %c(%d)\n", c, c); 
        if (c != ')') return (Node *) PtrPoison;

        return node;
    }

    if (c == '_') return NULL;
    ungetc(c, f);

    READ_NODE_DATA(processStrExpTreeCommandSymbol);

    return (Node *)PtrPoison;
}

#undef CommandScanFormat
#undef READ_NODE_DATA
