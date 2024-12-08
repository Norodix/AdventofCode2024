#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (1024) // maximum length of lines to be read

static int cols, rows;
#define GETCHAR(x) (x & 0xff)
#define SETNODE(x) (x = x | 0x100)
#define GETNODE(x) (x & 0x100)
#define INDEX(r, c) (cols * r + c)
#define ROW(x) (x / cols)
#define COL(x) (x - ((x / cols) * cols))
#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)



int main(int argc, char** argv) {
    uint16_t* tiles;
    char line[LINELEN];
    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }

    fgets(line, LINELEN, f);
    cols = strlen(line) - 1;
    rows = 0;
    while (!feof(f)) {
        rows++;
        fgets(line, LINELEN, f);
    }
    rewind(f);

    tiles = (uint16_t*) malloc(rows*cols*sizeof(uint16_t));
    memset(tiles, 0U, rows*cols*sizeof(uint16_t));
    fgets(line, LINELEN, f);
    int tiles_index = 0;
    while (!feof(f)) {
        for (int i = 0; i < cols; i++) {
            tiles[tiles_index] = line[i];
            tiles_index++;
        }
        fgets(line, LINELEN, f);
    }

    // for (int r = 0; r < rows; r++) {
    //     for (int c = 0; c < cols; c++) {
    //         char ch = GETCHAR(tiles[INDEX(r,c)]);
    //         printf("%c", ch);
    //     }
    //     printf("\n");
    // }
    // printf("\n");

    for (int i = 0; i < rows*cols; i++) {
        char c1 = GETCHAR(tiles[i]);
        if (c1 == '.') continue; // not interesting
        for (int j = i+1; j < rows*cols; j++) {
            char c2 = GETCHAR(tiles[j]);
            if (c2 == '.') continue; // not interesting
            if (c1 == c2) {
                // Add nodes

                // int d = i - j;
                // int i1 = MAX(i, j) + abs(d);
                // int i2 = MIN(i, j) - abs(d);
                // // THIS CHECK ACCEPTS INVALID ROW OVERFLOWS
                // if (i1 >= 0 && i1 < rows*cols) {
                //     tiles[i1] = SETNODE(tiles[i1]);
                // }
                // if (i2 >= 0 && i2 < rows*cols) {
                //     tiles[i2] = SETNODE(tiles[i2]);
                // }

                int ri = ROW(i);
                int rj = ROW(j);
                int ci = COL(i);
                int cj = COL(j);
                // printf("%i, %i vs %i, %i\n", ri, ci, rj, cj);
                // i + (i-j) vector offset
                int r1 = ri * 2 - rj;
                int c1 = ci * 2 - cj;
                // printf("r: %i, c: %i\n", r1, c1);
                if (
                    r1 >= 0    &&
                    r1 <  rows &&
                    c1 >= 0    &&
                    c1 <  cols
                ) {
                    int i1 = INDEX(r1, c1);
                    tiles[i1] = SETNODE(tiles[i1]);
                }
                else {
                    // printf("Out of bounds\n");
                }
                // j + (j-i) vector offset
                int r2 = rj * 2 - ri;
                int c2 = cj * 2 - ci;
                // printf("r: %i, c: %i\n", r2, c2);
                if (
                    r2 >= 0    &&
                    r2 <  rows &&
                    c2 >= 0    &&
                    c2 <  cols
                ) {
                    int i2 = INDEX(r2, c2);
                    tiles[i2] = SETNODE(tiles[i2]);
                }
                else {
                    // printf("Out of bounds\n");
                }
            }
        }
    }

    int sum = 0;
    for (int i = 0; i < rows*cols; i++) {
        sum += (GETNODE(tiles[i]) ? 1 : 0);
    }

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            char ch = GETNODE(tiles[INDEX(r,c)]) ? '#' : '.';
            printf("%c", ch);
        }
        printf("\n");
    }
    printf("\n");

    printf("Unique locations with antinodes: %i\n", sum);
    return 0;
}
