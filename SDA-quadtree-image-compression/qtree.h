/* Lache Alexandra Florentina Georgiana, 311CD */

#ifndef QTREE_H
#define QTREE_H

#include <stdio.h>
#include <stdlib.h>

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
    unsigned char NodeType; // 0 sau 1 daca nodul este intern sau terminal
    pixel pixel; // pentru nodurile terminale
    struct quadtree **kids; // referinte catre copii; NULL pentru frunze
} QuadTreeNode, *QuadTree;

int convert_string_to_number(const char string[]);
int max_four(int a, int b, int c, int d);
int min_four(int a, int b, int c, int d);

QuadTree createQTreeIntNode(void);
QuadTree createQTreeExtNode(pixel pixel);
QuadTree get_compresion_tree(int factor, int size, int x, int y, pixel **matrix);
QuadTree compression_tree_from_file (FILE *fp);
int QTree_height (QuadTree tree);
int QTree_min_path(QuadTree tree);
int get_leaf_count(QuadTree tree);
void destroy_QTree(QuadTree tree);

unsigned int get_red_score(int x, int y, int size, pixel **matrix);
unsigned int get_green_score(int x, int y, int size, pixel **matrix);
unsigned int get_blue_score(int x, int y, int size, pixel **matrix);
unsigned long long get_mean_score(int x, int y, int size, pixel **matrix);
void init_img(PPMimage *img, unsigned int size);
void read_image(FILE *input_file, PPMimage *img);
void decompress_img(PPMimage *img, QuadTree tree, int x, int y, int size);
void write_compressed_img(FILE *fp, unsigned int img, QuadTree tree);
void write_img (FILE *fp, PPMimage img);
void free_img (PPMimage *img);

#endif