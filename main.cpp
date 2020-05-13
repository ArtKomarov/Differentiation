#include <iostream>
#include <string>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "node.h"
#include "diffconsts.h"
#include "analyzer.h"

//#include "gtest/gtest.h"

/*double square(double x) {
    return sqrt(x);
}

TEST (SomethingTest, PositiveNos) {
    EXPECT_EQ(2, square(4));
}*/

using namespace std;

const char SupDotFile[] = "Sup.dot";

int main(int argc, char* argv[]) {
    //puts("To create a tree graph, enter the file name (.dot extension) as the first command line parameter.");
    puts("To save the graph-tree with a picture, enter the file name (.png expression) as the first parametr of command line.");
    puts("To save a differentiated tree graph with a picture, enter the file name (extension .png) with the second command line parameter.");
    puts("Enter a calculated expression, without spaces, with a trailing character #");

    std::cout << static_cast<double>('+') << std::endl;

    char input[1024];
    if(fgets(input, 1024, stdin) == NULL) {
        fprintf(stderr, "Expression not entered! (fgets failed)\n");
        return -1;
    }
    Analyzer *a = new Analyzer(input);
    if(argc > 1 && a->tree != NULL) {
        a->tree->MakeGraphFile(SupDotFile);
        sprintf(input, "dot %s -Tpng -o %s", SupDotFile, argv[1]);
        if(system(input) == -1) {
            if(argv[1] == NULL)
                fprintf(stderr, "system: null argument!\n");
            else
                fprintf(stderr, "Impossible to create picture %s, no access to shell, or impossible to create child process! (Command \"system\" failed) \n", argv[1]);
        }

        if(argc > 2) {
            Node* n = a->tree->Diff();
            n->Optimization();
            n->MakeGraphFile(SupDotFile);
            sprintf(input, "dot %s -Tpng -o %s", SupDotFile, argv[2]);
            if(system(input) == -1) {
                if(argv[2] == NULL)
                    fprintf(stderr, "system: null argument!\n");
                else
                    fprintf(stderr, "Impossible to create picture %s, no access to shell, or impossible to create child process! (Command \"system\" failed) \n", argv[2]);
            }
            delete n;
            n = NULL;
        }
        nod_val var;
        puts("Enter value of variable:");
        if(scanf("%d", &var) != 1) {
            fputs("Value of variable did't enter!\n", stderr);
        }
        else
            printf("Ответ: %d\n", a->tree->TreeCount(var));
    }
    delete a;
    a = NULL;
    //printf("Ответ: %d\n", a->tree->TreeCount());
    return 0;
}
