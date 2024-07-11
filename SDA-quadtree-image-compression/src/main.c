/* Tema 2 - Structuri de Date și Algoritmi - Arbore cuaternar de compresie
LACHE Alexandra Florentina Georgiana, 311 CD, UPB ACS 2022-2023 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "qtree.h"

// funcție care primește ca argumentr 4 numere întregi
// și îl returnează pe cel mai mare
int max_four(int a, int b, int c, int d) {
    a = (b > a)? b : a;
    c = (d > c)? d : c;
    return (a > c)? a : c;
}

// funcție care primește ca argumentr 4 numere întregi
// și îl returnează pe cel mai mic
int min_four(int a, int b, int c, int d) {
    a = (b < a)? b : a;
    c = (d < c)? d : c;
    return (a < c)? a : c;
}

int main(int argc, char const *argv[]) {
    int factor = 0;
    FILE *input_file, *output_file;
    PPMimage img;
    
    // este comparat primul argument cu -c1, -c2 si -d 
    // pentru a identifica cerinta
    if (strcmp(argv[1], "-c1") == 0) {
        factor = atoi(argv[2]);

        input_file = fopen(argv[3], "rb");
        if (input_file == NULL) {
            fprintf(stderr, "ERROR: Cannot open %s!", argv[3]);
            return -1;
        }
        output_file = fopen(argv[4], "wt");
        if (output_file == NULL) {
            fprintf(stderr, "ERROR: Cannot open %s!", argv[4]);
            return -1;
        }

        read_image(input_file, &img);
        QuadTree tree = get_compresion_tree(factor, img.width, 0, 0, img.matrix);
        fprintf(output_file, "%d\n%d\n%d\n", QTree_height(tree) + 1,
            get_leaf_count(tree), img.height>>QTree_min_path(tree));

        destroy_QTree(tree);

    }  else if (strcmp (argv[1], "-c2") == 0 ) {
        factor = atoi(argv[2]);

        input_file = fopen(argv[3], "rb");
        if (input_file == NULL) {
            fprintf(stderr, "ERROR: Cannot open %s!", argv[3]);
            return -1;
        }
        output_file = fopen(argv[4], "wb");
        if (output_file == NULL) {
            fprintf(stderr, "ERROR: Cannot open %s!", argv[4]);
            return -1;
        }

        read_image(input_file, &img);
        QuadTree tree = get_compresion_tree(factor, img.width, 0, 0, img.matrix);
        write_compressed_img(output_file, img.width, tree);

        destroy_QTree(tree);

    } else if (strcmp(argv[1], "-d")==0) {

        input_file = fopen(argv[2], "rb");
        if (input_file == NULL) {
            fprintf(stderr, "ERROR: Cannot open %s!", argv[3]);
            return -1;
        }
        output_file = fopen(argv[3], "wb");
        if (output_file == NULL) {
            fprintf(stderr, "ERROR: Cannot open %s!", argv[4]);
            return -1;
        }

        unsigned int size;
        fread(&size, sizeof(unsigned int), 1, input_file);
        init_img(&img, size);

        QuadTree tree = compression_tree_from_file(input_file);
        decompress_img(&img, tree, 0, 0, img.width);
        write_img(output_file, img);

        destroy_QTree(tree);

    } else fprintf(stderr,"ERROR: Unknown argument!\n");

    free_img(&img);
    fclose(input_file);
    fclose(output_file);

    return 0;
}
