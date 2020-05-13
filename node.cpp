#include <iostream>
#include <string>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <fstream>
//#include "gtest/gtest.h"

#include "diffconsts.h"
#include "node.h"

// Used to make graph by graphwiz
const char GraphHeader[] = "digraph G{\n "
                           "node [style=\"filled\", fillcolor=\"orange\", fontcolor=\"blue\"];\n";

// Node::Constructor == Node::Node
//Free construtor
Node::Node() {}

// Construtor for numerical nodes
Node::Node(int type, nod_val val) {
    switch(type) {
    case NUM: {
        type_ = type;
        val_  = val;
        break;
    }
    case OP: {
        std::cerr << __PRETTY_FUNCTION__ << " for numbers used for operation!" << std::endl;
        NodeSet(type, val, new Node(NUM, 0), new Node(NUM, 0));
        break;
    }
    case VAR: {
        std::cerr << __PRETTY_FUNCTION__ << " for numbers used for variable!\n" << std::endl;
        NodeSet(type);
        break;
    }
    default:
        type_ = type;
        val_  = val;
        UNKNOWN_TYPE_MSG(type)
    }
}

// Construtor for variable nodes
Node::Node(int type) {
    switch(type) {
    case VAR:
        val_  = static_cast<nod_val>('x');
        type_ = VAR;
        break;

    case NUM:
        std::cerr <<  __PRETTY_FUNCTION__ << " for variables used for number!" << std::endl;
        NodeSet(type, 0);
        break;

    case OP:
        std::cerr <<  __PRETTY_FUNCTION__ << " for variables used for operation!" << std::endl;
        NodeSet(type, 0, new Node(NUM, 0), new Node(NUM, 0));
        break;

    default:
        type_ = type;
        val_  = 0; //static_cast<nod_val>('U');
        UNKNOWN_TYPE_MSG(type)
    }
}

// Construtor for operation nodes, but may use for any else nodes
Node::Node(int type, nod_val val, Node* left, Node* right) {
    switch(type) {
    case OP: {
        type_  = type;
        val_   = val;

        if(left == NULL) {
            std::cerr <<  __PRETTY_FUNCTION__ << ": left child of operation node is NULL!" << std::endl;
            left_ = new Node(NUM, 0);
        }
        else
            left_ = left;

        right_ = right;
        break;
    }
    case VAR: {
        if(left != NULL || right != NULL)
            std::cerr <<  __PRETTY_FUNCTION__ << " for operation used for variable!" << std::endl;
        NodeSet(type);
        break;
    }
    case NUM: {
        if(left != NULL || right != NULL)
            std::cerr <<  __PRETTY_FUNCTION__ << " for operation used for number!" << std::endl;
        NodeSet(type, val);
        break;
    }
    default: {
        type_  = type;
        val_   = val;
        left_  = left;
        right_ = right;
        UNKNOWN_TYPE_MSG(type)
    }
    }
}

// Construtor for unary operation nodes
Node::Node(int type, nod_val val, Node* left) {
    assert(left  != NULL);
    switch(type) {
    case OP: {
        type_  = type;
        val_   = val;
        left_  = left;
        break;
    }
    case NUM: {
        std::cerr <<  __PRETTY_FUNCTION__ << " for operations used for number!" << std::endl;
        NodeSet(type, val);
        break;
    }
    case VAR: {
        std::cerr <<  __PRETTY_FUNCTION__ << " for operations used for variable!" << std::endl;
        NodeSet(type);
        break;
    }
    default:
        type_  = type;
        val_   = val;
        left_  = left;
        UNKNOWN_TYPE_MSG(type)
    }
}

// Recursive destructor
Node::~Node() {
    if(left_ != NULL) {
        delete left_;
        left_ = NULL;
    }
    if(right_ != NULL) {
        delete right_;
        right_ = NULL;
    }
}

// Node fileds setter for numerical nodes
void Node::NodeSet(int type, nod_val val) {
    if(type != OP && type != VAR && type != NUM)
        UNKNOWN_TYPE_MSG(type)

    type_  = type;
    val_   = val;
}

// Node fileds setter for variable nodes
void Node::NodeSet(int type) {
    if(type != OP && type != VAR && type != NUM)
        UNKNOWN_TYPE_MSG(type)

    type_  = type;
    val_   = 'x';
}

// Node fileds setter for operation nodes
void Node::NodeSet(int type, nod_val val, Node* left, Node* right) {
    if(type != OP && type != VAR && type != NUM)
        UNKNOWN_TYPE_MSG(type)
    type_  = type;
    val_   = val;
    left_  = left;
    right_ = right;
}

