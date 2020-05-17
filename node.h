#ifndef NODE_H
#define NODE_H

//#include <iostream>
#include <fstream>

#include "diffconsts.h"

extern const char GraphHeader[];

extern const size_t MAX_DIVIDEND_LEN;

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

    size_t  NumberOfNodes   ();
    nod_val TreeCount       (nod_val var);

    int MakeTex           (const char* FileName, bool completed = true);
    int MakeTex           (std::ofstream& out,   bool completed = false);
    int MakeTexNode       (std::ofstream& out);
    int MakeBinaryNodeTex (std::ofstream& out, char val_char, const char *left = "", const char *center = "", const char *right = "");
    int MakeUnaryNodeTex  (std::ofstream& out, char val_char, const char *left = "", const char *right  = "");

    bool    IsVar();
    Node*   Diff ();

    int Optimization();
    int DiffTex     (const char* FileName, bool optim = true);
};

class Node2 : public Node {
};

#define MAKE_NODE_TEX(NODE, OPER, STREAM, MSG) \
if(NODE) {                                     \
   if(NODE->MakeTexNode(STREAM) == -1)         \
       return -1;                              \
} else {                                       \
    std::cerr << MSG << std::endl;             \
    return -1;                                 \
}

#define MAKE_LEFT_NODE_TEX(LEFT, OPER, STREAM)                                          \
    char MAKE_LEFT_NODE_TEX_MSG_[128];                                                  \
    sprintf(MAKE_LEFT_NODE_TEX_MSG_, "Operation \'%c\' must have left operand!", OPER); \
    MAKE_NODE_TEX(LEFT, OPER, STREAM, MAKE_LEFT_NODE_TEX_MSG_);

#define MAKE_RIGHT_NODE_TEX(RIGHT, OPER, STREAM)                                              \
    char MAKE_RIGHT_NODE_TEX_MSG_[128];                                                       \
    sprintf(MAKE_RIGHT_NODE_TEX_MSG_, "Binary operation \'%c\' must have 2 operands!", OPER); \
    MAKE_NODE_TEX(RIGHT, OPER, STREAM, MAKE_RIGHT_NODE_TEX_MSG_);



#endif // NODE_H
