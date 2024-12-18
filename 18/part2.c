#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (1024) // maximum length of lines to be read
#define WALL    ('#')
#define FREE    ('.')

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
    char tile;
} node;

void print_map(node** nodes, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        printf("%2i ", r);
        for (int c = 0; c < cols; c++) {
            printf("%c", nodes[r][c].tile);
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
    if (c == '#') return 0;
    printf("Unexpected character: %c, %X\n", c, c);
    return 0;
}

// This should fill every node with the minimum available score
void search_path(node** nodes, int rows, int cols, node* current_node) {
    // printf("Searching around node at (%i,%i)\n", current_node->row, current_node->col);
    fflush(stdout);
    int r = current_node->row;
    int c = current_node->col;
    // Check in all 4 directions
    for (int i = 0; i < 4; i++) {
        int nextr = r + row_offsets[i];
        int nextc = c + col_offsets[i];
        if (!coords_valid(rows, cols, nextr, nextc)) continue;
        // If not free, just skip it
        if (!is_free(nodes[nextr][nextc].tile)) continue;
        uint64_t new_score = current_node->score;
        // If same direction as currently the score to be checked is +1
        new_score += 1;
        // Get other node
        node* next_node = &(nodes[nextr][nextc]);
        // If new score is more or the same as the stored one, ignore it
        if (new_score >= next_node->score) continue;
        // Otherwise this is the new minimum for the next node, and search from there
        else {
            // printf("Searching at new node (%i,%i) with score %li\n", nextr, nextc, new_score);
            next_node->score = new_score;
            search_path(nodes, rows, cols, next_node);
        }
    }
}

void reset_nodes(node** nodes, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            nodes[r][c].score = UINT64_MAX;
        }
    }
}


int main(int argc, char** argv) {
    int rows = 71;
    int cols = 71;
    int limit = 1024;
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

    if (strcmp(argv[1], "example") == 0) {
        rows = 7;
        cols = 7;
        limit = 12;
    }

    // Allocate array to store the tiles
    nodes = (node**) malloc(rows * sizeof(node*));
    for (int r = 0; r < rows; r++)
    {
        nodes[r] = (node*) malloc(cols * sizeof(node));
        for (int c = 0; c < cols; c++) {
            node* n = &nodes[r][c];
            n->col = c;
            n->row = r;
            n->score = UINT64_MAX;
            n->tile = FREE;
        }
    }

    fgets(line, LINELEN, f);
    int r_in, c_in;
    int walls = 0;
    while (!feof(f)) {
        sscanf(line, "%i,%i", &r_in, &c_in);
        nodes[r_in][c_in].tile = WALL;
        walls++;
        if (walls > limit) {
            reset_nodes(nodes, rows, cols);
            nodes[0][0].score = 0;
            search_path(nodes, rows, cols, &nodes[0][0]);
            if (nodes[rows-1][cols-1].score == UINT64_MAX) break;
        }
        fgets(line, LINELEN, f);
    }

    nodes[r_in][c_in].tile = '@';
    print_map(nodes, rows, cols);
    printf("The final blocking tile is at (%i,%i)\n", r_in, c_in);

    return 0;
}
