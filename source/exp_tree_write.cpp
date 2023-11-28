#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "tree_of_expressions.h"
#include "html_logfile.h"
#include "exp_tree_write.h"

#define CHECK_POISON_PTR(ptr) \
    if (ptr == PtrPoison)     \
    {                         \
        LOG("ERROR: PoisonPtr detected in %s(%d) %s\n", __FILE__, __LINE__, __func__);\
        return 0;                                                                        \
    }

int printNode(Evaluator *eval, Node *node, FILE *f)
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

        case EXP_TREE_VARIABLE:
            return printTreeVariable(eval, node, f);

        default:
            LOG("ERROR: unknown NodeType: %d\n", node->type);
            return EXIT_FAILURE;
    }
}

int dumpNode(Evaluator *eval, Node *node, FILE *f)
{
    if (node == PtrPoison) { fprintf(f, "Node * = PtrPoison\n"); return EXIT_FAILURE; }
    if (node == NULL     ) { fprintf(f, "Node * = NULL\n");      return EXIT_FAILURE; }

    fprintf(f, "\nI'm Node dump:\n");

    fprintf(f, "    type  = %d\n", node->type);
    fprintf(f, "    data  = ");
    printNode(eval, node, f);    
    putchar('\n');

    fprintf(f, "    left  = %p\n", node->left);
    fprintf(f, "    right = %p\n", node->right);

    return EXIT_SUCCESS;
}

#define OPER(oper) fprintf(f, oper); return EXIT_SUCCESS

int printTreeOperator(ExpTreeOperators operatorType, FILE *f)
{
    assert(f);

    switch (operatorType)
    {
        case ADD:    OPER("add");
        case SUB:    OPER("sub");
        case MUL:    OPER("mul");
        case DIV:    OPER("div");
        case LN:     OPER("ln");
        case LOGAR:  OPER("log");
        case POW:    OPER("pow");

        default:
            LOG("ERROR: unknown ExpTree operator type: %d", operatorType);
            return EXIT_FAILURE;
    }
}

#undef OPER

int printNodeSymbol(Evaluator *eval, Node *node, FILE *f)
{
    CHECK_POISON_PTR(node);
    assert(node);
    assert(f);

    switch (node->type)
    {
        case EXP_TREE_NOTHING: return EXIT_FAILURE;
        
        case EXP_TREE_NUMBER:
            fprintf(f, ElemNumberFormat, node->data.number);
            return EXIT_SUCCESS;

        case EXP_TREE_OPERATOR:
            return printTreeOperatorSymbol(node->data.operatorNum, f);

        case EXP_TREE_VARIABLE:
            return printTreeVariable(eval, node, f);

        default:
            LOG("ERROR: unknown NodeType: %d\n", node->type);
            return EXIT_FAILURE;
    }
}

int printTreeVariable(Evaluator *eval, Node *node, FILE *f)
{
    CHECK_POISON_PTR(node);
    assert(node);
    assert(f);

    int nameIndex = node->data.variableNum;

    if (!eval)  fprintf(f, "<eval = null>");

    else if (0 <= nameIndex && nameIndex < eval->names.count)
    {
        fprintf(f, "%s", eval->names.table[node->data.variableNum].name);
    }

    else      
    { 
        fprintf(f, "ERROR: bad nameIndex in Node: %d", nameIndex); 
        return EXIT_FAILURE; 
    }

    return EXIT_SUCCESS;
}

#define OPER(oper) fprintf(f, oper); return EXIT_SUCCESS

int printTreeOperatorSymbol(ExpTreeOperators operatorType, FILE *f)
{
    assert(f);

    switch (operatorType)
    {
        case ADD:    OPER("+");
        case SUB:    OPER("-");
        case MUL:    OPER("*");
        case DIV:    OPER("/");
        case LN:     OPER("ln");
        case LOGAR:  OPER("log");
        case POW:    OPER("^");

        default:
            LOG("ERROR: unknown ExpTree operator type: %d", operatorType);
            return EXIT_FAILURE;
    }
}

