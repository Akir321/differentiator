#include <stdio.h>

#include "tree_of_expressions.h"
#include "recursive_descent_reading.h"

const char *expression = NULL;
int                  p = 0;

#define syntax_assert(exp) if (!(exp)) printf("SYNTAX_ERROR: %s\n", #exp)

#define CHECK_POISON_PTR(ptr) \
    if (ptr == PtrPoison)     \
    {                         \
        LOG("ERROR: PoisonPtr detected in %s(%d) %s\n", __FILE__, __LINE__, __func__);\
        return 0;                                                                     \
    }

#define NEW_NODE(type, value, left, right)                     \
    createNode(type, createNodeData(type, value), left, right)

#define VAR_NODE(value)  NEW_NODE(EXP_TREE_NUMBER, value, NULL, NULL)

#define cL     copy      (eval, node->left)
#define cR     copy      (eval, node->right)
#define cN     copy      (eval, node)

#define dL_W     derivative(eval, node->left,  writeToTex)
#define dR_W     derivative(eval, node->right, writeToTex)

#define dL       derivative(eval, node->left)
#define dR       derivative(eval, node->right)

#define _ADD(left, right) NEW_NODE(EXP_TREE_OPERATOR, ADD, left, right)

#define _SUB(left, right) NEW_NODE(EXP_TREE_OPERATOR, SUB, left, right)

#define _MUL(left, right) NEW_NODE(EXP_TREE_OPERATOR, MUL, left, right)

#define _DIV(left, right) NEW_NODE(EXP_TREE_OPERATOR, DIV, left, right)

#define _LN(       right) NEW_NODE(EXP_TREE_OPERATOR, LN,  NULL, right)

#define _POW(left, right) NEW_NODE(EXP_TREE_OPERATOR, POW, left, right)

#define _SIN(      right) NEW_NODE(EXP_TREE_OPERATOR, SIN, NULL, right)

#define _COS(      right) NEW_NODE(EXP_TREE_OPERATOR, COS, NULL, right)


Node *getG(const char *str)
{
    expression = str;
    p = 0;

    Node *val = getE();

    syntax_assert(expression[p] == '\0');
    return val;
}

Node *getE()
{
    Node *val = getT();

    while (expression[p] == '+' || expression[p] == '-')
    {
        char oper = expression[p];
        p++;

        Node *val2 = getT();
        switch (oper)
        {
            case '+':   val = _ADD(val, val2); break;
            case '-':   val = _SUB(val, val2); break;
            default:    val = PtrPoison;       break;
        }
    }
    return val;
}

Node *getT()
{
    Node *val = getP();

    while (expression[p] == '*' || expression[p] == '/')
    {
        char oper = expression[p];
        p++;

        Node *val2 = getP();
        switch (oper)
        {
            case '*':   val = _MUL(val, val2); break;
            case '/':   val = _DIV(val, val2); break;
            default:    val = PtrPoison;       break;
        }
    }
    return val;
}

Node *getP()
{
    if (expression[p] == '(')
    {
        p++;
        Node *val = getE();

        syntax_assert(expression[p] == ')');
        p++;

        return val;
    }
    else
    {
        return getN();
    }
}

Node *getN()
{
    int val = 0;
    int old_p = p;

    while ('0' <= expression[p] && expression[p] <= '9')
    {
        val = val * 10 + expression[p] - '0';
        p++;
    }

    syntax_assert(old_p < p);
    //printf("old = %d, p = %d, char = %c\n", old_p, p, expression[p]);
    return NEW_NODE(EXP_TREE_NUMBER, val, NULL, NULL);
}

//"1000-7*100/(30+5*10-5*(100/50))+1"
