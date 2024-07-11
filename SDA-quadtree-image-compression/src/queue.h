/* Lache Alexandra Florentina Georgiana, 311CD */
#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

typedef struct queueNode {
    QuadTree treeNode;
    struct queueNode *next;
} QueueNode;

typedef struct queue {
    QueueNode *first;
    QueueNode *last;
    int size;
} *Queue;

Queue init_Queue(void);
QueueNode *init_QueueNode(QuadTree treeNode);
void enqueue(Queue q, QuadTree treeNode);
QuadTree dequeue (Queue q);

#endif