#undef OPER

int printTreePrefix(Evaluator *eval, Node *root, FILE *f)
{
    CHECK_POISON_PTR(root);
    assert(eval);
    assert(f);

    if (!root) 
    {
        fprintf(f, "nil");
        return EXIT_SUCCESS;
    }

    putc('(', f);

    printNode(eval, root, f);                  putc(' ', f);
    printTreePrefix(eval, root->left, f);      putc(' ', f);
    printTreePrefix(eval, root->right, f);   

    putc(')', f);

    return EXIT_SUCCESS;
}

int printTreeInfix(Evaluator *eval, Node *root, FILE *f)
{
    CHECK_POISON_PTR(root);
    assert(eval);
    assert(f);

    if (!root) 
    {
        fprintf(f, "nil");
        return EXIT_SUCCESS;
    }

    putc('(', f);

    printTreeInfix(eval, root->left, f);       putc(' ', f);
    printNode(eval, root, f);                  putc(' ', f);
    printTreeInfix(eval, root->right, f);

    putc(')', f);

    return EXIT_SUCCESS;
}

int printTreePostfix(Evaluator *eval, Node *root, FILE *f)
{
    CHECK_POISON_PTR(root);
    assert(eval);
    assert(f);

    if (!root) 
    {
        fprintf(f, "nil");
        return EXIT_SUCCESS;
    }

    putc('(', f);

    printTreePostfix(eval, root->left, f);     putc(' ', f);
    printTreePostfix(eval, root->right, f);    putc(' ', f);
    printNode(eval, root, f);
    
    putc(')', f);

    return EXIT_SUCCESS;
}

int expTreeNodePriority(Node *node)
{
    CHECK_POISON_PTR(node);
    if (!node) return PR_NULL;

    if (node->type == EXP_TREE_NUMBER) return PR_NUMBER;
    if (node->type == EXP_TREE_VARIABLE) return PR_NUMBER;

    switch (node->data.operatorNum)
    {
    case ADD: case SUB:
        return PR_ADD_SUB;
    
    case MUL: case DIV:
        return PR_MUL_DIV;

    case LN: case LOGAR:
        return PR_UNARY;

    case POW:
        return PR_POW;
    
    default:
        return PR_UNKNOWN;
    }
}

bool isCommutative(Node *node)
{
    CHECK_POISON_PTR(node);
    if (!node) return true;

    if (node->data.operatorNum == ADD ||
        node->data.operatorNum == MUL) return true;

    return false;
}

int printTreeInfixNoUselessBrackets(Evaluator *eval, Node *root, FILE *f)
{
    CHECK_POISON_PTR(root);
    assert(eval);
    assert(f);

    if (!root) return EXIT_SUCCESS;

    if (root->type == EXP_TREE_NUMBER ||
        root->type == EXP_TREE_VARIABLE)
    {
        printNodeSymbol(eval, root, f);
        return EXIT_SUCCESS;
    }

    printNodeUsefulBrackets(eval, root->left, root, f);       putc(' ', f);
    printNodeSymbol(eval, root, f);                           putc(' ', f);
    printNodeUsefulBrackets(eval, root->right, root, f);    //putc(' ', f);

    return EXIT_SUCCESS;
}

int printNodeUsefulBrackets(Evaluator *eval, Node *node, Node *parent, FILE *f)
{
    assert(eval);
    CHECK_POISON_PTR(node);
    CHECK_POISON_PTR(parent);

    int parentPriority = expTreeNodePriority(parent);
    int nodePriority   = expTreeNodePriority(node);

    if ((parentPriority > nodePriority) || 
        (parentPriority == nodePriority && !isCommutative(parent)))
    {
        putc('(', f);
        printTreeInfixNoUselessBrackets(eval, node, f);
        putc(')', f);
    }
    else    printTreeInfixNoUselessBrackets(eval, node, f);

    return EXIT_SUCCESS;
}

// a broken function which doesn't work properly
// i'd like to understand why it wasn't working
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
