#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINELEN (1024) // maximum length of lines to be read

typedef struct {
    int rows, cols;
    char** tiles;
} map;

// I could add a bunch more safeguards but they are not needed for valid inputs
void read_map(map* m, FILE* f) {
    char line[1024];

    // initalize all variables of the map struct
    m->rows = 0;
    m->cols = 0;

    // count the number of rows and columns
    rewind(f); // just to make sure
    fgets(line, sizeof(line), f);
    m->cols = strlen(line) - 1;
    while (!feof(f)) {
        m->rows++;
        fgets(line, sizeof(line), f);
    }
    rewind(f);

    // allocate the necessary amount of memory
    m->tiles = (char**) malloc(sizeof(char*) * m->rows);
    for (int r = 0; r < m->rows; r++){
        m->tiles[r] = (char*) malloc(sizeof(char) * m->cols);
    }

    // Parse the file for real
    fgets(line, sizeof(line), f);
    int r = 0;
    while (!feof(f)) {
        strncpy(m->tiles[r], line, m->cols);
        // get next line
        fgets(line, sizeof(line), f);
        r++;
    }
}

void print_map(map m) {
    for (int r = 0; r < m.rows; r++) {
        for (int c = 0; c < m.cols; c++) {
            printf("%c", m.tiles[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}

void rotate_dir(int* r, int* c) {
    int tmp_r = *r;
    int tmp_c = *c;

    // rotate by 90 deg right
    *r = tmp_c;
    *c = tmp_r * -1;
}

int is_rc_valid(map* m, int r, int c) {
    if (r < 0 || c < 0) return 0;
    if (r >= m->rows || c >= m->cols) return 0;
    return 1;
}

// Return 1 if the map is a loop, 0 otherwise
int traverse_map(map* m) {
    int row = -1;
    int col = -1; // The current row and column
    int dr  = -1;
    int dc  =  0; // The target direction
    int next_r;
    int next_c; // The next row and column target
    int steps = 0; // The number of steps already taken
    // This is the bruteforce approach,
    // smarter would be to check if the same direction and position has been visited already

    for (int r = 0; r < m->rows; r++) {
        for (int c = 0; c < m->cols; c++) {
            if (m->tiles[r][c] == '^') {
                row = r;
                col = c;
                goto start_found; // double break
            }
        }
    }
    // This should never run in case of a valid input
    printf("The start character could not be found\n");
    return -1;
start_found:

    // traverse the map
    next_r = row + dr;
    next_c = col + dc;
    // m->tiles[row][col] = 'X'; // In this case we are not interested in this
    while(is_rc_valid(m, next_r, next_c)) {
        // If next step can be performed, step
        char c = m->tiles[next_r][next_c];
        // # is the fixed, O is the temporary blockade
        if (c != '#' && c != 'O') {
            row = next_r;
            col = next_c;
            steps++;
        }
        else {
            rotate_dir(&dr, &dc);
        }
        // mark current position as visited
        // m->tiles[row][col] = 'X';
        next_r = row + dr;
        next_c = col + dc;
        // print_map(*m);
        // fflush(stdout);
        // usleep(20000); // sleep 20ms
        if (steps > m->rows * m->cols * 4) {
            // there are no more possible steps to take than each direction on each cell
            // If so many has been taken it must be a loop for sure
            return 1; // it is a loop
        }


    }
    return 0; // not a loop
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }
    map m;

    read_map(&m, f);
    print_map(m);

    // Count the number of Xes on the map
    int sum = 0;
    for (int r = 0; r < m.rows; r++) {
        for (int c = 0; c < m.cols; c++) {
            if (m.tiles[r][c] == '^') continue; // Do not place a box on guards head
            if (m.tiles[r][c] == '#') continue; // Already blocked, no need to check
            m.tiles[r][c] = 'O'; // Set the tile temporarily to blockade
            int blocked = traverse_map(&m);
            if (blocked == -1) {
                printf("This should not happen!!!\n");
                exit(-1);
            }
            sum += blocked;
            m.tiles[r][c] = '.';
        }
    }
    printf("Number of possible loop blockades: %i\n", sum);

    return 0;
}
