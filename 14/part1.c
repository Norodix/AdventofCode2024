#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define WIDTH (101)
#define HEIGHT (103)
#define STEPS (100)

void print_map(int tiles[WIDTH][HEIGHT], int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            printf("%i", tiles[x][y]);
        }
        printf("\n");
    }
    printf("\n");
}

int count_quadrants(int tiles[WIDTH][HEIGHT], int width, int height) {
    // Top/bottom left/right
    int tl=0, tr=0, bl=0, br=0;
    for (int y = 0; y < height/2; y++) {
        for (int x = 0; x < width/2; x++) {
            tl += tiles[x][y];
        }
    }
    for (int y = 0; y < height/2; y++) {
        for (int x = width/2+1; x < width; x++) {
            tr += tiles[x][y];
        }
    }
    for (int y = height/2+1; y < height; y++) {
        for (int x = 0; x < width/2; x++) {
            bl += tiles[x][y];
        }
    }
    for (int y = height/2+1; y < height; y++) {
        for (int x = width/2+1; x < width; x++) {
            br += tiles[x][y];
        }
    }
    printf("%i %i %i %i\n", tl, tr, bl, br);

    return tl*tr*bl*br;
}

int main(int argc, char** argv) {
    int w = WIDTH;
    int h = HEIGHT;
    int tiles[WIDTH][HEIGHT];
    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }
    if (strcmp(argv[1], "example") == 0) {
        // Special case for the example input
        w = 11;
        h = 7;
    }
    memset(tiles, 0U, WIDTH*HEIGHT*sizeof(int));

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }

    int px, py, vx, vy;
    while(!feof(f)) {
        // This modulo exponentiation will not work for extremely large numbers
        fscanf(f, "p=%i,%i v=%i,%i\n", &px, &py, &vx, &vy);
        // printf("p=%i,%i v=%i,%i\n", px, py, vx, vy);
        int endx = STEPS * vx + px;
        endx = (((endx % w) + w) % w);
        int endy = STEPS * vy + py;
        endy = (((endy % h) + h) % h);
        tiles[endx][endy]++;
        // printf("End up at (%i,%i)\n", endx, endy);
    }

    print_map(tiles, w, h);

    printf("The quadrant sum product is %i\n", count_quadrants(tiles, w, h));

   return 0;
}
