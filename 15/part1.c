#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (1024) // maximum length of lines to be read
#define EMPTY   ('.')
#define WALL    ('#')
#define BOX     ('O')
#define ROBOT   ('@')

void print_map(char** tiles, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            printf("%c", tiles[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}

int gps_sum(char** tiles, int rows, int cols) {
    int sum = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (tiles[r][c] == BOX) {
                sum += 100*r + c;
            }
        }
    }
    return sum;
}

int coords_valid(int rows, int cols, int r, int c) {
    if (r >= rows || r < 0) return 0;
    if (c >= cols || c < 0) return 0;
    return 1;
}

// Returns 0 if cannot push, 1 otherwise
int push(char** tiles, int rows, int cols, int r, int c, int dirr, int dirc) {
    if (dirr == 0 && dirc == 0) return 0;
    // Cannot push off the edge of the map
    if (!coords_valid(rows, cols, r, c)) return 0;
    // If this tile is empty, tell the caller that it can push
    if (tiles[r][c] == EMPTY) return 1;
    // Cannot push walls
    if (tiles[r][c] == WALL) return 0;

    // In other cases, check if the neighbor can be pushed
    int can_push = push(tiles, rows, cols, r+dirr, c+dirc, dirr, dirc);
    if (can_push) {
        tiles[r+dirr][c+dirc] = tiles[r][c];
        tiles[r][c] = EMPTY;
    }
    return can_push;
}

int main(int argc, char** argv) {
    int cols, rows;
    char** tiles;
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
        if (strcmp(line, "\n") == 0) break;
    }
    rewind(f);

    // Allocate array to store the tiles
    tiles = (char**) malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; i++)
    {
        tiles[i] = (char*) malloc(cols * sizeof(int));
    }

    fgets(line, LINELEN, f);
    int r = 0;
    int robot_r, robot_c;
    while (!feof(f)) {
        for (int c = 0; c < strlen(line) - 1; c++) {
            tiles[r][c] = line[c];
            if (tiles[r][c] == ROBOT) {
                robot_r = r;
                robot_c = c;
            }
        }
        fgets(line, LINELEN, f);
        r++;
        if (strcmp(line, "\n") == 0) break;
    }

    print_map(tiles, rows, cols);

    while (!feof(f)) {
        char dir = fgetc(f);
        int dirr=0, dirc=0;
        switch (dir) {
            case '<':
                dirc = -1;
                break;
            case '>':
                dirc = +1;
                break;
            case '^':
                dirr = -1;
                break;
            case 'v':
                dirr = +1;
                break;
        }
        if (push(tiles, rows, cols, robot_r, robot_c, dirr, dirc)) {
            robot_r += dirr;
            robot_c += dirc;
        }
        // printf("%c\n", dir);
        // print_map(tiles, rows, cols);
    }
    print_map(tiles, rows, cols);

    printf("The GPS sum is %i\n", gps_sum(tiles, rows, cols));
    return 0;
}
