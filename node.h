#ifndef NODE_H
#define NODE_H

#include "diffconsts.h"

extern const char GraphHeader[];

class Node {
private:
    nod_val val;
    int type;
    Node* left  = NULL;
    Node* right = NULL;
    Node* operator +(Node* other);
    Node* operator -(Node* other);
    Node* operator -(int other);
    Node* operator *(Node* other);
    Node* operator /(Node* other);
    Node* operator ^(Node* other);
    Node* operator ^(int other);
public:
    Node();
    Node(int type);
    Node(int type, nod_val val);
    Node(int type, nod_val val, Node* left);
    Node(int type, nod_val val, Node* left, Node* right);
    void  NodeSet(int type);
    void  NodeSet(int type, nod_val val);
    void  NodeSet(int type, nod_val val, Node* left);
    void  NodeSet(int type, nod_val val, Node* left, Node* right);
    Node* NodeCopy();
    ~Node();

    nod_val GetVal();

    int     FPrintGraphNode(FILE* fd);
    int     MakeGraphFile(char* FileName);
    int     FPrintNode(FILE* fd);
    int     SaveTree(char* filename);
    void    TreeConstruct(char* filename);
    int     FScanNode(char* CurStr, FILE* fd);
    int     NumberOfNodes();
    nod_val TreeCount();

    bool    IsVar();
    Node*   Diff();
    int     Optimization();
};

#endif // NODE_H
