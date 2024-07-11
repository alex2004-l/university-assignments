/* LACHE Alexandra Florentina Georgiana, 311 CD */

#include "CommandQueue.h"

/* functie care initializeaza o coada cu verificare */
Queue init_queue (void) {
    Queue newQueue = malloc(sizeof(struct Queue));
    if (newQueue != NULL) {
        newQueue->front = newQueue->rear = NULL;
        newQueue->size = 0;
    } else
        fprintf(stderr, "ERROR: Cannot allocate memory for queue\n");
    return newQueue;
}

/* functie care creeaza un nod de coada cu verificare */
QueueNode *createQueueNode (char command[], char argument) {
    QueueNode *newQueueNode = malloc(sizeof(struct QueueNode));
    if (newQueueNode != NULL) {
        strcpy(newQueueNode->command, command);
        newQueueNode->argument = argument;
        newQueueNode->next = NULL;
    } else
        fprintf(stderr, "ERROR: Cannot allocate memory for queue node\n");
    return newQueueNode;
}

/* functie care adauga un nod nou in coada */
void enqueue (Queue q, char command[], char argument) {
    QueueNode *newQueueNode = createQueueNode(command, argument);
    // se verifica daca s-a alocat memorie pentru nod
    if (newQueueNode != NULL) { 
        // cazul in care coada initiala este vida
        if (q == NULL) {
            q = init_queue();
            // se verifica daca s-a alocat memorie
            if (q != NULL) { 
            q->front = q->rear = newQueueNode;
            q->size = 1;
            } else {
                /* daca nu s-a putut aloca memorie pentru coada
                se dealoca memoria pentru nod */
                free(newQueueNode);
            }
            return;
        }
        // cazul in care coada initiala este goala
        if (q->front == NULL || q->size == 0) {
            q->front = q->rear = newQueueNode;
            q->size = 1;
            return;
        }
        q->rear->next = newQueueNode;
        q->rear = newQueueNode;
        q->size = q->size + 1;
    }
}

/* functie care elimina primul nod din coada si il returneaza */
QueueNode *dequeue (Queue q) {
    QueueNode *result;
    if (q == NULL || q->front == NULL || q->size == 0)
        return NULL; // nu se poate extrage un element din coada
    result = q->front;
    q->front = q->front->next;
    q->size = q->size - 1;
    // cazul in care coada avea un singur nod
    if (q->size == 0)
        q->rear = NULL;
    return result;
}

/* functie care dealoca toata memoria folosita pentru o coada */
void destroyQueue(Queue q){
	QueueNode *iter, *temp;

    // se verifica daca coada este vida
    if (q == NULL) {
        return;
    }

	iter = q->front;
	while (iter != NULL) {
		temp = iter;
		iter = iter->next;
		free(temp);
	}
	free(q);
}
