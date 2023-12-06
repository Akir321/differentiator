#ifndef  __RECURSIVE_DESCENT_READING_H__
#define  __RECURSIVE_DESCENT_READING_H__

#include "tree_of_expressions.h"

struct ReadBuf
{
    char *str;
    int size;
    int position;

    int line;
    int linePosition;
};

struct Token
{
    int line;
    int startPosition;

    ExpTreeNodeType type;
    ExpTreeData     data;
};

int setToken(Token *token, int line, int startPosition, ExpTreeNodeType type, ExpTreeData data);

Token *createTokenArray(const char *source);

int getToken(Token *token, ReadBuf *readBuf);

int caseNumber(Token *token, ReadBuf *readBuf);
int caseLetter(Token *token, ReadBuf *readBuf);
int skipSpaces(ReadBuf *readBuf);

ExpTreeOperators getWordOperator(const char *word);

int printTokenArray(Token *tokenArray, FILE *f);

Node *getG(const char *str);
Node *getE();
Node *getT();
Node *getP();
Node *getN();


#endif //__RECURSIVE_DESCENT_READING_H__
