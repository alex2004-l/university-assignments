/* LACHE Alexandra Florentina Georgiana, 311 CD */

#include "magicStrip.h"

/* functie de initializare a santinelei cu verificare */
magicStripNode *init_sentinel(void) {
    magicStripNode *sentinel = calloc(1, sizeof(struct magicStripNode));
    if (sentinel == NULL)
        fprintf(stderr, "ERROR: Cannot allocate memory for sentinel\n");
    return sentinel;
}

/* functie de creare a unui nod din banda cu verificare */
magicStripNode *create_node(void) {
    magicStripNode *newNode = malloc(sizeof(struct magicStripNode));
    if (newNode != NULL) {
        newNode->character = '#';
        newNode->next = newNode->prev = NULL;
    } else
        fprintf(stderr, "ERROR: Cannot allocate memory for strip node\n");
    return newNode;
}

/* functie de initializare a benzii cu verificare */
magicStrip init_strip(void) {
    magicStrip strip = malloc(sizeof(struct magicStrip));
    if (strip != NULL) {
        strip->first = init_sentinel();
        strip->last = create_node();
        if (strip->first != NULL) {
            strip->first->next = strip->last;
        }
        if (strip->last != NULL) {
            strip->last->prev = strip->first;
        }
        strip->actual = strip->last;
    } else
        fprintf(stderr, "ERROR: Cannot allocate memory for strip\n");
    return strip;
}

/* functie care adauga un nod la finalul benzii */
void addNodeRear(magicStrip strip) {
    if (strip != NULL) {
        magicStripNode *newNode = create_node();
        if (newNode != NULL) {
            newNode->prev = strip->last;
            strip->last->next = newNode;
            strip->last = newNode;
        }
    }
}

/* functie care deplaseaza degetul cu o pozitie la stanga */
void moveLeft(magicStrip strip) {
    if (strip != NULL && strip->actual->prev != strip->first) {
        strip->actual = strip->actual->prev;
    }
}

/* functie care deplaseaza degetul cu o pozitie la dreapta */
void moveRight(magicStrip strip) {
    if (strip != NULL) {
        if (strip->actual == strip->last) {
            // daca se ajunge la finalul benzii se creeaza un nod nou
            addNodeRear(strip);
            strip->actual = strip->last;
        } else {
            strip->actual = strip->actual->next;
        }
    }
}

/* functie care deplaseaza pozitia degetului la stanga pana
la prima aparitie a caracterului character */
void moveLeftChar(FILE *file, magicStrip strip, char character) {
    int charFound = 0;
    if (strip != NULL && strip->actual != NULL) {
        magicStripNode *iter = strip->actual;
        // verificare caracter pozitie actuala
        if (character == iter->character) {
            return;
        }
        do {
            iter = iter->prev;
            if (character == iter->character) {
                strip->actual = iter;
                charFound = 1;
            }
        } while (!charFound && iter != strip->first);
        // daca nu s-a gasit caracterul
        if (charFound == 0) fprintf(file,"ERROR\n");
    }
}

/* functie care deplaseaza pozitia degetului la dreapta 
pana la prima aparitie a caracterului character */
void moveRightChar(magicStrip strip, char character) {
    int charFound = 0;
    if (strip != NULL && strip->actual != NULL) {
        magicStripNode *iter = strip->actual;
        // verificare caracter pozitie actuala
        if (character == iter->character) {
            return;
        }
        while (!charFound && iter != strip->last) {
            iter = iter->next;
            if (character == iter->character) {
                strip->actual = iter;
                charFound = 1;
            }
        }
        // daca nu s-a gasit caracterul
        if (charFound == 0) {
            addNodeRear(strip);
            strip->actual = strip->last;
        }
    }
}

/* functie care suprascrie valoarea de la pozitia actuala */
void write(magicStrip strip, char c) {
    if (strip != NULL && strip->actual != NULL)
        strip->actual->character = c;
}

/* functie care insereaza la stanga degetului un nod cu simbolul character*/
void insertLeft(FILE *file, magicStrip strip, char character) {
    if (strip != NULL && strip->actual != NULL && strip->first != NULL) {

        // se verifica daca nodul actual nu este primul nod din lista
        if (strip->actual->prev == strip->first) {
            fprintf(file, "ERROR\n"); // mesaj de eroare
            return;
        }
        magicStripNode *newNode = create_node();
        if (newNode != NULL) {
            newNode->character = character;
            newNode->prev = strip->actual->prev;
            newNode->next = strip->actual;
            strip->actual->prev->next = newNode;
            strip->actual->prev = newNode;
            strip->actual = newNode;
        }
    }
}

/* functie care insereaza la dreapta degetului un nod cu simbolul character*/
void insertRight (magicStrip strip, char character) {
    if (strip != NULL && strip->actual != NULL && strip->last != NULL) {

        // se verifica daca nodul actual nu este ultimul nod din lista
        if (strip->last == strip->actual) {
            // se creeaza un nou nod cu simbolul character
            addNodeRear(strip);
            strip->last->character = character;
            strip->actual = strip->last;
            return;
        }
        magicStripNode *newNode = create_node();
        if (newNode != NULL) {
            newNode->character = character;
            newNode->prev = strip->actual;
            newNode->next = strip->actual->next;
            strip->actual->next->prev = newNode;
            strip->actual->next = newNode;
            strip->actual = newNode;
        }
    }
}

/* functie care arata simbolul de la pozitia actuala a degetului */
void showCurrent (FILE *file, magicStrip strip) {
    if (strip != NULL) {
        fprintf(file, "%c\n", strip->actual->character );
    }
}

/* functie care arata intreg continutul benzii */
void show (FILE *file, magicStrip strip) {
    if (strip != NULL && strip->first != NULL) {
        magicStripNode *iter = strip->first->next;
        while (iter != NULL) {
            if (iter == strip->actual){
                fprintf(file, "|%c|", iter->character);
            } else {
                fprintf(file, "%c", iter->character);
            }
            iter = iter->next;
        }
        fprintf(file, "\n");
    }
}

/* functie care dealoca toata memoria folosita pentru o banda */
void freeMagicStrip (magicStrip strip) {
    if (strip == NULL) // se verifica ca banda sa nu fie vida
        return;
    magicStripNode *iter, *temp;
    iter = strip->first;
    while (iter != NULL) {
        temp = iter;
        iter = iter->next;
        free(temp);
    }
    free(strip);
}
