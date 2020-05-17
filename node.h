#ifndef NODE_H
#define NODE_H

#include <fstream>

#include "diffconsts.h"

extern const char GraphHeader[];

extern const size_t MAX_DIVIDEND_LEN;

class Node {
private:
    // The value can be a number (in the case of NUM)
    // or a character code (in the case of OP)
    nod_val val_;
    // Possible types: NUM - number, VAR - variable, OP - operand
    int     type_;

    Node* left_  = nullptr; // left node
    Node* right_ = nullptr; // right node

public:
    Node ();                      // Empty constructor
    Node (int type);              // Constructor for variables
    Node (int type, nod_val val); // Constructor for numbers

    Node (int type, nod_val val, Node* left);              // Constructor for unary operators
    Node (int type, nod_val val, Node* left, Node* right); // Constructor for binary operators

    void  NodeSet (int type);              // Node fileds setter for variable nodes
    void  NodeSet (int type, nod_val val); // Node fileds setter for numerical nodes

    void  NodeSet (int type, nod_val val, Node* left);              // Node fileds setter for unary operation nodes
    void  NodeSet (int type, nod_val val, Node* left, Node* right); // Node fileds setter for binary operation nodes

    Node* NodeCopy(); // Construct a same tree
    ~Node();          // Destructor

    // Operators between nodes, that make a new node with corresponding operator
    Node* operator +(Node* other);
    Node* operator -(Node* other);
    Node* operator -(int other);
    Node* operator *(Node* other);
    Node* operator /(Node* other);
    Node* operator ^(Node* other);
    Node* operator ^(int other);

    nod_val GetVal(); // Value getter (this.val_)

    size_t  NumberOfNodes   ();
    nod_val TreeCount       (nod_val var);

    // Used to make tex file with expression
    // If completed == true (default), then the full structure of the tex file is built
    int MakeTex           (const char* FileName, bool completed = true);
    int MakeTex           (std::ofstream& out,   bool completed = false);

    // Support recursive function, used in Node::MakeTex,
    // prints node and his children to ifstream out
    int MakeTexNode       (std::ofstream& out);

    // Support recursive functions, used in Node::MakeTexNode,
    // prints node of type binary/unary OP and his children to ifstream out
    int MakeBinaryNodeTex (std::ofstream& out, char val_char, const char *left = "", const char *center = "", const char *right = "");
    int MakeUnaryNodeTex  (std::ofstream& out, char val_char, const char *left = "", const char *right  = "");

    bool    IsVar(); // Reccursive check, does this tree contain variable node
    Node*   Diff (); // Differentiation this tree

    // Reccursive optimization, delete nodes, that don't effect on the value of expression
    int Optimization();
    // Differentiates the expression and writes the result to a .tex file
    int DiffTex     (const char* FileName, bool optim = true);

    //                                           OPTIONAL PART

    int     MakeGraphFile   (const char* FileName); //Making file, that used by graphwiz to build graph
    int     FPrintGraphNode (std::ofstream& fd);    // Support function for Node::MakeGraphFile - prints node

    int     SaveTree        (const char* filename);            // Simple saving graph (expression) to file for future reset
    int     FPrintNode      (std::ofstream& fd);               // Support function for Node::SaveTree, prints node
    void    TreeConstruct   (const char* filename);            // Build tree from file
    int     FScanNode       (char* CurStr, std::ifstream& fd); //Support function for Node::TreeConstruct (scan node)
};

// Generate array (writing to brae[][2]), that contain "(\0" and ")\0" | or | "[\0" and "]\0", probability: 2:1
void RandomBrace(char brace[][2]);
// Message to std::cerr that type is unknown
void UnknownTypeMsg(int type);

// Support def, for checking MakeTexNode correctness
#define MAKE_NODE_TEX(NODE, OPER, STREAM, MSG) \
if(NODE) {                                     \
   if(NODE->MakeTexNode(STREAM) == -1)         \
       return -1;                              \
} else {                                       \
    std::cerr << MSG << std::endl;             \
    return -1;                                 \
}

// Support def, for checking MakeTexNode correctness. For left Node.
#define MAKE_LEFT_NODE_TEX(LEFT, OPER, STREAM)                                          \
    char MAKE_LEFT_NODE_TEX_MSG_[128];                                                  \
    sprintf(MAKE_LEFT_NODE_TEX_MSG_, "Operation \'%c\' must have left operand!", OPER); \
    MAKE_NODE_TEX(LEFT, OPER, STREAM, MAKE_LEFT_NODE_TEX_MSG_);

// Support def, for checking MakeTexNode correctness. For right Node.
#define MAKE_RIGHT_NODE_TEX(RIGHT, OPER, STREAM)                                              \
    char MAKE_RIGHT_NODE_TEX_MSG_[128];                                                       \
    sprintf(MAKE_RIGHT_NODE_TEX_MSG_, "Binary operation \'%c\' must have 2 operands!", OPER); \
    MAKE_NODE_TEX(RIGHT, OPER, STREAM, MAKE_RIGHT_NODE_TEX_MSG_);


#endif // NODE_H
