#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (1024) // maximum length of lines to be read
#define EMPTY   ('.')
#define WALL    ('#')
#define ROBOT   ('@')
#define BOX_L   ('[')
#define BOX_R   (']')

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
            if (tiles[r][c] == BOX_L) {
                sum += 100 * r + c;
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

// For part2 I need a separate recursive search for "can push" and if all OK, push
int can_push(char** tiles, int rows, int cols, int r, int c, int dirr, int dirc) {
    if (dirr == 0 && dirc == 0) return 0;
    // Invalid push direction
    if (dirr != 0 && dirc != 0) return 0;
    // Cannot push off the edge of the map
    if (!coords_valid(rows, cols, r, c)) return 0;
    // If this tile is empty, tell the caller that it can push
    if (tiles[r][c] == EMPTY) return 1;
    // Cannot push walls
    if (tiles[r][c] == WALL) return 0;
    // Boxes are only connected left/right, so left/right pushes can proceed as normal
    if (dirr == 0) return can_push(tiles, rows, cols, r, c+dirc, dirr, dirc);

    if (tiles[r][c] == BOX_L) {
        return \
        can_push(tiles, rows, cols, r+dirr, c+dirc, dirr, dirc) &&
        can_push(tiles, rows, cols, r+dirr, c+dirc + 1, dirr, dirc);
    }

    if (tiles[r][c] == BOX_R) {
        return \
        can_push(tiles, rows, cols, r+dirr, c+dirc, dirr, dirc) &&
        can_push(tiles, rows, cols, r+dirr, c+dirc - 1, dirr, dirc);
    }

    if (tiles[r][c] == ROBOT) {
        return can_push(tiles, rows, cols, r+dirr, c+dirc, dirr, dirc);
    }

    // Should never come here
    printf("Unexpected argument combination in can_push\n");
    printf("r: %i, c: %i, dirr: %i, dirc: %i, char: %c\n", r, c, dirr, dirc, tiles[r][c]);
    fflush(stdout);
    exit(-1);
    return 0;
}

// Push should only ever be called after can_push is verified by caller
void push(char** tiles, int rows, int cols, int r, int c, int dirr, int dirc) {
    // Some sanity checks
    if (dirr == 0 && dirc == 0) return;
    // Cannot push off the edge of the map
    if (!coords_valid(rows, cols, r, c)) return;
    // If this tile is empty, tell the caller that it can push
    if (tiles[r][c] == EMPTY) return;
    // Cannot push walls
    if (tiles[r][c] == WALL) return;

    if (dirr == 0) {
        push(tiles, rows, cols, r, c+dirc, dirr, dirc);
    }
    else {
        if (tiles[r+dirr][c+dirc] == BOX_L) {
            push(tiles, rows, cols, r+dirr, c+dirc, dirr, dirc);
            push(tiles, rows, cols, r+dirr, c+dirc + 1, dirr, dirc);
        }
        if (tiles[r+dirr][c+dirc] == BOX_R) {
            push(tiles, rows, cols, r+dirr, c+dirc, dirr, dirc);
            push(tiles, rows, cols, r+dirr, c+dirc - 1, dirr, dirc);
        }
        if (tiles[r+dirr][c+dirc] == ROBOT) {
            push(tiles, rows, cols, r+dirr, c+dirc, dirr, dirc);
        }
    }
    // Actually perform push after everything in the way has been pushed away
    tiles[r+dirr][c+dirc] = tiles[r][c];
    tiles[r][c] = EMPTY;
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
    cols *= 2; // double width everything
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
            switch (line[c]) {
                case '#':
                    tiles[r][c*2] = '#';
                    tiles[r][c*2+1] = '#';
                    break;
                case 'O':
                    tiles[r][c*2] = '[';
                    tiles[r][c*2+1] = ']';
                    break;
                case '.':
                    tiles[r][c*2] = '.';
                    tiles[r][c*2+1] = '.';
                    break;
                case '@':
                    tiles[r][c*2] = '@';
                    tiles[r][c*2+1] = '.';
                    robot_r = r;
                    robot_c = c*2;
                    break;
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
        if (can_push(tiles, rows, cols, robot_r, robot_c, dirr, dirc)) {
            push(tiles, rows, cols, robot_r, robot_c, dirr, dirc);
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
