#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "thpool.h"

// This implementation uses the following library:
// https://github.com/Pithikos/C-Thread-Pool
// 4eb5a69


#define LINELEN (1024) // maximum length of lines to be read
#define THREADS (24) // Number of worker threads

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

typedef struct {
    map* m;
    volatile int* sum;
    int  tempR;
    int  tempC;
} traverse_properties;

// Return 1 if the map is a loop, 0 otherwise
void traverse_map(traverse_properties* tp) {
    int row = -1;
    int col = -1;  // The current row and column
    int dr  = -1;
    int dc  =  0;  // The target direction
    int next_r;
    int next_c;    // The next row and column target
    int steps = 0; // The number of steps already taken
    int** freq;    // Frequency tiles
    // This is the bruteforce approach,
    // smarter would be to check if the same direction and position has been visited already
    map* m = tp->m;


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
    goto free_freq;
start_found:
    // Create frequency tiles
    freq = malloc(sizeof(int*) * m->rows);
    for (int r = 0; r < m->rows; r++) {
        freq[r] = malloc(sizeof(int*) * m->cols);
        memset(freq[r], 0, sizeof(int) * m->cols);
    }

    // traverse the map
    next_r = row + dr;
    next_c = col + dc;
    // m->tiles[row][col] = 'X'; // In this case we are not interested in this
    while(is_rc_valid(m, next_r, next_c)) {
        // If next step can be performed, step
        char c = m->tiles[next_r][next_c];
        int blocked = c == '#';
        blocked |= (next_r == tp->tempR && next_c == tp->tempC);
        if (!blocked) {
            row = next_r;
            col = next_c;
            freq[row][col]++;
            if (freq[row][col] > 4){
                // Found a loop
                *(tp->sum) += 1;
                goto free_freq;
            }
        }
        else {
            rotate_dir(&dr, &dc);
        }
        next_r = row + dr;
        next_c = col + dc;
    }
free_freq:
    for (int r = 0; r < m->rows; r++) {
        free(freq[r]);
    }
    free(freq);
    return;
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
    threadpool thpool = thpool_init(THREADS);
    int num_prop = m.rows * m.cols;
    int used_prop = 0;
    traverse_properties* properties = (traverse_properties*) malloc(num_prop * sizeof(traverse_properties));


    printf("Start feeding work pool\n");
    fflush(stdout);
    volatile int sum = 0;
    for (int r = 0; r < m.rows; r++) {
        for (int c = 0; c < m.cols; c++) {
            if (m.tiles[r][c] == '^') continue; // Do not place a box on guards head
            if (m.tiles[r][c] == '#') continue; // Already blocked, no need to check

            // busy wait until there are free elements in the workpool
            while(thpool_num_threads_working(thpool) >= THREADS) {
                usleep(1);
            }
            traverse_properties* p = &properties[used_prop];
            p->tempR = r;
            p->tempC = c;
            p->sum = &sum;
            p->m = &m;
            used_prop++;
            if (used_prop >= num_prop) {
                num_prop <<= 1;
                properties = realloc(properties, num_prop * sizeof(traverse_properties));
            }
            thpool_add_work(thpool, (void (*)(void*))traverse_map, p);
        }
    }
    thpool_wait(thpool);
    thpool_destroy(thpool);

    printf("Number of possible loop blockades: %i\n", sum);
    // Free is ommitted because we return early anyway

    return 0;
}
