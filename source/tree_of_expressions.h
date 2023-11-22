#ifndef  __TREES_H__
#define  __TREES_H__

enum ExpTreeNodeType
{
    EXP_TREE_NOTHING  = 0,
    EXP_TREE_NUMBER   = 1,
    EXP_TREE_OPERATOR = 2,
};

#define ElemNumberFormat "%lg"

enum ExpTreeOperators
{
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4
};

union ExpTreeData
{
    double           number;
    ExpTreeOperators operatorNum;
};

struct Node
{
    ExpTreeNodeType type;
    ExpTreeData     data;
    
    Node *left;
    Node *right;
};

struct Tree
{
    Node *root;
    int size;
};

enum ExpTreeOperatorPriorities
{
    PR_UNKNOWN = -1,
    PR_ADD_SUB = 2,
    PR_MUL_DIV = 4,
    PR_NUMBER  = 100,
    PR_NULL    = 101,
};

enum ExpTreeErrors
{
    NO_ERROR         = 0,
    DIVISION_BY_ZERO = 1,
    UNKNOWN_OPERATOR = 2,
};

Node * const PtrPoison = (Node *)42;

const int DataPoison = -11111111;

const int WordLength = 256;


Node *createNode(ExpTreeNodeType type, ExpTreeData data, Node *left, Node *right);
int destroyNode (Node **nodePtr);

int treeCtor(Tree *tree, Node *root);

int treeSize(Node *root);
int treeDtor(Node *root);

double expTreeEvaluate(Node *root, ExpTreeErrors *error);
double NodeCalculate(double leftTree, double rightTree, 
                     ExpTreeOperators operatorType, ExpTreeErrors *error);

const double PrecisionConst = 0.000001;
bool equalDouble(double a, double b);

#endif //__TREES_H__