// Node fileds setter for unary operation nodes
void Node::NodeSet(int type, nod_val val, Node* left) {
    if(type != OP && type != VAR && type != NUM)
        UNKNOWN_TYPE_MSG(type)
    type_  = type;
    val_   = val;
    left_  = left;
}

// Recursive copy tree, whose root is this node
Node* Node::NodeCopy() {
    Node* left = NULL;
    Node* right = NULL;
    if(left_ != NULL)
        left = left_->NodeCopy();
    if(right_ != NULL)
        right = right_->NodeCopy();
    return new Node(type_, val_, left, right);
}

// Value getter
nod_val Node::GetVal() {
    return val_;
}

// Support function for Node::MakeGraphFile - prints node
int Node::FPrintGraphNode(std::ofstream& out) {
    long id      = this - (Node*)0x0; // Unique id for every node, that helps building correct the graph
    long leftid  = 0;
    long rightid = 0;
    if(left_ != NULL) {
        leftid = left_ - (Node*)0x0;
        out << leftid <<" [label = ";
        if(left_->type_ == NUM) {
            out << "\"" << left_->val_ <<"\"]\n";
        }
        else
            out << "\"" << left_->val_ << "\"]\n";
        out << id << " -> " << leftid << "\n";
    }
    if(right_ != NULL) {
        rightid = right_ - (Node*)0x0;
        out << rightid << " [label = ";
        if(right_->type_ == NUM) {
            out << "\"" << right_->val_ << "\"]\n";
        }
        else
            out << "\"" << right_->val_ << "\"]\n";
        out << id <<" -> " << rightid << "\n";
    }
    if(left_ != NULL)
        left_->Node::FPrintGraphNode(out);
    if(this->right_ != NULL)
        right_->Node::FPrintGraphNode(out);
    return 0;
}

//Making file, that used by graphwiz to build graph
int Node::MakeGraphFile(const char* FileName) {
    assert(FileName != NULL);

    std::ofstream out(FileName);
    if(!out) {
        std::cerr << __PRETTY_FUNCTION__<< ": " << strerror(errno) << std::endl;
        return -1;
    }

    out << GraphHeader;
    long id = this - (Node*)0x0;                          // down
    out << id << " [label = ";
    if(type_ == NUM) {
        out << "\"" << val_ << "\"]\n";
    }
    else
        out << "\"" << val_ << "\"]\n";
    this->Node::FPrintGraphNode(out);
    out << "}\n";
    //fclose(fd);
    //out.close();
    return 0;
}

// Support function for Node::SaveTree, prints node
int Node::FPrintNode(std::ofstream& out) {
    if(!out) {
        std::cerr << __PRETTY_FUNCTION__<< ": " << strerror(errno) << std::endl;
    }
    //printf("%d %d\n", type_, val_);
    out << type_ << " " << val_ << std::endl;
    if(left_ != NULL) {
        //printf("(");
        out << "(";
        left_->Node::FPrintNode(out);
    }
    if(right_ != NULL) {
        //printf(",");
        out << ",";
        right_->Node::FPrintNode(out);
        //printf(")\n");
        out << ")\n";
    }
    else if(left_ != NULL) {
        //printf(")\n");
        out << ")\n";
    }
    return 0;
}

// Simple saving graph (expression) to file for future reset
int Node::SaveTree(const char* filename) {
    assert(filename != NULL);
    std::ofstream out(filename);
    if(!out) {
        std::cerr << __PRETTY_FUNCTION__<< ": " << strerror(errno) << std::endl;
        return -1;
    }
    
    out << type_ << " " << val_ << std::endl;
    out << type_ << " " << val_ << std::endl;
    if(left_ != NULL)
        left_->Node::FPrintNode(out);
    if(right_ != NULL)
        right_->Node::FPrintNode(out);
    //fclose(fd);
    return 0;
}

// Build tree from file
void Node::TreeConstruct(const char* filename) {
    assert(filename != NULL);
    std::ifstream in(filename);
    if(!in) {
        std::cerr << __PRETTY_FUNCTION__<< ": " << strerror(errno) << std::endl;
        return;
    }
    
    left_  = NULL;
    right_ = NULL;
    
    in >> type_;
    if(!in.good())
        return;
    in >> val_;
    
//    if(fscanf(fd, "%d%d", &type_, &val_) == EOF) {
//        fclose(fd);
//        return;
//    }

    char buff[MAX_NAME_LEN+1];
    in.getline(buff, MAX_NAME_LEN); //fgets(buff, MAX_NAME_LEN, fd);

    this->FScanNode(buff, in);
    //fclose(fd);
}

