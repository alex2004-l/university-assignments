/* LACHE Alexandra Florentina Georgiana, 311 CD */

#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// structurile pentru coada de instructiuni si un nod al acesteia
typedef struct QueueNode {
    char command[21];
    char argument; // camp in care se retine argumentul pentru anumite comenzi
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    struct QueueNode *front;
    struct QueueNode *rear;
    int size;
} *Queue;

// semnaturile functiilor pentru prelucrarea cozii
Queue init_queue(void);
QueueNode *createQueueNode (char command[], char argument);

void enqueue(Queue q, char command[], char argument);
QueueNode *dequeue(Queue q);

void destroyQueue(Queue q);

#endif