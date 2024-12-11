#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (1024) // maximum length of lines to be read
#define REACHED (10)

void print_map(int** tiles, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            printf("%c", tiles[r][c] + '0');
        }
        printf("\n");
    }
    printf("\n");
}

void clear_map(int** tiles, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (tiles[r][c] == REACHED) {
                tiles[r][c] = 9;
            }
        }
    }
}


int coords_valid(int rows, int cols, int r, int c) {
    if (r >= rows || r < 0) return 0;
    if (c >= cols || c < 0) return 0;
    return 1;
}

int follow_trail(int** tiles, int rows, int cols, int r, int c)
{
    int retval = 0;
    // Return invalid coordinates
    if (!coords_valid(rows, cols, r, c)) return 0;

    int n = tiles[r][c];
    int m;
    if (n==9) {
        // Mark the target as reached, dont count it twice
        tiles[r][c] = REACHED;
        return 1;
    }

    // follow in all 4 directions and sum up the results
    int r_cardinal[] = {r+1, r-1, r  , r  };
    int c_cardinal[] = {c  , c  , c+1, c-1};

    for (int i = 0; i < 4; i++) {
        int ri = r_cardinal[i];
        int ci = c_cardinal[i];
        if (coords_valid(rows, cols, ri, ci)) {
            int m = tiles[ri][ci];
            if (m == n+1) {
                retval += follow_trail(tiles, rows, cols, ri, ci);
            }
        }
    }

    return retval;
}

int main(int argc, char** argv) {
    int cols, rows;
    int** tiles;
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

    // Allocate array to store the tiles
    tiles = (int**) malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++)
    {
        tiles[i] = (int*) malloc(cols * sizeof(int));
    }

    fgets(line, LINELEN, f);
    int r = 0;
    while (!feof(f)) {
        for (int c = 0; c < strlen(line) - 1; c++) {
            tiles[r][c] = line[c] - '0';
        }
        fgets(line, LINELEN, f);
        r++;
    }

    print_map(tiles, rows, cols);

    int sum = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (tiles[r][c] == 0) {
                int trail_value = follow_trail(tiles, rows, cols, r, c);
                printf("trail_value for (%i,%i) is %i\n", r, c, trail_value);
                sum += trail_value;
            }
            clear_map(tiles, rows, cols);
        }
    }
    printf("The total sum of trail values is %i\n", sum);
    return 0;
}
