#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define LINELEN (1024) // maximum length of lines to be read
#define THREADS (11) // Number of worker threads

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
void traverse_map(map* m, volatile int* sum, int tempR, int tempC) {
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
    return;
start_found:

    // traverse the map
    next_r = row + dr;
    next_c = col + dc;
    // m->tiles[row][col] = 'X'; // In this case we are not interested in this
    while(is_rc_valid(m, next_r, next_c)) {
        // If next step can be performed, step
        char c = m->tiles[next_r][next_c];
        int blocked = c == '#';
        blocked |= (next_r == tempR && next_c == tempC);
        if (!blocked) {
            row = next_r;
            col = next_c;
            steps++;
        }
        else {
            rotate_dir(&dr, &dc);
        }
        next_r = row + dr;
        next_c = col + dc;
        if (steps > m->rows * m->cols * 4) {
            // there are no more possible steps to take than each direction on each cell
            // If so many has been taken it must be a loop for sure
            *sum += 1;
            return; // it is a loop
        }


    }
    return; // not a loop
}

typedef enum {
    PROP_STATUS_INVALID = 0,
    PROP_STATUS_READY,
    PROP_STATUS_WORKING,
    PROP_STATUS_DONE,
} PROP_STATUS;

typedef struct {
    map* m;
    volatile int* sum;
    int  tempR;
    int  tempC;
    PROP_STATUS prop_status;
} traverse_properties;


typedef struct {
    traverse_properties pool[THREADS];
    sem_t semaphore;
    pthread_mutex_t mutex;
    volatile int finished;
} traverse_properties_pool;

void* traverse_thread(void* properties_pool) {
    traverse_properties_pool* pool = (traverse_properties_pool*) properties_pool;
    while (!pool->finished) {
        // Wait for anything in the pool
        if (sem_trywait(&pool->semaphore)) {
            sched_yield();
            continue;
        }
        traverse_properties* data;
        pthread_mutex_lock(&pool->mutex);
        // Find the first ready data
        for (int i = 0; i < THREADS; i++) {
            if (pool->pool[i].prop_status == PROP_STATUS_READY) {
                data = &pool->pool[i];
                data->prop_status = PROP_STATUS_WORKING;
                break;
            }
        }
        pthread_mutex_unlock(&pool->mutex);
        // perform the operation on the data
        traverse_map(data->m, data->sum, data->tempR, data->tempC);
        // Signal that this data has been processed
        pthread_mutex_lock(&pool->mutex);
        data->prop_status = PROP_STATUS_DONE;
        pthread_mutex_unlock(&pool->mutex);
    }
    return NULL;
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

    pthread_t threads[THREADS];
    traverse_properties_pool pool;
    memset(&pool, 0, sizeof(pool));
    pool.finished = 0;
    pthread_mutex_init(&pool.mutex, NULL);
    sem_init(&pool.semaphore, 0, 0);

    for (int i = 0; i < THREADS; i++) {
        pthread_create(&threads[i], NULL, traverse_thread, &pool);
    }

    printf("Start feeding work pool\n");
    fflush(stdout);
    volatile int sum = 0;
    for (int r = 0; r < m.rows; r++) {
        for (int c = 0; c < m.cols; c++) {
            if (m.tiles[r][c] == '^') continue; // Do not place a box on guards head
            if (m.tiles[r][c] == '#') continue; // Already blocked, no need to check

            // busy wait until there are free elements in the workpool
            int i = 0;
            while(1) {
                i++;
                i %= THREADS;
                int s = pool.pool[i].prop_status;
                pthread_mutex_lock(&pool.mutex);
                if (s == PROP_STATUS_DONE || s == PROP_STATUS_INVALID) {
                    traverse_properties* p = &pool.pool[i];
                    p->tempR = r;
                    p->tempC = c;
                    p->sum = &sum;
                    p->m = &m;
                    p->prop_status = PROP_STATUS_READY;
                    sem_post(&pool.semaphore);
                    break;
                }
                pthread_mutex_unlock(&pool.mutex);
            }

            pthread_mutex_unlock(&pool.mutex);
        }
    }

    int n;
    sem_getvalue(&pool.semaphore, &n);
    while(n > 0) {
        usleep(10);
        sem_getvalue(&pool.semaphore, &n);
    }
    pool.finished = 1;

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Number of possible loop blockades: %i\n", sum);

    return 0;
}