//Support function for Node::TreeConstruct (scan node)
int Node::FScanNode(char* CurStr, std::ifstream& in) {
    assert(!in.fail()); //assert(fd  != NULL);
    //char buff[MAX_NAME_LEN+1];
    if(!in.getline(CurStr, MAX_NAME_LEN + 1) || CurStr[0] == '\n') { //fgets(CurStr, MAX_NAME_LEN + 1, fd) == NULL || CurStr[0] == '\n') {
        return -1;
    }
    else {
        int BuffLen = strlen(CurStr);
        if(BuffLen > 0 && CurStr[BuffLen - 1] == '\n')
            CurStr[BuffLen - 1] = '\0';
        if(CurStr[0] != '(')
            return 0;
        else {
            left_ = new Node;
            sscanf(CurStr + 1, "%d%d", &left_->type_, &left_->val_);
            if(left_->Node::FScanNode(CurStr, in) == -1)
                return -1;

            if(CurStr[0] != ',') {
                if(CurStr[0] != ')') {
                    fprintf(stderr, "Invalid format of input file!!!\n");
                    return -1;
                }
            }
            else {
                right_ = new Node;
                sscanf(CurStr + 1, "%d%d", &right_->type_, &right_->val_);

                if(right_->Node::FScanNode(CurStr, in) == -1) return -1;
                if(CurStr[0] != ')') {
                    fprintf(stderr, "Invalid format of input file!!!\n");
                    return -1;
                }
            }
            in.getline(CurStr, MAX_NAME_LEN + 1);
            //fgets(CurStr, MAX_NAME_LEN + 1, fd);
        }
    }
    return 0;
}

// Calculate number of nodes in tree, started by this root (used by destructor)
int Node::NumberOfNodes() {
    int i = 0;
    if(left_ != NULL)
        i += left_->NumberOfNodes();
    if(right_ != NULL)
        i += right_->NumberOfNodes();
    return i + 1;
}

// Calculate expression, that contain in this tree
nod_val Node::TreeCount(nod_val var) {
    switch(type_) {
    case OP:
        if(left_ == NULL) {
            fprintf(stderr, "There is broken tree after operation \"%c\"!\n", val_);
            return 0;
        }
        switch(static_cast<char>(val_)) {
        case '+':
            if(right_ == NULL) {
                fprintf(stderr, "There is broken tree after operation \"%c\"!\n", val_);
            }
            return left_->TreeCount(var) + right_->TreeCount(var);
        case '-':
            if(right_ == NULL) {
                //fprintf(stderr, "There is broken tree after operation \"%c\"!\n", this->val);
                return (nod_val)((-1) * left_->TreeCount(var));
            }
            return left_->TreeCount(var) - right_->TreeCount(var);
        case '*':
            if(right_ == NULL) {
                fprintf(stderr, "There is broken tree after operation \"%c\"!\n", val_);
            }
            return left_->TreeCount(var) * right_->TreeCount(var);
        case '/':
            if(right_ == NULL) {
                fprintf(stderr, "There is broken tree after operation \"%c\"!\n", val_);
            }
            return left_->TreeCount(var) / right_->TreeCount(var);
        case '^':
            if(right_ == NULL) {
                fprintf(stderr, "There is broken tree after operation \"%c\"!\n", val_);
            }
            return (nod_val) pow(left_->TreeCount(var), right_->TreeCount(var));
        case 's':
            return (nod_val) sin(left_->TreeCount(var));
        case 'c':
            return (nod_val) cos(left_->TreeCount(var));
        case 'l':
            return (nod_val) log(left_->TreeCount(var));
        default:
            fprintf(stderr, "There is broken tree, unknown operation \"%c\"!\n", val_);
            return 0;
        }
        break;
    case NUM:
        return val_;
    case VAR:
        if(var != NAN)
            return var;
        else {
            fputs("TreeCount: Variable value is NAN!\n", stderr);
            return 0;
        }
    default:
        fprintf(stderr, "There is broken tree, unknown type number \"%d\"!\n", type_);
        return 0;
    }
}

// Operations with nodes
Node* Node::operator +(Node* other) {
    return new Node(OP, '+', this, other);
}

Node* Node::operator -(Node* other) {
    return new Node(OP, '-', this, other);
}

