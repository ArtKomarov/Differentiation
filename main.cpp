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

int main(int argc, char* argv[]) {
    //puts("To create a tree graph, enter the file name (.dot extension) as the first command line parameter.");
    puts("To save the graph-tree with a picture, enter the file name (.png expression) as the first parametr of command line.");
    puts("To save a differentiated tree graph with a picture, enter the file name (extension .png) with the second command line parameter.");
    puts("Enter a calculated expression without spaces");
    char input[1024];
    if(fgets(input, 1024, stdin) == NULL) {
        fprintf(stderr, "Expression not entered! (fgets failed)\n");
        return -1;
    }
    Analyzer *a = new Analyzer(input);
    if(argc > 1 && a->tree != NULL) {

        a->tree->DiffTex(argv[3]);
        sprintf(input, "pdflatex %s", argv[3]);
        if(system(input) == -1) {
            if(argv[3] == NULL)
                fprintf(stderr, "system: null argument!\n");
            else
                fprintf(stderr, "Impossible to create picture %s, no access to shell,"
                                " or impossible to create child process! (Command \"system\" failed) \n", argv[3]);
        }

        nod_val var = 0;
        puts("Enter value of variable:");
        if(scanf("%lf", &var) != 1) {
            fputs("Value of variable did't enter!\n", stderr);
        }
        else
            printf("Ответ: %lf\n", a->tree->TreeCount(var));
    }
    delete a;
    a = NULL;
    //printf("Ответ: %d\n", a->tree->TreeCount());
    return 0;
}
