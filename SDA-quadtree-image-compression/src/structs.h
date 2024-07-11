#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct pixel {
    unsigned char r, g, b;
} pixel;

typedef struct image {
    char file_type[20]; //tipul imaginii PPM
    unsigned int width, height; // dimensiuni
    unsigned int max_range;
    pixel **matrix; // matricea de pixeli asociata imaginii
} PPMimage;

typedef struct quadtree {
    unsigned char type; // 0 sau 1 daca nodul este intern sau terminal
    pixel pixel; // pentru nodurile terminale
    struct quadtree **kids; // referinte catre copii; NULL pentru frunze
} QuadTreeNode, *QuadTree;

#endif
