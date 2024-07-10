/* Lache Alexandra Florentina Georgiana, 311CD */


unsigned int get_red_mean_score(int x, int y, int size, pixel **matrix) {
    unsigned long long sum = 0;
    for (int i = x; i < x + size; ++i) {
        for (int j = y; j < y + size; ++j) {
            sum += (unsigned int)matrix[i][j].r;
        }
    }
    sum /= (size * size);
    return (unsigned int) sum;
}

unsigned int get_green_mean_score(int x, int y, int size, pixel **matrix) {
    unsigned long long sum = 0;
    for (int i = x; i < x + size; ++i) {
        for (int j = y; j < y + size; ++j) {
            sum += (unsigned int)matrix[i][j].g;
        }
    }
    sum /= (size * size);
    return (unsigned int) sum;
}

unsigned int get_blue_mean_score(int x, int y, int size, pixel **matrix) {
    unsigned long long sum = 0;
    for (int i = x; i < x + size; ++i) {
        for (int j = y; j < y + size; ++j) {
            sum += (unsigned int)matrix[i][j].b;
        }
    }
    sum /= (size * size);
    return (unsigned int) sum;
}

unsigned long long get_mean_score(int x, int y, int size, pixel **matrix) {
    unsigned int red = get_red_mean_score(x, y, size, matrix);
    unsigned int green = get_green_mean_score(x, y, size, matrix);
    unsigned int blue = get_blue_mean_score(x, y, size, matrix);
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

void read_image(FILE *input_file, PPMimage *img) {
    char random;
    fscanf(input_file, "%s", img->file_type);
    fscanf(input_file, "%d%d", &(img->width), &(img->height));
    fscanf(input_file, "%d", &(img->max_range));
    fscanf(input_file, "%c", &random);

    img->matrix = malloc (sizeof(pixel*) * img->width);
    for (int i = 0; i < img->width; ++i) {
        img->matrix[i] = malloc(sizeof(pixel) * img->height);
    }
    for (int i = 0; i < img->width; ++i) {
        for (int j = 0; j < img->height; ++j) {
            fread(&(img->matrix[i][j]), sizeof(unsigned char), 3, input_file);
        }
    }
}

void decompress_img(PPMimage *img, QuadTree tree, int x, int y, int size) {
    if (tree->NodeType == 0) {
        decompress_img(img, tree->kids[0], x, y, size/2);
        decompress_img(img, tree->kids[1], x, y+size/2, size/2);
        decompress_img(img, tree->kids[2], x+size/2, y+size/2, size/2);
        decompress_img(img, tree->kids[3], x+size/2, y, size/2);
    } else {
        for (int i = x; i < x + size; ++i) {
            for (int j = y; j < y + size; ++j) {
                img->matrix[i][j] = tree->pixel;
            }
        }
    }
}