#ifndef ANALYZER_H
#define ANALYZER_H

#include <iostream>
#include <string>
#include <string.h>

#include "diffconsts.h"
#include "node.h"

using namespace std;

class Analyzer
{
private:
    char*  s;
    int    slen;
    string varstr;

    //Node* OpUnion(Node* nod1, Node* nod2, char op);
    //Node* OpUnion(Node* nod1, char op);
    //void  MyAssert(bool b);
public:
    Node* tree;
    Analyzer(char* input);
    ~Analyzer();

    Node* GetG();
    Node* GetN();
    Node* GetE();
    Node* GetT();
    Node* GetB();
    Node* GetV();
    Node* GetVN();
    Node* GetSin();
    Node* GetCos();
    Node* GetSC();
    Node* GetPow();
    Node* GetL();

    Node* GetSB(const char* func_name);

};

#endif // ANALYZER_H
