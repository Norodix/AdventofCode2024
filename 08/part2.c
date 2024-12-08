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


    // Loop through the tiles
    for (int i = 0; i < rows*cols; i++) {
        char c1 = GETCHAR(tiles[i]);
        if (c1 == '.') continue; // not interesting
        // Find a pair
        for (int j = i+1; j < rows*cols; j++) {
            char c2 = GETCHAR(tiles[j]);
            if (c2 == '.') continue; // not interesting
            if (c1 == c2) {
                // Pair found
                int ri = ROW(i);
                int rj = ROW(j);
                int ci = COL(i);
                int cj = COL(j);
                // Try to add all the matching antinodes
                for (int k = -cols; k <= cols; k++) {
                    int dr = ri - rj;
                    int dc = ci - cj;
                    int rk = ri + dr * k;
                    int ck = ci + dc * k;
                    // All the invalid conditions lead to a continue
                    if (rk < 0) continue;
                    if (ck < 0) continue;
                    if (rk >= rows) continue;
                    if (ck >= cols) continue;
                    // If everything says this is a valid antinode, set it
                    int ik = INDEX(rk, ck);
                    tiles[ik] = SETNODE(tiles[ik]);
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
