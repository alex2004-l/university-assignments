/* LACHE Alexandra Florentina Georgiana, 311 CD */

#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "magicStrip.h"

// structurile pentru stiva si un nod al acesteia
typedef struct StackNode {
    struct magicStripNode *node; // pointer catre adresa unui nod din banda
    struct StackNode *next;
} *StackNode;

typedef struct Stack {
    struct StackNode *head;
    int size;
} *Stack;

// semnaturile functiilor pentru prelucrarea stivei
Stack init_stack (void);
StackNode createStackNode(magicStripNode *node);

Stack push(Stack Stack, magicStripNode* node);
magicStripNode* pop(Stack stack);

void destroyStack (Stack Stack);

#endif