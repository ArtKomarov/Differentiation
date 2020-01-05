#include <iostream>
#include <string>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "node.h"
#include "diffconsts.h"
#include "analyzer.h"


using namespace std;

int main(int argc, char* argv[]) {
    puts("Для создание графа-дерева введите первым параметром командной строки имя файла (расширение .dot).");
    puts("Для сохранения графа-дерева картинкой введите вторым параметром командной строки имя файла (расширение .png).");
    puts("Для сохранения продиффиринцированного графа-дерева картинкой введите третьтим параметром командной строки имя файла (расширение .png).");
    puts("Введите вычисляемое выражение, без пробелов, с завершающим символом #");
    char input[1024];
    if(fgets(input, 1024, stdin) == NULL) {
        fprintf(stderr, "Выражение не было введено! (fgets failed)\n");
        return -1;
    }
    Analyzer *a = new Analyzer(input);
    if(argc > 1) {
        a->tree->MakeGraphFile(argv[1]);
        if(argc > 2) {
            sprintf(input, "dot %s -Tpng -o %s", argv[1], argv[2]);
            if(system(input) == -1) {
                if(argv[1] == NULL || argv[2] == NULL)
                    fprintf(stderr, "system: null argument!\n");
                else
                    fprintf(stderr, "Невозможно создать картинку %s из %s, нет доступа к shell либо невозможно создать дочерний процесс! (Command \"system\" failed) \n", argv[1], argv[2]);
            }
            if(argc > 3) {
                char s[1024];
                sprintf(s, "Sup.dot");
                a->tree->SaveTree(s);
            }
            //a->tree->Optimization();

            //delete a;
            Node* n = a->tree->Diff();
            n->Optimization();
            n->MakeGraphFile(argv[4]);
            sprintf(input, "dot %s -Tpng -o %s", argv[4], argv[5]);
            system(input);
            delete a;
            delete n;
        }
    }
    //printf("Ответ: %d\n", a.tree->TreeCount());
    return 0;
}
