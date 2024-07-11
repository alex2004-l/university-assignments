/* Lache Alexandra Florentina Georgiana, 311CD */

#ifndef QTREE_H
#define QTREE_H

#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

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
