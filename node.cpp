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
const char TEX_HEADER[]  =
       "\\documentclass[14pt]{article}\n"
       "\\usepackage{fancyhdr}\n"
       "\\usepackage[utf8]{inputenc}\n"
       "\\usepackage[russian]{babel}\n"
       "\\begin{document}\n "
       "\\pagestyle{fancy}\n"
       "\\fancyhead[R]{Acronis}\n"
       "\\fancyhead[L]{\"Утрем нос Стивену Вольфраму!\"}\n $";
const char TEX_END_DOCUMENT[] = "$\\end{document}";

const size_t MAX_DIVIDEND_LEN = 15;
//const size_t MAX_BRACE_LEN    = 20;

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
        UnknownTypeMsg(type);
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
        UnknownTypeMsg(type);
    }
}

// Construtor for operation nodes, but may use for any else nodes
Node::Node(int type, nod_val val, Node* left, Node* right) {
    switch(type) {
    case OP: {
        type_  = type;
        val_   = val;

        if(left == nullptr) {
            std::cerr <<  __PRETTY_FUNCTION__ << ": left child of operation node is nullptr!" << std::endl;
            left_ = new Node(NUM, 0);
        }
        else
            left_ = left;

        right_ = right;
        break;
    }
    case VAR: {
        if(left != nullptr || right != nullptr)
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
        UnknownTypeMsg(type);
    }
    }
}

// Construtor for unary operation nodes
Node::Node(int type, nod_val val, Node* left) {
    assert(left  != nullptr);
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
        UnknownTypeMsg(type);
    }
}

// Recursive destructor
Node::~Node() {
    if(left_ != nullptr) {
        delete left_;
        left_ = nullptr;
    }
    if(right_ != nullptr) {
        delete right_;
        right_ = nullptr;
    }
}

// Node fileds setter for numerical nodes
void Node::NodeSet(int type, nod_val val) {
    if(type != OP && type != VAR && type != NUM)
        UnknownTypeMsg(type);

    type_  = type;
    val_   = val;
}

// Node fileds setter for variable nodes
void Node::NodeSet(int type) {
    if(type != OP && type != VAR && type != NUM)
        UnknownTypeMsg(type);

    type_  = type;
    val_   = 'x';
}

// Node fileds setter for operation nodes
void Node::NodeSet(int type, nod_val val, Node* left, Node* right) {
    if(type != OP && type != VAR && type != NUM)
        UnknownTypeMsg(type);
    type_  = type;
    val_   = val;
    left_  = left;
    right_ = right;
}

// Node fileds setter for unary operation nodes
void Node::NodeSet(int type, nod_val val, Node* left) {
    if(type != OP && type != VAR && type != NUM)
        UnknownTypeMsg(type);
    type_  = type;
    val_   = val;
    left_  = left;
}

// Recursive copy tree, whose root is this node
Node* Node::NodeCopy() {
    Node* left = nullptr;
    Node* right = nullptr;
    if(left_ != nullptr)
        left = left_->NodeCopy();
    if(right_ != nullptr)
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
    if(left_ != nullptr) {
        leftid = left_ - (Node*)0x0;   // down
        out << leftid <<" [label = ";
        if(left_->type_ == NUM) {
            out << "\"" << left_->val_ <<"\"]\n";
        }
        else
            out << "\"" << static_cast<char>(left_->val_) << "\"]\n";
        out << id << " -> " << leftid << "\n";
    }
    if(right_ != nullptr) {
        rightid = right_ - (Node*)0x0;
        out << rightid << " [label = ";

        if(right_->type_ == NUM) {
            out << "\"" << right_->val_ << "\"]\n";
        }
        else
            out << "\"" << static_cast<char>(right_->val_) << "\"]\n";

        out << id <<" -> " << rightid << "\n";
    }
    if(left_ != nullptr)
        left_->Node::FPrintGraphNode(out);
    if(this->right_ != nullptr)
        right_->Node::FPrintGraphNode(out);
    return 0;
}

//Making file, that used by graphwiz to build graph
int Node::MakeGraphFile(const char* FileName) {
    assert(FileName != nullptr);

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
        out << "\"" << static_cast<char>(val_) << "\"]\n";

    this->Node::FPrintGraphNode(out);
    out << "}\n";
    //fclose(fd);
    //out.close();
    return 0;
}

