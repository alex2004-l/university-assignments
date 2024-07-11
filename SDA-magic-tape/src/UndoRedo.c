/* LACHE Alexandra Florentina Georgiana, 311 CD */

#include "UndoRedo.h"

/* functie care initializeaza o stiva noua cu verificare */
Stack init_stack (void) {
    Stack newStack = malloc(sizeof(struct Stack));
    if (newStack != NULL) {
        newStack->head = NULL;
        newStack->size = 0;
    } else
        fprintf(stderr, "ERROR: Cannot allocate memory for stack\n");
    return newStack;
}

/* functie care creeaza un nod nou de stiva cu verificare */
StackNode createStackNode(magicStripNode *node) {
    StackNode newStackNode = malloc(sizeof(struct StackNode));
    if (newStackNode != NULL) {
        newStackNode->node = node;
        newStackNode->next = NULL;
    } else
        fprintf(stderr, "ERROR: Cannot allocate memory for stack node\n");
    return newStackNode;
}

/* functie care adauga un nod nou in stiva */
Stack push(Stack Stack, magicStripNode* node) {
    StackNode newNode = createStackNode(node);

    // se verifica daca s-a alocat memorie pentru nod
    if (newNode != NULL) {
        // cazul in care stiva initiala este vida
        if (Stack == NULL) {  
            Stack = init_stack();
            // se verifica daca s-a putut aloca memorie
            if (Stack != NULL) { 
                Stack->head = newNode;
                Stack->size = 1;
            } else {
                /* daca nu s-a putut aloca memorie pentru stiva
                se dealoca memoria pentru nod */
                free(newNode); 
            }
            return Stack;
        }
        newNode->next = Stack->head;
        Stack->head = newNode;
        Stack->size = Stack->size + 1;
    }
    return Stack;
}

/* functie care elimina primul nod din stiva si returneaza continutul acestuia */
magicStripNode* pop(Stack stack) {
    if (stack == NULL || stack->head == NULL || stack->size == 0) {
        return NULL; // nu se poate extrage un element din stiva
    }
    StackNode tmp = stack->head;
    magicStripNode *result = tmp->node;
    stack->head = stack->head->next;
    stack->size = stack->size - 1;
    free(tmp);
    return result;
}

/* functie care dealoca toata memoria folosita pentru o stiva */
void destroyStack (Stack Stack) {
    if (Stack == NULL ) // se verifica daca stiva nu este vida
        return; 
    StackNode iter = Stack->head, temp;
    while (iter != NULL) {
        temp = iter;
        iter = iter->next;
        free(temp);
    }
    free(Stack);
}