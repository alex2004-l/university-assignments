/* Lache Alexandra Florentina Georgiana, 311CD */

#ifndef PPMIMG_H
#define PPMIMG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct pixel {
    unsigned char r, g, b;
} pixel;

typedef struct image {
    char file_type[20];
    unsigned int width, height;
    unsigned int max_range;
    pixel **matrix;
} PPMimage;

unsigned int get_red_mean_score(int x, int y, int size, pixel **matrix);
unsigned int get_green_mean_score(int x, int y, int size, pixel **matrix);
unsigned int get_blue_mean_score(int x, int y, int size, pixel **matrix);
unsigned long long get_mean_score(int x, int y, int size, pixel **matrix);
void read_image(FILE *input_file, PPMimage *img);
void decompress_img(PPMimage *img, QuadTree tree, int x, int y, int size);

#endif