Node* Node::operator -(int other) {
    return new Node(OP, '-', this, new Node(NUM, other));
}

Node* Node::operator *(Node* other) {
    return new Node(OP, '*', this, other);
}

Node* Node::operator /(Node* other) {
    return new Node(OP, '/', this, other);
}

Node* Node::operator ^(Node* other) {
    return new Node(OP, '^', this, other);
}

Node* Node::operator ^(int other) {
    return new Node(OP, '^', this, new Node(NUM, other));
}

// Reccursive check, does this tree contain variable node
bool Node::IsVar() {
    if(type_ == VAR)
        return true;
    bool VarLeft, VarRight;
    if(left_ != NULL)
        VarLeft = left_->IsVar();
    else
        VarLeft = false;
    if(right_ != NULL)
        VarRight = right_->IsVar();
    else
        VarRight = false;
    return VarLeft || VarRight;
}

// Differentiation this tree
Node* Node::Diff() {
    if(type_ == NUM)
        return new Node(NUM, 0);
    if(type_ == VAR)
        return new Node(NUM, 1);
    if(type_ != OP) {
        fprintf(stderr, "Unknown type of node!\n");
        return NULL;
    }

    switch (static_cast<char>(val_)) {
    case '+':
        return *left_->Diff() + right_->Diff();
        break;
    case '-':
        if(right_ == NULL) {
            return new Node(OP, '-', left_->Diff());
        }
        return *left_->Diff() - right_->Diff();
        break;
    case '*':
        return *(*left_->Diff() * right_->NodeCopy()) + (*left_->NodeCopy() * right_->Diff());
        break;
    case '/':
        return *(*(*left_->Diff() * right_->NodeCopy()) - (*left_->NodeCopy() * right_->Diff())) / (*right_->NodeCopy() ^ 2);
        break;
    case 's':
        if(left_ == NULL) {
            fprintf(stderr, "Diff: No argument in sin!\n"); //argument of sin/cos must be left child
            return this->NodeCopy();
        }
        return *new Node(OP, 'c', left_->NodeCopy()) * left_->Diff();
        break;
    case 'c':
        if(left_ == NULL) {
            fprintf(stderr, "Diff: No argument in cos!\n"); //argument of cos/sin must be left child
            return this->NodeCopy();
        }
        return *(*new Node(NUM, 0) - new Node(OP, 's', left_->NodeCopy())) * left_->Diff();
        break;
    case '^':
        if(left_ == NULL || right_ == NULL) {
            fprintf(stderr, "Diff: Left or right child does not exists for /'^/'!\n");
            return this->NodeCopy();
        }

        //May be chack IsVar, but it will do optimization
        if(right_->IsVar() == false) { // (x^a)' = a * x^(a-1) * x'
            Node* mul1 = right_->NodeCopy();
            Node* mul2 = *left_->NodeCopy() ^ (*right_->NodeCopy() - 1);
            Node* mul3 = left_->Diff();
            return *mul1 * (*mul2 * mul3);
        }
        else if(left_->IsVar() == false){ // (a^x)' = a^x * ln(a) * x'
            Node* mul1 = this->NodeCopy();
            Node* mul2 = new Node(OP, 'l', left_->NodeCopy());
            Node* mul3 = right_->Diff(); //?
            return *mul1 * (*mul2 * mul3);
        }
        else { // (f^g)' = (e ^ (g * ln(f)))' = (f^g) * (g' * ln(f) + (f' / f) * g) = (x^x)(ln(x) + 1/x * x)
            Node* mul1 = this->NodeCopy();
            Node* add1 = *right_->Diff() * (new Node(OP, 'l', left_->NodeCopy()));
            Node* add2 = *(*left_->Diff() / left_->NodeCopy()) * right_->NodeCopy();
            return *mul1 * (*add1 + add2);
        }
        break;
    case 'l': // ln(x)' = (1/x) * x'
        if(left_ == NULL) {
            fprintf(stderr, "Diff: Left child does not exists for /'ln/'!\n");
            return this->NodeCopy();
        }
        return new Node(OP, '/', left_->Diff(), this->NodeCopy());
        break;
    default:
        fprintf(stderr, "Diff: Unknown operation \"%c\"!\n", val_);
        return NULL;
        break;
    }
}

