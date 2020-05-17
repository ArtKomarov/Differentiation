#ifndef DIFFCONSTS_H
#define DIFFCONSTS_H

// Type for node value
typedef double nod_val;

// Buffer size for TreeConstruct (optional part)
#define MAX_NAME_LEN 256

// Possible node types
enum {
    NUM = 1,
    VAR = 2,
    OP  = 3
};


#endif // DIFFCONSTS_H
