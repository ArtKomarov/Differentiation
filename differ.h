#ifndef DIFFER_H
#define DIFFER_H

#include "analyzer.h"

class Differ {
private:
    Analyzer *an;
public:
    Differ();
    Differ(Analyzer *a);
};

#endif // DIFFER_H
