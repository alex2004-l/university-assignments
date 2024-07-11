/* LACHE Alexandra Florentina Georgiana, 311 CD */

#ifndef MAGICSTRIP_H
#define MAGICSTRIP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// structurile pentru banda magica si un nod al acesteia
typedef struct magicStripNode {
    char character;
    struct magicStripNode *next;
    struct magicStripNode *prev; 
} magicStripNode;

typedef struct magicStrip {
    struct magicStripNode *first;
    struct magicStripNode *last;
    struct magicStripNode *actual; // pozitia degetului 
} *magicStrip;

// semnaturile functiilor pentru prelucrarea benzii
magicStripNode *init_sentinel(void);
magicStripNode *create_node(void);
magicStrip init_strip(void);

// functiile pentru operatiile de tip UPDATE
void addNodeRear(magicStrip strip);
void moveLeft(magicStrip strip);
void moveRight(magicStrip strip);
void moveLeftChar(FILE *file, magicStrip strip, char c);
void moveRightChar(magicStrip strip, char c);
void write(magicStrip strip, char c);
void insertLeft(FILE *file, magicStrip strip, char c);
void insertRight (magicStrip strip, char c);

// functiile pentru operatiile de tip QUERY
void showCurrent (FILE *file, magicStrip strip);
void show (FILE *file, magicStrip strip);

void freeMagicStrip (magicStrip strip);

#endif