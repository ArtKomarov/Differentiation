#ifndef ANALYZER_H
#define ANALYZER_H

#include <iostream>
#include <string>
#include <string.h>

#include "diffconsts.h"
#include "node.h"

using namespace std;

class Analyzer {
private:
    char*  s;
    int    slen;
    string varstr;

    //Node* OpUnion(Node* nod1, Node* nod2, char op);
    //Node* OpUnion(Node* nod1, char op);
    //void  MyAssert(bool b);
public:
    Node* tree;
    Analyzer(const char* input);
    ~Analyzer();

    int SkipSpaces();

    Node* GetG  (); // Get full expression
    Node* GetE  (); // Get addition or subtraction
    Node* GetT  (); // Get multiplication ot division
    Node* GetPow(); // Get power
    Node* GetP  (); // Get parenthesis
    Node* GetVN (); // Get one of variable, number, variable + number, sin, cos, ln
    Node* GetV  (); // Get variable
    Node* GetN  (); // Get number
    Node* GetSin(); // Get sin
    Node* GetCos(); // Get cos
    Node* GetL  (); // Get ln

    Node* GetSP(const char* func_name); //Get strict parenthesis

};

#endif // ANALYZER_H
