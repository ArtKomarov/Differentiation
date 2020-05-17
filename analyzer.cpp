#include <iostream>
#include <string>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "diffconsts.h"
#include "analyzer.h"

// Constructor
Analyzer::Analyzer(const char* s) {
    assert(s != nullptr);
    this->s = strdup(s);
    this->slen = (int) strlen(s);
    this->tree = this->GetG(); // Analyze expression
}

// Destructor
Analyzer::~Analyzer() {
    while(*s != '\0') s++;
    free(this->s - this->slen);
    this->s = nullptr;
    delete this->tree;
    this->tree = nullptr;
}

int Analyzer::SkipSpaces() {
    if(s == nullptr) {
        std::cerr << "Analyzed string is nullptr!" << std::endl;
        return -1;
    }
    while(*s == ' ')
        s++;
    return 0;
}

// Analyze expression, that must contain last symbol #
Node* Analyzer::GetG() {
    if(this->SkipSpaces() == -1)
        return nullptr;
    Node *val = GetE(); //Get x+y or x-y
    if(this->SkipSpaces() == -1)
        return nullptr;
    if(val != nullptr && (*s != '\n') && (*s != '\0'))
        std::cerr << "Syntax error: the expression was expected to end, but a \'" << *s << "\' was found!" << std::endl;
    return val;
}

//Analyze sub-expression that contain addition or subtractin
Node *Analyzer::GetE() {
    if(this->SkipSpaces() == -1)
        return nullptr;
    Node* val = GetT(); // Get x*y or x/y // May be NULL in case of unary operator '-'
    Node* total;

    while(*s == '+' || *s == '-') { // Get second, third, ... terms
        char oper = *s;
        s++;
        Node* val2 = GetT(); // Get x*y or x/y

        if(val2 == nullptr) {
            std::cerr << "Syntax error: expected correct expression after /'" << oper << "/'!" << std::endl;
            if(val != nullptr) delete val;
            return nullptr;
        }

        if(oper == '-' && val == nullptr)
            total = new Node(OP, oper, val2);
        else if(val != nullptr)
            total = new Node(OP, oper, val, val2);
        else {
            std::cerr << "Syntax error: expected correct expression before /'+/'!" << std::endl;
            if(val != nullptr) delete val;
            return nullptr;
        }

        val = total; // Collect result
    }

    if(this->SkipSpaces() == -1)
        return nullptr;
    return val;
}

//Analyze sub-expression that contain multiplication or division
Node *Analyzer::GetT() {
    if(this->SkipSpaces() == -1)
        return nullptr;
    Node* val = GetPow(); // Get x^y
    Node* total;


    while(*s == '*' || *s == '/') {
        if(val == nullptr) {
            std::cerr << "Syntax error: expected correct expression before \'"
                      << *s << "\'!" << std::endl;
            return nullptr;
        }
        char oper = *s++;

        Node* val2 = GetPow(); //Get x^y

        if(val2 == nullptr) {
            std::cerr << "Syntax error: expected correct expression before \'"
                      << *s << "\'!" << std::endl;
            if(val != nullptr) delete val;
            return nullptr;
        }

        total = new Node(OP, oper, val, val2);
        val   = total;
    }
    if(this->SkipSpaces() == -1)
        return nullptr;
    return val;
}

//Analyze sub-expression that contain power
Node *Analyzer::GetPow() {
    if(this->SkipSpaces() == -1)
        return nullptr;
    Node* val = GetP(); // Get (x)
    Node* total;

    while(*s == '^') {
        if(val == nullptr) {
            std::cerr << "Syntax error, expected correct expression before \'^\'!" << std::endl;
            return nullptr;
        }
        s++;

        Node* val2 = GetP(); // Get (x)
        if(val2 == nullptr) {
            std::cerr << "Syntax error, expected correct expression after \'^\'!" << std::endl;
            delete val;
            val = nullptr;
            return nullptr;
        }

        total = new Node(OP, '^', val, val2);
        val = total;
    }

    if(this->SkipSpaces() == -1)
        return nullptr;
    return val;
}

//Analyze sub-expression that contain braces (x)
Node* Analyzer::GetP() {
    if(this->SkipSpaces() == -1)
        return nullptr;
    if(*s == '(') {
        s++;
        Node* val = GetE(); // Get x+y or x-y

        if(val == nullptr) { //expresion
            std::cerr << "Syntax error, expected expresion after \'(\'!" << std::endl;
            return nullptr;
        }

        if(*s++ != ')')
            std::cerr << "Syntax error, expected \')\'!" << std::endl;
        return val;
    }
    else
        return GetVN(); //Get sin/cos, ln, var or const (or const * var)
}

//Analyze sub-expression that MUST contain braces (x)
Node* Analyzer::GetSP(const char* func_name) {
    if(this->SkipSpaces() == -1)
        return nullptr;

    if(*s++ != '(') {
        std::cerr << "Syntax error, expected \'(\' after "
                  << func_name << "!" << std::endl;
        return nullptr;
    }

    Node* val = GetE(); // Get x+y or x-y

    if(val == nullptr) { //expresion
        std::cerr << "Syntax error, expected expresion after \""
                  << func_name << "(\" !" << std::endl;
        return nullptr;
    }

    if(*s++ != ')') {
        std::cerr << "Syntax error, expected \')\' after argument of "
                  << func_name << "!" << std::endl;
        delete val;
        val = nullptr;
    }

    if(this->SkipSpaces() == -1)
        return nullptr;
    return val;
}

