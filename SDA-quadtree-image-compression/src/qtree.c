/* Lache Alexandra Florentina Georgiana, 311CD */

#include "qtree.h"
#include "queue.h"


// functie care returneaza media culorii rosu pentru o matrice de pixeli
unsigned int get_red_score(int x, int y, int size, pixel **matrix) {
    unsigned long long sum = 0;
    for (int i = x; i < x + size; ++i) {
        for (int j = y; j < y + size; ++j) {
            sum += (unsigned int)matrix[i][j].r;
        }
    }
    sum /= (size * size);
    return (unsigned int) sum;
}


// functie care returneaza media culorii verde pentru o matrice de pixeli
unsigned int get_green_score(int x, int y, int size, pixel **matrix) {
    unsigned long long sum = 0;
    for (int i = x; i < x + size; ++i) {
        for (int j = y; j < y + size; ++j) {
            sum += (unsigned int)matrix[i][j].g;
        }
    }
    sum /= (size * size);
    return (unsigned int) sum;
}


// functie care returneaza media culorii albastru pentru o matrice de pixeli
unsigned int get_blue_score(int x, int y, int size, pixel **matrix) {
    unsigned long long sum = 0;
    for (int i = x; i < x + size; ++i) {
        for (int j = y; j < y + size; ++j) {
            sum += (unsigned int)matrix[i][j].b;
        }
    }
    sum /= (size * size);
    return (unsigned int) sum;
}


// functie care returneaza scorul mean pentru o matrice de pixeli
unsigned long long get_mean_score(int x, int y, int size, pixel **matrix) {
    unsigned int red = get_red_score(x, y, size, matrix);
    unsigned int green = get_green_score(x, y, size, matrix);
    unsigned int blue = get_blue_score(x, y, size, matrix);

    unsigned long long result = 0;
    for (int i = x; i < x + size; ++i) {
        for (int j = y; j < y + size; ++j) {
            result += (red - matrix[i][j].r) * (red - matrix[i][j].r)
            + (green - matrix[i][j].g) * (green - matrix[i][j].g) 
            + (blue - matrix[i][j].b) * (blue - matrix[i][j].b);
        }
    }

    result /= (3*size*size);
    return result;
}


// functie care initializeaza o imagine PPM P6
void init_img(PPMimage *img, unsigned int size) {
    img->height = img->width = size;
    strcpy(img->file_type, "P6");
    img->max_range = 255;

    // este alocata memorie pentru matricea de pixeli
    img->matrix = malloc (sizeof(pixel *) * size);
    for (int i = 0; i < size; ++i) {
        img->matrix[i] = malloc(sizeof(pixel) * size);
    }
}


// functie care citeste dintr-un fisier de input imaginea PPM
void read_image(FILE *input_file, PPMimage *img) {
    char random;

    // citire informatii despre imagine
    fscanf(input_file, "%s", img->file_type);
    fscanf(input_file, "%d%d", &(img->width), &(img->height));
    fscanf(input_file, "%d", &(img->max_range));
    fscanf(input_file, "%c", &random);

    // alocare memorie matrice de pixeli
    img->matrix = malloc (sizeof(pixel*) * img->width);
    for (int i = 0; i < img->width; ++i) {
        img->matrix[i] = malloc(sizeof(pixel) * img->height);
    }

    // citire imagine propriu-zise
    for (int i = 0; i < img->width; ++i) {
        for (int j = 0; j < img->height; ++j) {
            fread(&(img->matrix[i][j]), sizeof(unsigned char), 3, input_file);
        }
    }
}


// functie care reconstruieste imaginea initiala pe baza arborelui de compresie
void decompress_img(PPMimage *img, QuadTree tree, int x, int y, int size) {
    if (tree->type == 0) {
        //pentru nodurile interioare, functia este apelata recursiv pentru copii
        decompress_img(img, tree->kids[0], x, y, size/2);
        decompress_img(img, tree->kids[1], x, y+size/2, size/2);
        decompress_img(img, tree->kids[2], x+size/2, y+size/2, size/2);
        decompress_img(img, tree->kids[3], x+size/2, y, size/2);
    } else {
        // pentru nodurile exterioare are loc completarea imaginii cu valoarea
        // pixelului pentru zona patrata de dimensiune size care incepe la (x,y)
        for (int i = x; i < x + size; ++i) {
            for (int j = y; j < y + size; ++j) {
                img->matrix[i][j] = tree->pixel;
            }
        }
    }
}


// functie pentru scrierea imaginii comprimate intr-un fisier
void write_compressed_img(FILE *fp, unsigned int size, QuadTree tree) {
    fwrite(&size, sizeof(unsigned int), 1, fp);

    Queue q = init_Queue();
    enqueue(q,tree);

    while (q->size != 0) {
        QuadTree node = dequeue(q);
        if (node->type == 0) {
            enqueue(q, node->kids[0]);
            enqueue(q, node->kids[1]);
            enqueue(q, node->kids[2]);
            enqueue(q, node->kids[3]);
            fwrite(&(node->type), sizeof(unsigned char), 1, fp);
        } else {
            fwrite(&(node->type), sizeof(unsigned char), 1, fp);
            fwrite(&(node->pixel.r), sizeof(unsigned char), 1, fp);
            fwrite(&(node->pixel.b), sizeof(unsigned char), 1, fp);
            fwrite(&(node->pixel.g), sizeof(unsigned char), 1, fp);
        }
    }
    free(q);
}


