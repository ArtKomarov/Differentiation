#include <iostream>
#include <string>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "node.h"
#include "diffconsts.h"
#include "analyzer.h"

//#include "gtest/gtest.h"

using namespace std;

const char SupDotFile[] = "Sup.dot";

void SystemError(char arg[]) ;

//x^3+cos(x+x^2) + ln(sin(x + 5) * cos(x^5)) + 10 + 23 / (x - x^7) / 50 * cos(x^6+x^5+x^4 + x^3 + x^2 + x + 1) + x^3+cos(x+x^2) + ln(sin(x + 5) * cos(x^5)) + 10 + 23 / (x - x^7) + x^7+x^9
int main(int argc, char* argv[]) {
    //puts("To create a tree graph, enter the file name (.dot extension) as the first command line parameter.");
    puts("To differentiate, enter the support file name (.tex expression) as the first parametr of command line.");
    puts("To save the graph-tree with a picture, enter the file name (.png expression) as the second parametr of command line.");
    puts("To save a differentiated tree graph with a picture, enter the file name (extension .png) with the third command line parameter.");
    puts("In all cases the value of the expression will be calculated");
    puts("Enter expression:");
    char input[1024];
    if(fgets(input, 1024, stdin) == NULL) {
        fprintf(stderr, "Expression not entered! (fgets failed)\n");
        return -1;
    }
    Analyzer *a = new Analyzer(input);
    if(a->tree != nullptr) {
        if(argc > 1) {
            if(a->tree->DiffTex(argv[1]) != -1)
                sprintf(input, "pdflatex %s", argv[1]);

            if(system(input) == -1) {
                SystemError(argv[1]);
            }

            if(argc > 2) {
                a->tree->MakeGraphFile(SupDotFile);
                sprintf(input, "dot %s -Tpng -o %s", SupDotFile, argv[2]);
                if(system(input) == -1) {
                    SystemError(argv[2]);
                }

                if(argc > 3) {
                    Node* n = a->tree->Diff();
                    n->Optimization();
                    n->MakeGraphFile(SupDotFile);
                    sprintf(input, "dot %s -Tpng -o %s", SupDotFile, argv[3]);
                    delete n;
                }
            }
        }
        nod_val var = 0;
        if(a->tree->IsVar()) {
            puts("Enter value of variable:");
            if(scanf("%lf", &var) != 1) {
                std::cerr << "Value of variable did't enter! (Set it to zero)" << std::endl;
            }
        }
        std::cout << "Ответ: " << a->tree->TreeCount(var) << std::endl;
    }
    delete a;
    return 0;
}

void SystemError(char arg[]) {
    if(arg == nullptr)
        std::cerr << "system: null argument!" << std::endl;
    else
        std::cerr << "Impossible to create picture %s, no access to shell,"
                     " or impossible to create child process! (Command \"system\" failed) \n" << std::endl;
}
