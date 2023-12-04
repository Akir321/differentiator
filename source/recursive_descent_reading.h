#ifndef  __RECURSIVE_DESCENT_READING_H__
#define  __RECURSIVE_DESCENT_READING_H__

struct readBuf
{
    const char *str;
    int p;
};

Node *getG(const char *str);
Node *getE();
Node *getT();
Node *getP();
Node *getN();


#endif //__RECURSIVE_DESCENT_READING_H__