//Analyze sub-expression that contain sin/cos, variable or constant (or const * var)
Node* Analyzer::GetVN() {
    if(this->SkipSpaces() == -1)
        return nullptr;
    Node *val1  = nullptr,
          *val2 = nullptr;
    if((val1 = GetSin()) != nullptr) // Get sin(x)
        return val1;

    if((val1 = GetCos()) != nullptr) // Get cos(x)
        return val1;

    if((val1 = GetL()) != nullptr)  // Get ln(x)
        return val1;

    if((val1 = GetN()) != nullptr) {     // Get const
        if((val2 = GetV()) != nullptr) { // Get var
            Node* total;
            total = new Node(OP, '*', val1, val2); // It means, that 5x -> 5 * x
            if(this->SkipSpaces() == -1)
                return nullptr;
            return total;
        }
        else {
            if(this->SkipSpaces() == -1)
                return nullptr;
            return val1;
        }
    }
    else
        return GetV(); //Get var
}

Node* Analyzer::GetSin() {
    if(this->SkipSpaces() == -1)
        return nullptr;
    Node* val;
    if(strncmp(s, "sin", 3) == 0)
        s += 3;
    else return nullptr;

    val = GetSP("sin"); //Get (...) or (...)
    if(val == nullptr)
        return nullptr;

    Node* val2 = new Node(OP, 's', val);
    if(this->SkipSpaces() == -1)
        return nullptr;
    return val2;
}

Node* Analyzer::GetCos() {
    if(this->SkipSpaces() == -1)
        return nullptr;
    Node* val;
    if(strncmp(s, "cos", 3) == 0)
        s += 3;
    else
        return nullptr;

    val = GetSP("cos"); //Get (...) or (...)
    if(val == nullptr) {
        s -= 3;
        return nullptr;
    }

    Node* val2 = new Node(OP, 'c', val);
    if(this->SkipSpaces() == -1)
        return nullptr;
    return val2;
}

//Analyze sub-expression that contain sin() or cos() function
//Node* Analyzer::GetSC() {
//    Node* val;
//    char id;
//    if(strncmp(s, "sin", 3) == 0) {
//        s += 3;
//        id = 's';
//    }
//    else if(strncmp(s, "cos", 3) == 0) {
//        s += 3;
//        id = 'c';
//    }
//    else return NULL;
//    //MyAssert(*s == '(');
//    //s++;
//    if(*s++ != '(') {
//        if(id == 's')
//            fprintf(stderr, "Syntax error, expected /'(/' after sin!\n");
//        else
//            fprintf(stderr, "Syntax error, expected /'(/' after cos!\n");
//    }

//    val = GetE(); //Get x+y or x-y
//    if(val == NULL) {
//        if(id == 's')
//            fprintf(stderr, "Syntax error, expected correct expression after \"sin(\"!\n");
//        else
//            fprintf(stderr, "Syntax error, expected correct expression after \"cos(\"!\n");

//    }

//    if(*s++ != ')') {
//        if(id == 's')
//            fprintf(stderr, "Syntax error, expected /')/' after sin!\n");
//        else
//            fprintf(stderr, "Syntax error, expected /')/' after cos!\n");
//    }
//    Node* val2 = new Node(OP, id, val);
//    return val2;
//}

// Analyze sub-expression, that contatin ln() function
Node *Analyzer::GetL() {
    if(this->SkipSpaces() == -1)
        return nullptr;
    Node* val;
    if(strncmp("ln", s, 2) == 0) {
        s += 2;
    }
    else
        return nullptr;

    val = GetSP("ln");

    if(val == nullptr) {
        s -= 2;
        return nullptr;
    }

    Node* val2 = new Node(OP, 'l', val);
    if(this->SkipSpaces() == -1)
        return nullptr;
    return val2;
}

// Analyze sub-expression, that contain constant value
Node* Analyzer::GetN() {
    if(this->SkipSpaces() == -1)
        return nullptr;
    nod_val val   = 0;
    char   *start = s;

    // Start parsing
    while(*s >= '0' && *s <= '9') {
        val = val * 10 + (*(s++) - '0');
    }

    nod_val ord = 1e-1;
    if(*s == '.') {
        s++;
        while(*s >= '0' && *s <= '9') {
            val = val + (*(s++) - '0') * ord;
            ord /= 10;
        }
    }

    if(start == s) return nullptr;

    Node* res = new Node(NUM, val);
    if(this->SkipSpaces() == -1)
        return nullptr;
    return res;
}

//Analyze sub-expression that contain variable (var = x)
Node* Analyzer::GetV() {
    if(this->SkipSpaces() == -1)
        return nullptr;
    char *start = s;
    if(this->varstr == "") {
        while((*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z') || (*s == '_')) {
            this->varstr += *s++;
            //s++;
        }
    }
    else {
        int i = 0;
        while((*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z') || (*s == '_')) {
            if(this->varstr[i] != *s) {
                std::cerr << "Analyzer works only with 1 variable!" << std::endl;
                return nullptr;
            }
            i++;
            s++;
        }
    }

    if(start == s)
        return nullptr;
    else {
        Node* n = new Node(VAR);
        if(this->SkipSpaces() == -1)
            return nullptr;
        return n;
    }
}

