#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (1024) // maximum length of lines to be read
#define EMPTY ('.')
#define ACTIVE ('#')

void print_map(uint8_t** tiles, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            printf("%c", tiles[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}

int coords_valid(int rows, int cols, int r, int c) {
    if (r >= rows || r < 0) return 0;
    if (c >= cols || c < 0) return 0;
    return 1;
}

void flood_fill(uint8_t** tiles, int rows, int cols, int r, int c) {
    // chache the value
    char val = tiles[r][c];
    tiles[r][c] = ACTIVE;
    // The neighboring cells
    int nr[] = {r+1, r-1, r,   r  };
    int nc[] = {c,   c,   c+1, c-1};
    // For the neighbors check if they are the same value
    for (int i = 0; i < 4; i++) {
        if (coords_valid(rows, cols, nr[i], nc[i])) {
            if (tiles[nr[i]][nc[i]] == val) {
                flood_fill(tiles, rows, cols, nr[i], nc[i]);
            }
        }
    }
}

void wipe(uint8_t** tiles, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (tiles[r][c] == ACTIVE) tiles[r][c] = EMPTY;
        }
    }
}

// Count the number of borders around a single tile
int count_tile_borders(uint8_t** tiles, int rows, int cols, int r, int c) {
    int borders = 0;
    // The neighboring cells
    int nr[] = {r+1, r-1, r,   r  };
    int nc[] = {c,   c,   c+1, c-1};
    // For the neighbors check if they are different or dont exists
    for (int i = 0; i < 4; i++) {
        if (coords_valid(rows, cols, nr[i], nc[i])) {
            if (tiles[nr[i]][nc[i]] != ACTIVE) {
                borders++;
            }
        }
        else {
            borders++;
        }
    }
    return borders;
}

int count_perimiter(uint8_t** tiles, int rows, int cols) {
    int perimeter = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (tiles[r][c] == ACTIVE) {
                perimeter += count_tile_borders(tiles, rows, cols, r, c);
            }
        }
    }
    return perimeter;
}

int count_active_tiles(uint8_t** tiles, int rows, int cols) {
    int actives = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (tiles[r][c] == ACTIVE) {
                actives++;
            }
        }
    }
    return actives;
}

// Dynamic padding around tiles
uint8_t get_tiles(uint8_t** tiles, int rows, int cols, int r, int c) {
    if (!coords_valid(rows, cols, r, c)) {
        return EMPTY;
    }
    return tiles[r][c];
}

int count_tile_corners(uint8_t** tiles, int rows, int cols, int r, int c) {
    int corners = 0;
    uint8_t up = get_tiles(tiles, rows, cols, r-1, c+0);
    uint8_t dw = get_tiles(tiles, rows, cols, r+1, c+0);
    uint8_t lt = get_tiles(tiles, rows, cols, r+0, c-1);
    uint8_t rt = get_tiles(tiles, rows, cols, r+0, c+1);
    // diagonal neighbors
    uint8_t ru = get_tiles(tiles, rows, cols, r-1, c+1);
    uint8_t rd = get_tiles(tiles, rows, cols, r+1, c+1);
    uint8_t lu = get_tiles(tiles, rows, cols, r-1, c-1);
    uint8_t ld = get_tiles(tiles, rows, cols, r+1, c-1);

    // Check outter corners
    if (up != ACTIVE && rt != ACTIVE) corners++;
    if (up != ACTIVE && lt != ACTIVE) corners++;
    if (dw != ACTIVE && rt != ACTIVE) corners++;
    if (dw != ACTIVE && lt != ACTIVE) corners++;

    // Check inner corners
    if (up == ACTIVE && rt == ACTIVE && ru != ACTIVE) corners++;
    if (up == ACTIVE && lt == ACTIVE && lu != ACTIVE) corners++;
    if (dw == ACTIVE && rt == ACTIVE && rd != ACTIVE) corners++;
    if (dw == ACTIVE && lt == ACTIVE && ld != ACTIVE) corners++;

    return corners;
}

int count_corners(uint8_t** tiles, int rows, int cols) {
    int corners = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (tiles[r][c] == ACTIVE) {
                corners += count_tile_corners(tiles, rows, cols, r, c);
            }
        }
    }
    return corners;
}

int main(int argc, char** argv) {
    int cols, rows;
    uint8_t** tiles;
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
    tiles = (uint8_t**) malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++)
    {
        tiles[i] = (uint8_t*) malloc(cols * sizeof(int));
    }

    fgets(line, LINELEN, f);
    int r = 0;
    while (!feof(f)) {
        for (int c = 0; c < strlen(line) - 1; c++) {
            tiles[r][c] = line[c];
        }
        fgets(line, LINELEN, f);
        r++;
    }

    // print_map(tiles, rows, cols);
    // flood_fill(tiles, rows, cols, 0, 0);
    // print_map(tiles, rows, cols);
    // printf("Corners: %i\n", count_corners(tiles, rows, cols));
    // printf("Actives: %i\n", count_active_tiles(tiles, rows, cols));
    // wipe(tiles, rows, cols);
    // print_map(tiles, rows, cols);

    int sum = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            // If not empty 
            if (tiles[r][c] != EMPTY) {
                flood_fill(tiles, rows, cols, r, c);
                int cor = count_corners(tiles, rows, cols);
                int act = count_active_tiles(tiles, rows, cols);
                sum += cor * act;
                wipe(tiles, rows, cols);
            }
        }
    }
    printf("Total fence cost: %i\n", sum);

   return 0;
}