// functie care scrie intr-un fisier o imagine PPM primita ca argument
void write_img(FILE *fp, PPMimage img) {
    fprintf(fp, "%s\n", img.file_type);
    fprintf(fp, "%d %d\n", img.width, img.height);
    fprintf(fp, "%d\n", img.max_range);
    for (int i = 0; i < img. width; ++i) 
        for (int j = 0; j < img.height; ++j) {
            fwrite(&(img.matrix[i][j]), sizeof(unsigned char), 3, fp);
    }
}


// functie pentru dealocarea memoriei matricei de pixeli pentru o imagine
void free_img(PPMimage *img) {
    for (int i = 0; i < img->width; ++i)
        free(img->matrix[i]);
    free(img->matrix);
}


// functie pentru crearea unui nod interior al arborelui cuaternar
QuadTree createQTreeIntNode(void) {
    QuadTree newQuadTreeNode = malloc(sizeof(struct quadtree));
    newQuadTreeNode->type = 0;
    // alocare de memorie pentru cei 4 copii
    newQuadTreeNode->kids = malloc(4 * sizeof(struct quadtree*));
    for (int i = 0; i < 4; ++i)
        newQuadTreeNode->kids[i] = NULL;

    return newQuadTreeNode;
}


// functie pentru crearea unui nod exterior al arborelui cuaternar
QuadTree createQTreeExtNode(pixel pixel) {
    QuadTree newQuadTreeNode = malloc(sizeof(struct quadtree));
    newQuadTreeNode->type = 1;
    newQuadTreeNode->pixel = pixel;
    // nu se aloca memorie pentru copii
    newQuadTreeNode->kids = NULL;
    return newQuadTreeNode;
}


// functie care returneaza arborele de compresie pentru o imagine
QuadTree get_compresion_tree(int factor, int size, int x, int y, pixel **matrix) {
    unsigned long long mean = get_mean_score(x,y,size,matrix);

    // verifica daca mean scorul este mai mare decat factorul primit
    if (mean > factor) {
        // in caz afirmativ, in arbore este adaugat un nod interior
        // se apeleaza functia recursiv pentru copiii nodului
        // fiecare primind o diviziune a imaginii initiale
        QuadTree tree = createQTreeIntNode();
        tree->kids[0] = get_compresion_tree(factor, size/2, x, y, matrix);
        tree->kids[1] = get_compresion_tree(factor, size/2, x , y + size/2, matrix);
        tree->kids[2] = get_compresion_tree(factor, size/2, x + size/2, y + size/2, matrix);
        tree->kids[3] = get_compresion_tree(factor, size/2, x + size/2 , y, matrix);
        return tree;
    } else {
        // in caz neativ, este adaugat un nod exterior care retine media RGB a zonei
        unsigned int red = get_red_score(x, y, size, matrix);
        unsigned int green = get_green_score(x, y, size, matrix);
        unsigned int blue = get_blue_score(x, y, size, matrix);
        pixel pixel = {red, blue, green};
        return createQTreeExtNode(pixel);
    }
}


// functie pentru aflarea inaltimii arborelui cuaternar
int QTree_height (QuadTree tree) {
    if (tree == NULL || tree->kids == NULL)
        return 0;
    else {
        return 1 + max_four(QTree_height(tree->kids[0]), QTree_height(tree->kids[1]),
        QTree_height(tree->kids[2]), QTree_height(tree->kids[3]));
    }
}


// functie pentru aflarea numarului de frunze al arborelui cuaternar
int get_leaf_count(QuadTree tree) {
    if (tree == NULL)
        return 0;
    if (tree->kids == NULL) 
        return 1;
    else 
        return get_leaf_count(tree->kids[0]) + get_leaf_count(tree->kids[1])
        + get_leaf_count(tree->kids[2]) + get_leaf_count(tree->kids[3]);
}


// functie pentru aflarea celei mai mici distante de la radacina la o frunza
int QTree_min_path(QuadTree tree) {
    if (tree == NULL || tree->kids == NULL)
        return 0;
    else {
        return 1 + min_four(QTree_min_path(tree->kids[0]), QTree_min_path(tree->kids[1]),
        QTree_min_path(tree->kids[2]), QTree_min_path(tree->kids[3]));
    }
}


// functie pentru dealocarea memoriei
void destroy_QTree(QuadTree tree) {
    if (tree == NULL) return;
    if (tree->kids == NULL) free(tree);
    else {
        destroy_QTree(tree->kids[0]);
        destroy_QTree(tree->kids[1]);
        destroy_QTree(tree->kids[2]);
        destroy_QTree(tree->kids[3]);
        free(tree->kids);
        free(tree);
    }
}


// functie care reconstruieste arborele de compresie pe baza imaginii comprimate
QuadTree compression_tree_from_file(FILE *fp) {
    QuadTree tree = NULL;
    Queue q = init_Queue();

    unsigned char c;
    fread(&c, sizeof(unsigned char), 1, fp);

    if (c == 0) {
        tree = createQTreeIntNode();
        enqueue(q, tree);
    } else {
        pixel pixel;
        fread(&pixel, sizeof(unsigned char), 3, fp);
        tree = createQTreeExtNode(pixel);
    }

    while (q->size != 0) {
        QuadTree tmp = dequeue(q);
        for(int i = 0; i < 4; ++i) {
            fread(&c, sizeof(unsigned char), 1, fp);
            if (c == 0) {
                tmp->kids[i] = createQTreeIntNode();
                enqueue(q, tmp->kids[i]);
            } else {
                pixel pixel;
                fread(&pixel, sizeof(unsigned char), 3, fp);
                tmp->kids[i] = createQTreeExtNode(pixel);

            }
        }
    }

    free(q);
    return tree;
}
