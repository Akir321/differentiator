#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "tree_of_expressions.h"
#include "exp_tree_write.h"
#include "tree_graphic_dump.h"
#include "html_logfile.h"
#include "differentiator.h"

#ifdef  __PRINT_TO_TEX__
extern FILE *TexFile;
#endif

extern const char *getPrefix(Node *node);

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

#define EV_L canBeEvaluated(node->left)
#define EV_R canBeEvaluated(node->right)

#define VALUE_0(node) (canBeEvaluated(node) && equalDouble(expTreeEvaluate(eval, node, &error), 0))
#define VALUE_1(node) (canBeEvaluated(node) && equalDouble(expTreeEvaluate(eval, node, &error), 1))

int differentiate(Evaluator *from, Evaluator *to, bool writeToTex)
{
    assert(to);
    assert(from);

    evaluatorCtor(to);

    treeCtor(&to->tree, derivative(from, from->tree.root, writeToTex));
    nameTableCopy(&from->names, &to->names);

    return EXIT_SUCCESS;
}

Node *copy(Evaluator *eval, Node *node)
{
    assert(eval);
    CHECK_POISON_PTR(node);
    if (!node) return NULL;

    return createNode(node->type, node->data, cL, cR);
}

#define VAR_NODE_NAME(node) eval->names.table[node->data.variableNum].name
#define IS_X(node)          strcmp("x", VAR_NODE_NAME(node)) == 0

Node *derivative(Evaluator *eval, Node *node, bool writeToTex)
{
    assert(eval);
    assert(node);
    CHECK_POISON_PTR(node);

    switch (node->type)
    {
        case EXP_TREE_NUMBER:       return VAR_NODE(0);
    
        case EXP_TREE_VARIABLE:     if (IS_X(node)) return VAR_NODE(1);
                                    else            return VAR_NODE(0);
    
        case EXP_TREE_OPERATOR:     return processDifOperator(eval, node, writeToTex);

        case EXP_TREE_NOTHING:      printf("ERROR: node type: nothing\n");
                                    return PtrPoison;
    
        default:                    printf("ERROR: unknown node type: %d\n", node->type);
                                    return PtrPoison;
        
    }
}

#undef VAR_NODE_NAME
#undef IS_X

#ifdef __PRINT_TO_TEX__

#define DERIVATIVE(expression)                                          \
    {                                                                   \
        Node *deriv = expression;                                       \
                                                                        \
        if (writeToTex)                                                 \
        {                                                               \
            printTreeToTexFile(eval, node,  TexFile, getPrefix(node));  \
            fprintf(TexFile, " $\\Rightarrow$ ");                       \
            printTreeToTexFile(eval, deriv, TexFile, NULL);             \
            fprintf(TexFile, "\n\n");                                   \
        }                                                               \
                                                                        \
        return deriv;                                                   \
    }

#else

#define DERIVATIVE(expression) return expression;

#endif

Node *processDifOperator(Evaluator *eval, Node *node, bool writeToTex)
{
    assert(eval);
    assert(node);
    CHECK_POISON_PTR(node);

    switch (node->data.operatorNum)
    {
        case ADD:   DERIVATIVE(_ADD(dL_W, dR_W))

        case SUB:   DERIVATIVE(_SUB(dL_W, dR_W))
        
        case MUL:   DERIVATIVE(_ADD(_MUL(dL_W, cR), _MUL(cL, dR_W)))

        case DIV:   DERIVATIVE(_DIV(_SUB(_MUL(dL_W, cR), _MUL(cL, dR_W)), _MUL(cR, cR)))

        case LN:    DERIVATIVE(_DIV(dR_W, cR))

        case LOGAR: DERIVATIVE(difProcessLog(eval, node))

        case POW:   DERIVATIVE(difRrocessPow(eval, node))

        case SIN:   DERIVATIVE(_MUL(dR_W, _COS(cR)))

        case COS:   DERIVATIVE(_MUL(dR_W, _MUL(VAR_NODE(-1), _SIN(cR))))


        case L_BRACKET: case R_BRACKET: case NOT_OPER:
        default:    printf("ERROR: unknown operator: %d\n", node->data.operatorNum);
                    return PtrPoison;
    }
}