// Calculate number of nodes in tree, started by this root (used by destructor)
size_t Node::NumberOfNodes() {
    int i = 0;
    if(left_ != nullptr)
        i += left_->NumberOfNodes();
    if(right_ != nullptr)
        i += right_->NumberOfNodes();
    return i + 1;
}

// Calculate expression, that contain in this tree
nod_val Node::TreeCount(nod_val var) {
    const char ERROR_MSG[] = "There is broken tree after operation \'";
    switch(type_) {
    case OP:
        if(left_ == nullptr) {
            std::cerr << ERROR_MSG << static_cast<char>(val_) << "\'!" << std::endl;
            return 0;
        }
        switch(static_cast<char>(val_)) {
        case '+':
            if(right_ == nullptr) {
                std::cerr << ERROR_MSG << static_cast<char>(val_) << "\'!" << std::endl;
            }
            return left_->TreeCount(var) + right_->TreeCount(var);
        case '-':
            if(right_ == nullptr) {
                //fprintf(stderr, "There is broken tree after operation \"%c\"!\n", this->val);
                return (nod_val)((-1) * left_->TreeCount(var));
            }
            return left_->TreeCount(var) - right_->TreeCount(var);
        case '*':
            if(right_ == nullptr) {
                std::cerr << ERROR_MSG << static_cast<char>(val_) << "\'!" << std::endl;
            }
            return left_->TreeCount(var) * right_->TreeCount(var);
        case '/':
            if(right_ == nullptr) {
                std::cerr << ERROR_MSG << static_cast<char>(val_) << "\'!" << std::endl;
            }
            return left_->TreeCount(var) / right_->TreeCount(var);
        case '^':
            if(right_ == nullptr) {
                std::cerr << ERROR_MSG << static_cast<char>(val_) << "\'!" << std::endl;
            }
            return (nod_val) pow(left_->TreeCount(var), right_->TreeCount(var));
        case 's':
            return (nod_val) sin(left_->TreeCount(var));
        case 'c':
            return (nod_val) cos(left_->TreeCount(var));
        case 'l':
            return (nod_val) log(left_->TreeCount(var));
        default:
            std::cerr << "There is broken tree, unknown operation \'" << static_cast<char>(val_) << "\'!" << std::endl;
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

// Used to make tex file with expression
// If completed == true (default), then the full structure of the tex file is built
int Node::MakeTex (const char* FileName, bool completed) {
    assert(FileName != nullptr);

    std::ofstream out;
    if(completed)
        out.open(FileName);
    else
        out.open(FileName, std::ios::app);
    if(!out) {
        std::cerr << __PRETTY_FUNCTION__<< ": " << strerror(errno) << std::endl;
        return -1;
    }

    if(completed)
        out << TEX_HEADER;

    this->Node::MakeTexNode(out);
    if(completed)
        out << TEX_END_DOCUMENT;
    return 0;
}

// Used to make tex file with expression
// If completed == false (default), then the incomplete tex file is built (for appending)
int Node::MakeTex (std::ofstream& out, bool completed) {
    if(!out) {
        std::cerr << __PRETTY_FUNCTION__<< ": " << strerror(errno) << std::endl;
        return -1;
    }

    if(completed)
        out << TEX_HEADER;

    this->Node::MakeTexNode(out);

    if(completed)
        out << TEX_END_DOCUMENT;
    return 0;
}

// Support recursive function, used in Node::MakeTex,
//prints node and his children to ifstream out
int Node::MakeTexNode (std::ofstream& out) {
    if(!out) {
        std::cerr << __PRETTY_FUNCTION__<< ": " << strerror(errno) << std::endl;
        return -1;
    }

    switch(type_) {
    case OP: {
        char brace[2][2];
        RandomBrace(brace);
        char val_char = static_cast<char>(val_);
        switch (val_char) {
        case '+':
        case '-': {
            char oper[5]; // 'operand' + maybe brace + '\0'
            if(right_) {
                sprintf(oper, "%c", val_char);
                if(this->MakeBinaryNodeTex(out, val_char, "", oper) == -1)
                    return -1;
            }
            else {
                sprintf(oper, "%c%s", val_char, brace[0]);
                if(this->MakeUnaryNodeTex(out, val_char, oper, brace[1]) == -1)
                    return -1;
            }
            break;
        }
        case '*': {
            char center[2*2 + 8 + 1];
            sprintf(center, "%s \\cdot %s", brace[1], brace[0]);
            //if(this->MakeBinaryNodeTex(out, val_char, "(", ") \\cdot (", ")") == -1)
            if(this->MakeBinaryNodeTex(out, val_char, brace[0], center, brace[1]) == -1)
                return -1;
            break;
        }
        case '/':
            //Если будет длинная дробь, она не влезет на страницу,
            // поэтому используем \frac{}{} только для не большой глубины дерева (<= MAX_DIVIDEND_LEN)
            if(this->NumberOfNodes() <= MAX_DIVIDEND_LEN) {
                if(this->MakeBinaryNodeTex(out, val_char, "\\frac{", "}{", "}") == -1)
                    return -1;
            }
            else {
                char center[2*2 + 4];
                sprintf(center, "%s / %s", brace[1], brace[0]);
                if(this->MakeBinaryNodeTex(out, val_char, brace[0], center, brace[1]) == -1)
                    return -1;
            }
            break;
        case '^': { // К сожалению, тоже ставим скобки т.к. a^b^c - latex fail
            if(left_->type_ != OP && right_->type_ != OP) { // кроме самого простого случая
                if(this->MakeBinaryNodeTex(out, val_char, "", "^{", "}") == -1)
                    return -1;
                break;
            }
            char center[8];
            sprintf(center, "%s^{%s", brace[1], brace[0]);
            char right[8];
            sprintf(right, "%s}", brace[1]);
            if(this->MakeBinaryNodeTex(out, val_char, brace[0], center, right) == -1)
                return -1;
            break;
        }
        case 's': {
            char lef[8];
            sprintf(lef, "\\sin%s", brace[0]);
            if(this->MakeUnaryNodeTex(out, val_char, lef, brace[1]) == -1)
                return -1;
            break;
        }
        case 'c': {
            char lef[8];
            sprintf(lef, "\\cos%s", brace[0]);
            if(this->MakeUnaryNodeTex(out, val_char, lef, brace[1]) == -1)
                return -1;
            break;
        }
        case 'l': {
            char lef[8];
            sprintf(lef, "\\ln%s", brace[0]);
            if(this->MakeUnaryNodeTex(out, val_char, lef, brace[1]) == -1)
                return -1;
            break;
        }
        default:
            left_->MakeTexNode(out);
            out << val_char;
            right_->MakeTexNode(out);
            break;
        }
        break;
    }
    case NUM:
        out << val_;
        break;
    case VAR:
        out << static_cast<char>(val_);
        break;
    default:
        std::cerr << __PRETTY_FUNCTION__<< ": " << "Unknown type of node!";
        return -1;
    }
    return 0;
}

// Support recursive function, used in Node::MakeTexNode,
//prints node of type binary OP and his children to ifstream out
int Node::MakeBinaryNodeTex(std::ofstream& out, char val_char, const char *left, const char *center, const char *right) {
    out << left;
    MAKE_LEFT_NODE_TEX (left_,  val_char, out);
    out << center;
    MAKE_RIGHT_NODE_TEX(right_, val_char, out);
    out << right;
    return 0;
}

// Support recursive function, used in Node::MakeTexNode,
//prints node of type unary OP and his children to ifstream out
int Node::MakeUnaryNodeTex(std::ofstream& out, char val_char, const char *left, const char *right) {
    out << left;
    MAKE_LEFT_NODE_TEX(left_, val_char, out);
    out << right;
    return 0;
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
    if(left_ != nullptr)
        VarLeft = left_->IsVar();
    else
        VarLeft = false;
    if(right_ != nullptr)
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
        return nullptr;
    }

    switch (static_cast<char>(val_)) {
    case '+':
        return *left_->Diff() + right_->Diff();
        break;
    case '-':
        if(right_ == nullptr) {
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
        if(left_ == nullptr) {
            fprintf(stderr, "Diff: No argument in sin!\n"); //argument of sin/cos must be left child
            return this->NodeCopy();
        }
        return *new Node(OP, 'c', left_->NodeCopy()) * left_->Diff();
        break;
    case 'c':
        if(left_ == nullptr) {
            fprintf(stderr, "Diff: No argument in cos!\n"); //argument of cos/sin must be left child
            return this->NodeCopy();
        }
        return *(*new Node(NUM, 0) - new Node(OP, 's', left_->NodeCopy())) * left_->Diff();
        break;
    case '^':
        if(left_ == nullptr || right_ == nullptr) {
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
        if(left_ == nullptr) {
            std::cerr << __PRETTY_FUNCTION__ << ": Left child does not exists for \"ln\"!" << std::endl;
            return this->NodeCopy();
        }
        return new Node(OP, '/', left_->Diff(), left_->NodeCopy());
        break;
    default:
        std::cerr << __PRETTY_FUNCTION__ << ": Unknown operation \'" << val_ << "\'!" << std::endl;
        return nullptr;
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
        if(left_ != nullptr)
            LeftVal  = left_->Optimization();
        if(right_ != nullptr)
            RightVal = right_->Optimization();
        Node* Left  = left_;
        Node* Right = right_;
        switch (static_cast<char>(val_)) {
        case '+':
        case '-':
            if(RightVal == 0 && LeftVal == 0) {
                delete left_;
                delete right_;
                left_  = nullptr;
                right_ = nullptr;
                this->NodeSet(NUM, 0);
                return 0;
            }
            else if(RightVal == 0 && right_ != nullptr) {
                delete right_;
                right_ = nullptr;
                this->NodeSet(left_->type_, left_->val_, left_->left_, left_->right_);
                Left->left_  = nullptr;
                Left->right_ = nullptr;
                delete Left;
                Left = nullptr;
                return val_;
            }
            else if(LeftVal == 0) {
                delete left_;

                if(val_ == '-') {
                    left_ = right_;
                    right_ = nullptr;
                    return val_;
                }

                left_ = nullptr;
                this->NodeSet(right_->type_, right_->val_, right_->left_, right_->right_);
                Right->left_  = nullptr;
                Right->right_ = nullptr;

                delete Right;
                Right = nullptr;
                return val_;
            }
            break;

        case '*':
            if(LeftVal == 0 || RightVal == 0) {
                delete left_;
                delete right_;
                left_  = nullptr;
                right_ = nullptr;
                this->NodeSet(NUM, 0);
                return 0;
            }
            else if (RightVal == 1 && LeftVal == 1) {
                delete left_;
                delete right_;
                left_  = nullptr;
                right_ = nullptr;
                this->NodeSet(NUM, 1);
                return 1;
            }
            else if(RightVal == 1) {
                delete right_;
                right_ = nullptr;
                this->NodeSet(left_->type_, left_->val_, left_->left_, left_->right_);
                Left->left_  = nullptr;
                Left->right_ = nullptr;
                delete Left;
                Left = nullptr;
                return val_;
            }
            else if(LeftVal == 1) {
                delete left_;
                left_ = nullptr;
                this->NodeSet(right_->type_, right_->val_, right_->left_, right_->right_);
                Right->left_  = nullptr;
                Right->right_ = nullptr;
                delete Right;
                Right = nullptr;
                return val_;
            }
            break;
        case '/':
            if(RightVal == 0) {
                std::cerr << "The tree has a division by 0!" << std::endl;
                return '/';
            }
            else if(LeftVal == 0) {
                delete left_;
                delete right_;
                left_  = nullptr;
                right_ = nullptr;
                this->NodeSet(NUM, 0);
                return 0;
            }
            else if (RightVal == 1 && LeftVal == 1) {
                delete left_;
                delete right_;
                left_  = nullptr;
                right_ = nullptr;
                this->NodeSet(NUM, 1);
                return 1;
            }
            else if(RightVal == 1) {
                delete right_;
                right_ = nullptr;
                this->NodeSet(left_->type_, left_->val_, left_->left_, left_->right_);
                Left->left_  = nullptr;
                Left->right_ = nullptr;
                delete Left;
                Left = nullptr;
                return val_;
            }
            break;
        case 's':
        case 'c':
            if(LeftVal == 0) {
                delete left_;
                left_  = nullptr;
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
                    std::cerr << "The tree has a division by 0!" << std::endl;
                    return '^';
                }
                else {
                    delete left_;
                    delete right_;
                    left_  = nullptr;
                    right_ = nullptr;
                    this->NodeSet(NUM, 0);
                    return 0;
                }
            }
            if(LeftVal == 1 || RightVal == 0) {
                delete left_;
                delete right_;
                left_  = nullptr;
                right_ = nullptr;
                this->NodeSet(NUM, 1);
                return 1;
            }
            else if(RightVal == 1) {
                delete right_;
                right_ = nullptr;
                this->NodeSet(left_->type_, left_->val_, left_->left_, left_->right_);
                Left->left_  = nullptr;
                Left->right_ = nullptr;
                delete Left;
                Left = nullptr;
                return val_;
            }
            break;
        case 'l':
            return val_;
        default:
            std::cerr << "Unknown operation \'" << val_ << "\'!" << std::endl;
            break;
        }
        break;
    }
    default:
        std::cerr << "Optimization: Unknown type \'" << type_ << "\'!" << std::endl;
    }
    return val_;
}

// Differentiates the expression and writes the result to a .tex file
int Node::DiffTex (const char* FileName, bool optim) {
    assert(FileName != nullptr);

    std::ofstream out(FileName);
    if(!out) {
        std::cerr << __PRETTY_FUNCTION__<< ": " << strerror(errno) << std::endl;
        return -1;
    }

    if(optim)
        this->Optimization();

    Node* diff = this->Diff();
    if(diff == nullptr)
        return -1;

    if(optim)
        diff->Optimization();

    out << TEX_HEADER << "(\\;";
    if(this->MakeTex(out) == -1)
        return -1;

    out << "\\;)' = ";

    if(diff->MakeTex(out) == -1)
        return -1;
    out << TEX_END_DOCUMENT;
    return 0;
}

// Message to std::cerr that type is unknown
void UnknownTypeMsg(int type) {
    std::cerr << __PRETTY_FUNCTION__ << ": unknown type! Number of type " << type
              << ", equal character " << static_cast<char>(type) << "!" << std::endl;
}

// Generate array (writing to brae[][2]), that contain "(\0" and ")\0" | or | "[\0" and "]\0", probability: 2:1
void RandomBrace(char brace[][2]) {
    int brace_id = rand() % 3;      // 2:1 ():[]
    //char brace[2][MAX_BRACE_LEN];
    switch (brace_id) {
    case 0:
    case 1:
        brace[0][0] = '(';
        brace[0][1] = '\0';
        brace[1][0] = ')';
        brace[1][1] = '\0';
        break;
    case 2:
        brace[0][0] = '[';
        brace[0][1] = '\0';
        brace[1][0] = ']';
        brace[1][1] = '\0';
        break;
    }
    //return brace;
}

//                                                         OPTIONAL PART

// Support function for Node::SaveTree, prints node
int Node::FPrintNode(std::ofstream& out) {
    if(!out) {
        std::cerr << __PRETTY_FUNCTION__<< ": " << strerror(errno) << std::endl;
    }
    //printf("%d %d\n", type_, val_);
    out << type_ << " " << val_ << std::endl;
    if(left_ != nullptr) {
        //printf("(");
        out << "(";
        left_->Node::FPrintNode(out);
    }
    if(right_ != nullptr) {
        //printf(",");
        out << ",";
        right_->Node::FPrintNode(out);
        //printf(")\n");
        out << ")\n";
    }
    else if(left_ != nullptr) {
        //printf(")\n");
        out << ")\n";
    }
    return 0;
}

// Simple saving graph (expression) to file for future reset
int Node::SaveTree(const char* filename) {
    assert(filename != nullptr);
    std::ofstream out(filename);
    if(!out) {
        std::cerr << __PRETTY_FUNCTION__<< ": " << strerror(errno) << std::endl;
        return -1;
    }

    out << type_ << " " << val_ << std::endl;
    out << type_ << " " << val_ << std::endl;
    if(left_ != nullptr)
        left_->Node::FPrintNode(out);
    if(right_ != nullptr)
        right_->Node::FPrintNode(out);
    //fclose(fd);
    return 0;
}

// Build tree from file
void Node::TreeConstruct(const char* filename) {
    assert(filename != nullptr);
    std::ifstream in(filename);
    if(!in) {
        std::cerr << __PRETTY_FUNCTION__<< ": " << strerror(errno) << std::endl;
        return;
    }

    left_  = nullptr;
    right_ = nullptr;

    in >> type_;
    if(!in.good())
        return;
    in >> val_;

    char buff[MAX_NAME_LEN+1];
    in.getline(buff, MAX_NAME_LEN); //fgets(buff, MAX_NAME_LEN, fd);

    this->FScanNode(buff, in);
}

//Support function for Node::TreeConstruct (scan node)
int Node::FScanNode(char* CurStr, std::ifstream& in) {
    assert(!in.fail()); //assert(fd  != nullptr);
    //char buff[MAX_NAME_LEN+1];
    if(!in.getline(CurStr, MAX_NAME_LEN + 1) || CurStr[0] == '\n') {
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
            sscanf(CurStr + 1, "%d%lf", &left_->type_, &left_->val_);
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
                sscanf(CurStr + 1, "%d%lf", &right_->type_, &right_->val_);

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
