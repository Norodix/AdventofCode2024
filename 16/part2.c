#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (1024) // maximum length of lines to be read
#define WALL    ('#')
#define START   ('S')
#define END     ('E')
#define PATH    ('O')

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
    char facing;
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
    if (c == 'O') return 1;
    if (c == '#') return 0;
    printf("Unexpected character: %c, %X\n", c, c);
    return 0;
}

int get_cost(node* from, node* to) {
    int cost = 1;
    if (from->facing != to->facing) cost += 1000;
    if ((MAX(from->facing, to->facing) - MIN(from->facing, to->facing) == 2))
        cost += 1000;
    return cost;
}

// This should fill every node with the minimum available score
void search_path(char** tiles, int rows, int cols, node*** nodes, node* current_node) {
    // printf("Searching around node at (%i,%i)\n", current_node->row, current_node->col);
    int r = current_node->row;
    int c = current_node->col;
    // Check in all 4 directions
    for (int i = 0; i < 4; i++) {
        int nextr = r + row_offsets[i];
        int nextc = c + col_offsets[i];
        // If not free, just skip it
        if (!is_free(tiles[nextr][nextc])) continue;
        uint64_t new_score = current_node->score;
        // If same direction as currently the score to be checked is +1
        // Get other node
        node* next_node = &(nodes[nextr][nextc][i]);
        new_score += get_cost(current_node, next_node);
        // If new score is more or the same as the stored one, ignore it
        if (new_score >= next_node->score) continue;
        // Otherwise this is the new minimum for the next node, and search from there
        else {
            // printf("Searching at new node (%i,%i) with score %li\n", nextr, nextc, new_score);
            next_node->score = new_score;
            search_path(tiles, rows, cols, nodes, next_node);
        }
    }
}

// Now that I have a minimum path, I can backtrack
// and mark each spot with an O on the tiles map
// While backtracking, i must backtrack to each node
// that could have landed on the current one for the same score.
void backtrack(char** tiles, int rows, int cols, node*** nodes, node* current_node) {
    int r = current_node->row;
    int c = current_node->col;
    // The node says which tile it came from, but the facing on the previous tile could have been anything
    int lastr = current_node->row - row_offsets[current_node->facing];
    int lastc = current_node->col - col_offsets[current_node->facing];
    tiles[r][c] = 'O';

    for (int i = 0; i < 4; i++) {
        node* last_node = &nodes[lastr][lastc][i];
        // This should normally not happen, it would mean we backtracked into a wall
        if (! is_free(tiles[last_node->row][last_node->col])) {
            printf("Backtracked into a wall, should not happen\n");
            printf("(%i,%i)\n", last_node->row, last_node->col);
            continue;
        }

        uint64_t cost = get_cost(last_node, current_node);
        // Skip if could not have arrived here for the same price
        if (last_node->score + cost != current_node->score) continue;
        // Otherwise, backtrack from this node
        if (tiles[last_node->row][last_node->col] != START) {
            backtrack(tiles, rows, cols, nodes, last_node);
        }
    }
}

int count_path(char** tiles, int rows, int cols) {
    int sum = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (tiles[r][c] == PATH) sum++;
            if (tiles[r][c] == START) sum++;
            if (tiles[r][c] == END) sum++;
        }
    }
    return sum;
}


int main(int argc, char** argv) {
    int cols, rows;
    char** tiles;
    node*** nodes;
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
    nodes = (node***) malloc(rows * sizeof(node**));
    for (int i = 0; i < rows; i++)
    {
        nodes[i] = (node**) malloc(cols * sizeof(node*));
        for (int j = 0; j < cols; j++) {
            nodes[i][j] = (node*) malloc(4 * sizeof(node));
            for (int k = 0; k < 4; k++) {
                node *n = &nodes[i][j][k];
                n->facing = k;
                n->score = UINT64_MAX / 2;
                n->row = i;
                n->col = j;
            }
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
    node* startnode = &nodes[startr][startc][EAST];
    startnode->score = 0;

    // walk the maze
    printf("Walk the maze!\n");
    search_path(tiles, rows, cols, nodes, startnode);
    printf("End scores:\n");
    printf("    North: %lu\n", nodes[endr][endc][NORTH].score);
    printf("    East: %lu\n", nodes[endr][endc][EAST].score);
    printf("    South: %lu\n", nodes[endr][endc][SOUTH].score);
    printf("    West: %lu\n", nodes[endr][endc][WEST].score);

    node* min_node = &nodes[endr][endc][NORTH];
    if (nodes[endr][endc][EAST].score < min_node->score) min_node = &nodes[endr][endc][EAST];
    if (nodes[endr][endc][SOUTH].score < min_node->score) min_node = &nodes[endr][endc][SOUTH];
    if (nodes[endr][endc][WEST].score < min_node->score) min_node = &nodes[endr][endc][WEST];

    backtrack(tiles, rows, cols, nodes, min_node);
    print_map(tiles, rows, cols);

    printf("The number of touched path tiles: %i\n", count_path(tiles, rows, cols));
    return 0;
}