Node *difProcessLog(Evaluator *eval, Node *node)
{
    assert(eval);
    assert(node);
    CHECK_POISON_PTR(node);

    Node *divLns       = _DIV(_LN(cR), _LN(cL));
    Node *logDerivative = derivative(eval, divLns);

    subTreeDtor(divLns);
    return logDerivative;
}

Node *difRrocessPow(Evaluator *eval, Node *node)
{
    assert(eval);
    assert(node);
    CHECK_POISON_PTR(node);

    bool canBeEvalL = EV_L,
         canBeEvalR = EV_R;

    if      (canBeEvalL && canBeEvalR)    return VAR_NODE(0);

    else if (canBeEvalL)                  return _MUL(cN, _LN(cL));
    
    else if (canBeEvalR)                  return _MUL(cR, _MUL(dL, _POW(cL, _SUB(cR, VAR_NODE(1)))));
    
    else
    {
        Node *powerOfE        = _MUL(cR, _LN(cL));
        Node *powerDerivative = derivative(eval, powerOfE);

        subTreeDtor(powerOfE);
        return _MUL(cN, powerDerivative);
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
        ExpTreeErrors error = TREE_NO_ERROR;
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

        return 1;
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

int tryNodeSimplify(Evaluator *eval, Node *node)
{
    assert(eval);
    CHECK_POISON_PTR(node);

    switch (node->data.operatorNum)
    {
        case ADD: case SUB:
        {
            if (casePlus0(eval, node, node->left,  node->right))  return CHANGED;
            if (casePlus0(eval, node, node->right, node->left))   return CHANGED;

            return EXIT_SUCCESS;
        }
        case MUL:
        {
            if (caseTimes0(eval, node, node->left))               return CHANGED;
            if (caseTimes0(eval, node, node->right))              return CHANGED;
            if (caseTimes1(eval, node, node->left,  node->right)) return CHANGED;
            if (caseTimes1(eval, node, node->right, node->left))  return CHANGED;

            return EXIT_SUCCESS;
        }
        case DIV:
        {
            if (caseTimes0(eval, node, node->left)) return CHANGED;
            return EXIT_SUCCESS;
        }
        case POW:
            return EXIT_SUCCESS;

        case LN:  case LOGAR:
        case SIN: case COS:
            return EXIT_SUCCESS;

        case L_BRACKET: case R_BRACKET: case NOT_OPER:
        default:
            printf("ERROR: unknown operator: %d\n", node->data.operatorNum);
            return UNKNOWN_OPERATOR;
    }
}

int casePlus0(Evaluator *eval, Node *node, Node *zero, Node *savedNode) 
{
    CHECK_POISON_PTR(zero);
    CHECK_POISON_PTR(savedNode);

    ExpTreeErrors error = TREE_NO_ERROR;

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
    ExpTreeErrors error = TREE_NO_ERROR;

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

    ExpTreeErrors error = TREE_NO_ERROR;

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

int casePow0(Evaluator *eval, Node *node)
{
    ExpTreeErrors error = TREE_NO_ERROR;

    if (VALUE_0(node))
    {
        if (error) return error;
    }

    return EXIT_SUCCESS;
}

int casePow1(Evaluator *eval, Node *node)
{
    ExpTreeErrors error = TREE_NO_ERROR;
    
    if (VALUE_1(node))
    {
        if (error) return error;
    }

    return EXIT_SUCCESS;
}

#undef CHANGED

int processArgv(int argC, const char *argV[], const char **fileInName, char **fileOutName)
{
    assert(argV);
    assert(fileInName);
    assert(fileOutName);

    if (argC == 1) return EXIT_FAILURE;

    *fileInName  = argV[1];

    char *temp = strdup(*fileInName);
    pointToZero(temp);

    __mingw_asprintf(fileOutName, "%s.tex", temp);
    free(temp);

    return EXIT_SUCCESS;
}

void pointToZero(char *str)
{
    assert(str);

    size_t i = 0;
    while(str[i] != '.' && str[i] != '\0') { i++; }

    str[i] = '\0';
}
