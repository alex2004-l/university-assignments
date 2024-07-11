/* Lache Alexandra Florentina Georgiana, 311CD */

#include "queue.h"

// functie care initializeaza coada
Queue init_Queue(void) {
    Queue newQueue = malloc(sizeof(struct queue));
    newQueue->first = newQueue->last = NULL;
    newQueue->size = 0;
    return newQueue;
}

// functie care initializeaza un nod al cozii
QueueNode *init_QueueNode(QuadTree treeNode) {
    QueueNode *newQNode = malloc(sizeof(QueueNode));
    newQNode->treeNode = treeNode;
    newQNode->next = NULL;
    return newQNode;
}

// functie care adauga un nod la finalul cozii
void enqueue(Queue q, QuadTree treeNode) {
    if (q == NULL)
        q = init_Queue();
    QueueNode *node = init_QueueNode(treeNode);
    if (q->first == NULL) {
        q->first = q->last = node;
        q->size = 1;
    } else {
        q->last->next = node;
        q->last = node;
        q->size += 1;
    }
}

// functie care elimina primul nod din coada
QuadTree dequeue (Queue q) {
    if (q == NULL || q->first == NULL)
        return NULL;
    else {
        QueueNode *tmp = q->first;
        q->first = q->first->next;
        QuadTree result = tmp->treeNode;
        free(tmp);
        q->size --;
        if (q->size == 0) q->last = NULL;
        return result;
    }
}
