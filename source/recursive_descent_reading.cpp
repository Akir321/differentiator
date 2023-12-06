#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "tree_of_expressions.h"
#include "exp_tree_write.h"
#include "recursive_descent_reading.h"
#include "html_logfile.h"

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



Token *createTokenArray(const char *source)
{
    assert(source);

    ReadBuf readBuf = { strdup(source), (int) strlen(source), 0, 0, 0 };
    if (!readBuf.str) return NULL;

    int arrPosition = 0;
    Token *buf = (Token *)calloc(strlen(source) + 1, sizeof(Token));
    if (!buf) return NULL;  

    while (readBuf.position < readBuf.size)
    {
        LOG("\nreadBuf pos = %d\n", readBuf.position);
        LOG( "Tokenarr pos = %d\n", arrPosition);

        int error = getToken(buf + arrPosition, &readBuf);
        printTokenArray(buf, LogFile);
        if (error) { LOG("getToken: ERROR occured: %d\n", error); return NULL; }

        arrPosition++;
    }

    free(readBuf.str);

    return buf;
}



#define SET_TOKEN(type, value)\
    setToken(token, readBuf->line, readBuf->linePosition, type, createNodeData(type, value))

#define TOKEN_OP(opType)                 \
    SET_TOKEN(EXP_TREE_OPERATOR, opType);\
    readBuf->linePosition++;             \
    readBuf->position++;    


int getToken(Token *token, ReadBuf *readBuf)
{
    assert(token);
    assert(readBuf);
    assert(readBuf->str);

    switch (readBuf->str[readBuf->position])
    {
        case '1': case '2': case '3': case '4': case '5': case '6':
        case '7': case '8': case '9': case '0':
        {
            caseNumber(token, readBuf); return EXIT_SUCCESS;
        }

        case '+':   TOKEN_OP(ADD);  return EXIT_SUCCESS;
        case '-':   TOKEN_OP(SUB);  return EXIT_SUCCESS;
        case '*':   TOKEN_OP(MUL);  return EXIT_SUCCESS;
        case '/':   TOKEN_OP(DIV);  return EXIT_SUCCESS;
        case '^':   TOKEN_OP(POW);  return EXIT_SUCCESS;

        case '(':   TOKEN_OP(L_BRACKET); return EXIT_SUCCESS;
        case ')':   TOKEN_OP(R_BRACKET); return EXIT_SUCCESS;

        case ' ':  case '\n':
        case '\t': case '\r':   skipSpaces(readBuf);
                                getToken(token, readBuf);
                                return EXIT_SUCCESS;

        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': 
        case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': 
        case 'v': case 'w': case 'x': case 'y': case 'z':
        {
            caseLetter(token, readBuf); return EXIT_SUCCESS;
        }

        default:    LOG("ERROR: unknown operator: %d\n\tSYNTAX_ERROR since line %d, position %d\n", 
                        readBuf->str[readBuf->position], readBuf->line, readBuf->linePosition);
                    return EXIT_FAILURE;
    }
}

int setToken(Token *token, int line, int startPosition, ExpTreeNodeType type, ExpTreeData data)
{
    assert(token);

    (*token).type          = type;
    (*token).data          = data;
    (*token).line          = line;
    (*token).startPosition = startPosition;

    return EXIT_SUCCESS;
}

int skipSpaces(ReadBuf *readBuf)
{
    assert(readBuf);
    assert(readBuf->str);

    while (isspace(readBuf->str[readBuf->position]))
    {
        char c = readBuf->str[readBuf->position];

        switch (c)
        {
            case ' ': case '\t':
            case '\r':
                                readBuf->position++;
                                readBuf->linePosition++;
                                break;           
            
            case '\n':          readBuf->position++;
                                readBuf->line++;
                                readBuf->linePosition = 0;
                                break;
        
            default:            LOG("ERROR: unknown space operator: %c(%d)\n",c, c);
                                break;
        }
    }

    return EXIT_SUCCESS;
}

int caseNumber(Token *token, ReadBuf *readBuf)
{
    assert(token);
    assert(readBuf);
    assert(readBuf->str);

    LOG("position  = %d\n", readBuf->position);
    LOG("str (before strtod) = %p\n", readBuf->str);
    char *end = readBuf->str + readBuf->position;
    double value = strtod(readBuf->str + readBuf->position, &end);

    LOG("str (after strtod) = %p\n", readBuf->str);
    LOG("caseNumber value = %lg\n", value);
    LOG("position  = %d\n", readBuf->position);
    LOG("buf + pos = %p, end = %p\n", readBuf->str + readBuf->position, end);

    SET_TOKEN(EXP_TREE_NUMBER, value);

    int shift = (int)(end - readBuf->str - readBuf->position) / sizeof(char);
    LOG("shift = %d\n", shift);

    readBuf->linePosition += shift;
    readBuf->position     += shift;
    LOG("new position  = %d\n", readBuf->position);

    return EXIT_SUCCESS;
}

int caseLetter(Token *token, ReadBuf *readBuf)
{
    assert(token);
    assert(readBuf);
    assert(readBuf->str);

    int position = 0;
    char *word   = (char *)calloc(readBuf->size + 1 - readBuf->position, sizeof(char));
    if (!word) return MEMORY_ERROR;

    int linePosition = readBuf->linePosition;
    
    while (isalpha(readBuf->str[readBuf->position]))
    {
        word[position++] = readBuf->str[readBuf->position];
        readBuf->position++;
        readBuf->linePosition++;
    }

    ExpTreeOperators op = getWordOperator(word);
    if (!op) 
    { 
        LOG("ERROR: unknown word: %s on line %d position %d", word, readBuf->line, linePosition)
        return UNKNOWN_OPERATOR;
    }
    
    SET_TOKEN(EXP_TREE_OPERATOR, op);

    return EXIT_SUCCESS;
}

ExpTreeOperators getWordOperator(const char *word)
{
    assert(word);

    if (*word == '\0') return NOT_OPER;

    else if (strcmp(word, "sin") == 0) return SIN;
    else if (strcmp(word, "cos") == 0) return COS;
    else if (strcmp(word, "log") == 0) return LOGAR;
    else if (strcmp(word, "ln")  == 0) return LN;

    return NOT_OPER;
}

int printTokenArray(Token *tokenArray, FILE *f)
{
    assert(tokenArray);
    assert(f);

    fprintf(f, "I'm Token Array\n{\n");

    for (int i = 0; tokenArray[i].type != EXP_TREE_NOTHING; i++)
    {
        switch (tokenArray[i].type)
        {
            case EXP_TREE_NUMBER:
                fprintf(f, " %lg\n", tokenArray[i].data.number);
                break;

            case EXP_TREE_OPERATOR:
                putc(' ', f);
                printTreeOperator(tokenArray[i].data.operatorNum, f);
                putc('\n', f);
                break;

            case EXP_TREE_VARIABLE: case EXP_TREE_NOTHING:
                break;

            default: printf("ERROR: incorrect operator type: %d\n", tokenArray[i].data.operatorNum);
        }
    }

    fprintf(f, "}\n");

    return EXIT_SUCCESS;
}





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
    Node *val = getN();

    while (expression[p] == '*' || expression[p] == '/')
    {
        char oper = expression[p];
        p++;

        Node *val2 = getN();
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
//"100-5*3-14/7"
