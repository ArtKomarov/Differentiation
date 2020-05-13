#ifndef NODE_H
#define NODE_H

//#include <iostream>
#include <fstream>

#include "diffconsts.h"

extern const char GraphHeader[];

#define UNKNOWN_TYPE_MSG(type) \
    std::cerr << __PRETTY_FUNCTION__ << ": unknown type! Number of type " << type \
              << ", equal character " << static_cast<char>(type) << "!" << std::endl;

class Node {
private:
    nod_val val_;
    int     type_;

    Node* left_  = NULL;
    Node* right_ = NULL;

    Node* operator +(Node* other);
    Node* operator -(Node* other);
    Node* operator -(int other);
    Node* operator *(Node* other);
    Node* operator /(Node* other);
    Node* operator ^(Node* other);
    Node* operator ^(int other);
public:
    Node ();
    Node (int type);
    Node (int type, nod_val val);
    Node (int type, nod_val val, Node* left);
    Node (int type, nod_val val, Node* left, Node* right);

    void  NodeSet (int type);
    void  NodeSet (int type, nod_val val);
    void  NodeSet (int type, nod_val val, Node* left);
    void  NodeSet (int type, nod_val val, Node* left, Node* right);

    Node* NodeCopy();
    ~Node();

    nod_val GetVal();

    int     FPrintGraphNode (std::ofstream& fd);
    int     MakeGraphFile   (const char* FileName);
    int     FPrintNode      (std::ofstream& fd);
    int     SaveTree        (const char* filename);
    void    TreeConstruct   (const char* filename);
    int     FScanNode       (char* CurStr, std::ifstream& fd);
    int     NumberOfNodes   ();
    nod_val TreeCount       (nod_val var);

    bool    IsVar();
    Node*   Diff ();

    int     Optimization();
};

class Node2 : public Node {
};

#endif // NODE_H
