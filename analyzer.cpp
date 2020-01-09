#include <iostream>
#include <string>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "diffconsts.h"
#include "analyzer.h"

                                                           //private
/*Node* Analyzer::OpUnion(Node* nod1, Node* nod2, char op) {
    assert(nod1  != NULL);
    assert(nod2  != NULL);
    Node* total  = new Node(OP, op, nod1, nod2);
    //total->Construct(OP, op, nod1, nod2);
    return total;
}*/

/*Node* Analyzer::OpUnion(Node* nod1, char op) {
    assert(nod1  != NULL);
    Node* total  = new Node(OP, op, nod1);
    //total->Construct(OP, op, nod1);
    return total;
}*/

//void Analyzer::MyAssert(bool b) {
//    if(!b)
//        fprintf(stderr, "Ошибка, ожидалась скобка! (символ \"(\" или \")\", зависит от контекста)\n");
//}

                                                           //public
// Constructor
Analyzer::Analyzer(char* s) {
    assert(s != NULL);
    this->s = strdup(s);
    this->slen = (int) strlen(s);
    this->tree = this->GetG(); // Analyze expression
}

// Destructor
Analyzer::~Analyzer() {
    while(*s != '\0') s++;
    free(this->s - this->slen);
    this->s = NULL;
    delete this->tree;
    this->tree = NULL;
}

// Analyze expression, that must contain last symbol #
Node* Analyzer::GetG() {
    Node *val = GetE(); //Get x+y or x-y
    if(val != NULL && *s != '#')
        fprintf(stderr, "Syntax error: expected last symbol \"#\"!\n");
    return val;
}

// Analyze sub-expression, that contain constant value
Node* Analyzer::GetN() {
    int val = 0;
    char *start = s;
    while(*s >= '0' && *s <= '9') {
        val = val * 10 + (*(s++) - '0');
    }
    if(start == s) return NULL;
    Node* n = new Node(NUM, val);
    return n;
}

//Analyze sub-expression that contain addition or subtractin
Node *Analyzer::GetE() {
    Node* val = GetT(); // Get x*y or x/y // May be NULL in case of '-'
    Node* total;

    while(*s == '+' || *s == '-') {
        char oper = *s;
        s++;
        Node* val2 = GetT(); // Get x*y or x/y

        if(val2 == NULL) {
            fprintf(stderr, "Syntax error: expected correct expression after /'%c/'!\n", oper);
            if(val != NULL) delete val;
            return NULL;
        }

        if(oper == '-' && val == NULL)
            total = new Node(OP, oper, val2);
        else if(val != NULL)
            total = new Node(OP, oper, val, val2);
        else {
            fprintf(stderr, "Syntax error: expected correct expression before /'+/'!\n");
            if(val != NULL) delete val;
            return NULL;
        }

        val = total;
    }
    return val;
}

//Analyze sub-expression that contain multiplication or division
Node *Analyzer::GetT() {
    Node* val = GetPow(); // Get x^y
    Node* total;
    while(*s == '*' || *s == '/') {
        if(val == NULL) {
            fprintf(stderr, "Syntax error: expected correct expression before /'%c/'!\n", *s);
            return NULL;
        }
        char oper = *s++;

        Node* val2 = GetPow(); //Get x^y
        if(val2 == NULL) {
            fprintf(stderr, "Syntax error: expected correct expression before /'%c/'!\n", *s);
            if(val != NULL) delete val;
            return NULL;
        }
        total = new Node(OP, oper, val, val2);
        val = total;
    }
    return val;
}

//Analyze sub-expression that contain braces (x)
Node* Analyzer::GetP() {
    if(*s == '(') {
        s++;
        Node* val = GetE(); // Get x+y or x-y
        if(val == NULL) { //expresion
            fprintf(stderr, "Syntax error, expected expresion after /'(/'!\n");
            return NULL;
        }

        if(*s++ != ')')
            fprintf(stderr, "Syntax error, expected /')/'!\n");
        return val;
    }
    else
        return GetVN(); //Get sin/cos, var or const (or const * var)
}

//Analyze sub-expression that contain variable (var = x)
Node* Analyzer::GetV() {
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
                fprintf(stderr, "Our analyzer works only with 1 variable!\n");
                return NULL;
            }
            i++;
            s++;
        }
    }
    if(start == s) return NULL;
    else {
        Node* n = new Node(VAR);
        return n;
    }
}

//Analyze sub-expression that contain sin/cos, variable or constant (or const * var)
Node* Analyzer::GetVN() {
    Node *val1  = NULL,
          *val2 = NULL;
    if((val1 = GetSC()) != NULL) // Get sin(x) or cos(x)
        return val1;
    if((val1 = GetL()) != NULL) // Get ln(x)
        return val1;
    if((val1 = GetN()) != NULL) { // Get const
        if((val2 = GetV()) != NULL) { // Get var
            Node* total;
            total = new Node(OP, '*', val1, val2);
            return total;
        }
        else {
            return val1;
        }
    }
    else
        return GetV(); //Get var
}

//Analyze sub-expression that contain sin() or cos() function
Node* Analyzer::GetSC() {
    Node* val;
    char id;
    if(strncmp(s, "sin", 3) == 0) {
        s += 3;
        id = 's';
    }
    else if(strncmp(s, "cos", 3) == 0) {
        s += 3;
        id = 'c';
    }
    else return NULL;
    //MyAssert(*s == '(');
    //s++;
    if(*s++ != '(') {
        if(id == 's')
            fprintf(stderr, "Syntax error, expected /'(/' after sin!\n");
        else
            fprintf(stderr, "Syntax error, expected /'(/' after cos!\n");
    }

    val = GetE(); //Get x+y or x-y
    if(val == NULL) {
        if(id == 's')
            fprintf(stderr, "Syntax error, expected correct expression after \"sin(\"!\n");
        else
            fprintf(stderr, "Syntax error, expected correct expression after \"cos(\"!\n");

    }

    if(*s++ != ')') {
        if(id == 's')
            fprintf(stderr, "Syntax error, expected /')/' after sin!\n");
        else
            fprintf(stderr, "Syntax error, expected /')/' after cos!\n");
    }
    Node* val2 = new Node(OP, id, val);
    return val2;
}

//Analyze sub-expression that contain power
Node *Analyzer::GetPow() {
    Node* val = GetP(); // Get (x)
    Node* total;
    while(*s == '^') {
        if(val == NULL) {
            fprintf(stderr, "Syntax error, expected correct expression before ^!\n");
            return NULL;
        }
        s++;

        Node* val2 = GetP(); // Get (x)
        if(val2 == NULL) {
            fprintf(stderr, "Syntax error, expected correct expression after ^!\n");
            delete val;
            val = NULL;
            return NULL;
        }
        total = new Node(OP, '^', val, val2);
        val = total;
    }
    return val;
}

Node *Analyzer::GetL() {
    Node* val;
    if(strncmp("ln", s, 2) == 0) {
        s += 2;
    }
    else
        return NULL;
    if(*s++ != '(')
        fprintf(stderr, "Syntax error, expected /'(/' after ln!\n");

    val = GetE(); //Get x+y or x-y
    if(val == NULL)
        fprintf(stderr, "Syntax error, expected correct expression after \"ln(\"!\n");

    if(*s++ != ')')
        fprintf(stderr, "Syntax error, expected /')/' after ln!\n");
    Node* val2 = new Node(OP, 'l', val);
    return val2;
}
