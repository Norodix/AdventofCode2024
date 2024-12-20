#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN    (1024) // maximum length of lines to be read
#define WALL       ('#')
#define START      ('S')
#define END        ('E')
#define EMPTY      ('.')
#define CHEATSTART ('s')
#define CHEATEND   ('e')

#define NORTH (0)
#define EAST  (1)
#define SOUTH (2)
#define WEST  (3)

int row_offsets[] = { -1, 0, 1, 0, };
int col_offsets[] = { 0, 1, 0, -1, };

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

typedef struct node {
    uint64_t score;
    int row;
    int col;
} node;

void print_map(char** tiles, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        printf("%2i ", r);
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

int is_free(char c) {
    if (c == '.') return 1;
    if (c == 'S') return 1;
    if (c == 'E') return 1;
    if (c == 's') return 1;
    if (c == 'e') return 1;
    if (c == '#') return 0;
    printf("Unexpected character: %c, %X\n", c, c);
    return 0;
}

// This should fill every node with the minimum available score
void search_path(char** tiles, int rows, int cols, node** nodes, node* current_node) {
    // printf("Searching around node at (%i,%i)\n", current_node->row, current_node->col);
    // fflush(stdout);
    int r = current_node->row;
    int c = current_node->col;
    // Check in all 4 directions
    for (int i = 0; i < 4; i++) {
        int nextr = r + row_offsets[i];
        int nextc = c + col_offsets[i];
        // If not free, just skip it
        if (!is_free(tiles[nextr][nextc])) continue;
        uint64_t new_score = current_node->score;
        new_score += 1;
        // Get other node
        node* next_node = &(nodes[nextr][nextc]);
        // If new score is more or the same as the stored one, ignore it
        if (new_score >= next_node->score) continue;
        // Otherwise this is the new minimum for the next node, and search from there
        // printf("Searching at new node (%i,%i) with score %li\n", nextr, nextc, new_score);
        next_node->score = new_score;
        search_path(tiles, rows, cols, nodes, next_node);
    }
}

int cheat_valid(char** tiles, int rows, int cols, int startr, int startc, int endr, int endc) {
    if (coords_valid(rows, cols, startr, startc) == 0) return 0;
    if (coords_valid(rows, cols, endr, endc) == 0) return 0;
    char startchar = tiles[startr][startc];
    char endchar = tiles[endr][endc];
    if (endchar == WALL) return 0; // Must end on a valid tile
    if (startchar == WALL) return 0;
    // Startchar must have at least 1 tile neighbors, otherwise it is pointless
    int wall_neighbors = 0;
    for (int i = 0; i < 4; i++) {
        int nextr = startr + row_offsets[i];
        int nextc = startc + col_offsets[i];
        if (coords_valid(rows, cols, nextr, nextc) == 0) {
            wall_neighbors++;
        }
        else {
            if (tiles[nextr][nextc] == WALL) wall_neighbors++;
        }
    }
    if (wall_neighbors > 3) return 0;
    return 1;
}

int main(int argc, char** argv) {
    int cols, rows;
    char** tiles;
    node** nodes;
    char line[LINELEN];
    const char facings[4] = {'N', 'E', 'S', 'W'};
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
    tiles = (char**) malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; i++)
    {
        tiles[i] = (char*) malloc(cols * sizeof(int));
    }

    // initialize the nodes
    nodes = (node**) malloc(rows * sizeof(node*));
    for (int i = 0; i < rows; i++)
    {
        nodes[i] = (node*) malloc(cols * sizeof(node));
        for (int j = 0; j < cols; j++) {
            node *n = &nodes[i][j];
            n->score = UINT64_MAX;
            n->row = i;
            n->col = j;
        }
    }

    int startr, startc, endr, endc;
    // Initialize the tiles
    fgets(line, LINELEN, f);
    int r = 0;
    while (!feof(f)) {
        for (int c = 0; c < strlen(line) - 1; c++) {
            tiles[r][c] = line[c];
            if (tiles[r][c] == START) {
                startr = r;
                startc = c;
            }
            if (tiles[r][c] == END) {
                endr = r;
                endc = c;
            }
        }
        r++;
        fgets(line, LINELEN, f);
    }
    print_map(tiles, rows, cols);

    printf("Starting from (%i,%i)\n", startr, startc);
    // Set the starting score (facing east) to 0
    node* startnode = &nodes[startr][startc];
    startnode->score = 0;
    node* endnode = &nodes[endr][endc];

    // walk the maze
    printf("Walk the maze!\n");
    search_path(tiles, rows, cols, nodes, startnode);
    printf("Score of end tile (%i, %i): %lu\n", endr, endc, endnode->score);
    uint64_t base_score = endnode->score;

    // The unique start is the tile from which you step onto the first wall
    int sum = 0;
    // Generate all cheats and check their savings
    // Shorthands: CheatStartRow, CheatStartColumn, CheatEndRow, CheatEndColumn
    for (int csr = 0; csr < rows; csr++)
    {
        for (int csc = 0; csc < cols; csc++) {
            if (tiles[csr][csc] == WALL) continue;
            // Around this cheat start tile get the 20x20 possible cheat end tiles
            for (int cer = csr - 20; cer < csr + 21; cer++){
                for (int cec = csc - 20; cec < csc + 21; cec++){
                    if (csr == cer && csc == cec) continue; // Same tile, just ignore it
                    if (cheat_valid(tiles, rows, cols, csr, csc, cer, cec) == 0) continue;
                    uint64_t enter_score = nodes[csr][csc].score;
                    uint64_t exit_score = nodes[cer][cec].score;
                    uint64_t distance_row = MAX(csr, cer) - MIN(csr, cer);
                    uint64_t distance_col = MAX(csc, cec) - MIN(csc, cec);
                    uint64_t distance = distance_row + distance_col;
                    if (distance > 20) continue;
                    if (exit_score <= enter_score + distance) continue;
                    uint64_t saving = exit_score - (enter_score + distance);
                    if (saving >= 100){
                        sum++;
                        // printf("Saving: %lu\n", saving);
                        // printf("Going from (%i,%i) %lu\n", csr, csc, enter_score);
                        // printf("Going to (%i,%i) %lu\n",   cer, cec, exit_score);
                        // printf("\n");

                    }
                }
            }
        }
    }
    printf("Number of cheats that saves at least 100: %i\n", sum);

    return 0;
}