// Reccursive optimization, delete nodes, that don't effect on the value of expression
int Node::Optimization() {
    switch(type_) {
    case NUM:
        return val_;
    case VAR:
        return -1;
    case OP: {
        int LeftVal  = 0;
        int RightVal = 0;
        if(left_ != NULL)
            LeftVal  = left_->Optimization();
        if(right_ != NULL)
            RightVal = right_->Optimization();
        Node* Left  = left_;
        Node* Right = right_;
        switch (static_cast<char>(val_)) {
        case '+':
        case '-':
            if(RightVal == 0 && LeftVal == 0) {
                delete left_;
                delete right_;
                left_  = NULL;
                right_ = NULL;
                this->NodeSet(NUM, 0);
                return 0;
            }
            else if(RightVal == 0) {
                delete right_;
                right_ = NULL;
                this->NodeSet(left_->type_, left_->val_, left_->left_, left_->right_);
                Left->left_  = NULL;
                Left->right_ = NULL;
                delete Left;
                Left = NULL;
                return val_;
            }
            else if(LeftVal == 0) {
                delete left_;
                left_ = NULL;
                if(val_ == '-')
                    return '-';
                this->NodeSet(right_->type_, right_->val_, right_->left_, right_->right_);
                Right->left_  = NULL;
                Right->right_ = NULL;
                delete Right;
                Right = NULL;
                return val_;
            }
            break;

        case '*':
            if(LeftVal == 0 || RightVal == 0) {
                delete left_;
                delete right_;
                left_  = NULL;
                right_ = NULL;
                this->NodeSet(NUM, 0);
                return 0;
            }
            else if (RightVal == 1 && LeftVal == 1) {
                delete left_;
                delete right_;
                left_  = NULL;
                right_ = NULL;
                this->NodeSet(NUM, 1);
                return 1;
            }
            else if(RightVal == 1) {
                delete right_;
                right_ = NULL;
                this->NodeSet(left_->type_, left_->val_, left_->left_, left_->right_);
                Left->left_  = NULL;
                Left->right_ = NULL;
                delete Left;
                Left = NULL;
                return val_;
            }
            else if(LeftVal == 1) {
                delete left_;
                left_ = NULL;
                this->NodeSet(right_->type_, right_->val_, right_->left_, right_->right_);
                Right->left_  = NULL;
                Right->right_ = NULL;
                delete Right;
                Right = NULL;
                return val_;
            }
            break;
        case '/':
            if(RightVal == 0) {
                fprintf(stderr, "В дереве есть деление на ноль!\n");
                return '/';
            }
            else if(LeftVal == 0) {
                delete left_;
                delete right_;
                left_  = NULL;
                right_ = NULL;
                this->NodeSet(NUM, 0);
                return 0;
            }
            else if (RightVal == 1 && LeftVal == 1) {
                delete left_;
                delete right_;
                left_  = NULL;
                right_ = NULL;
                this->NodeSet(NUM, 1);
                return 1;
            }
            else if(RightVal == 1) {
                delete right_;
                right_ = NULL;
                this->NodeSet(left_->type_, left_->val_, left_->left_, left_->right_);
                Left->left_  = NULL;
                Left->right_ = NULL;
                delete Left;
                Left = NULL;
                return val_;
            }
            break;
        case 's':
        case 'c':
            if(LeftVal == 0) {
                delete left_;
                left_  = NULL;
                if(val_ == 's')
                    this->NodeSet(NUM, 0);
                else
                    this->NodeSet(NUM, 1);
                return val_;
            }
            break;
        case '^':
            if(LeftVal == 0) {
                if(RightVal < 0) {
                    fprintf(stderr, "В дереве есть деление на 0!\n");
                    return '^';
                }
                else {
                    delete left_;
                    delete right_;
                    left_  = NULL;
                    right_ = NULL;
                    this->NodeSet(NUM, 0);
                    return 0;
                }
            }
            if(LeftVal == 1 || RightVal == 0) {
                delete left_;
                delete right_;
                left_  = NULL;
                right_ = NULL;
                this->NodeSet(NUM, 1);
                return 1;
            }
            else if(RightVal == 1) {
                delete right_;
                right_ = NULL;
                this->NodeSet(left_->type_, left_->val_, left_->left_, left_->right_);
                Left->left_  = NULL;
                Left->right_ = NULL;
                delete Left;
                Left = NULL;
                return val_;
            }
            break;
        case 'l':
            return val_;
        default:
            fprintf(stderr, "Unknown operation %c!\n", val_);
            break;
        }
        break;
    }
    default:
        fprintf(stderr, "Optimization: Unknown type \"%d\"!\n", type_);
    }
    return val_;
}


