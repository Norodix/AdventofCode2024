#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN      (2048) // maximum length of lines to be read
#define MAX_PATT_LEN (10U)

typedef struct {
    char pattern[MAX_PATT_LEN];
    int layers_left;
    uint8_t used;
    uint64_t score;
} cached_score;

#define CACHE_LEN (100000U)
cached_score cache[CACHE_LEN];
// Linear search is slower than a hashmap but idc
void store_cache(cached_score data) {
    // printf("Storing cache: %lu for %s\n", data.score, data.pattern);
    for (int i = 0; i < CACHE_LEN; i++) {
        if (cache[i].used == 0) {
            memcpy(&cache[i], &data, sizeof(data));
            cache[i].used = 1;
            return;
        }
    }
    printf("Cache ran out\n");
    exit(1);
}

cached_score* get_cache(char* pattern, int layers_left) {
    for (int i = 0; i < CACHE_LEN; i++) {
        int used = cache[i].used;
        if (!used) break;
        int same = strncmp(pattern, cache[i].pattern, MAX_PATT_LEN);
        if (same == 0 && layers_left == cache[i].layers_left) {
            // printf("Cache hit for %s at %i\n", pattern, i);
            return &cache[i];
        }
    }
    return NULL;
}

void init_cache() {
    memset(cache, 0U, CACHE_LEN * sizeof(cached_score));
}

int get_row_num(char c) {
    if (c >= '7' && c <= '9') return 0;
    if (c >= '4' && c <= '6') return 1;
    if (c >= '1' && c <= '3') return 2;
    if (c == '0' || c == 'A') return 3;
    return -1;
}

int get_col_num(char c) {
    if (c == '7' || c == '4' || c == '1')           return 0;
    if (c == '8' || c == '5' || c == '2' || c == '0') return 1;
    if (c == '9' || c == '6' || c == '3' || c == 'A') return 2;
    return -1;
}

int get_row_dir(char c) {
    if (c == '^' || c == 'A') return 0;
    if (c == '<' || c == 'v' || c == '>') return 1;
    return -1;
}

int get_col_dir(char c) {
    if (c == '<')             return 0;
    if (c == '^' || c == 'v') return 1;
    if (c == 'A' || c == '>') return 2;
    return -1;
}

const char numpad[4][3] = {
{'7', '8', '9'},
{'4', '5', '6'},
{'1', '2', '3'},
{'F', '0', 'A'},
};

const char dirpad[2][3] = {
{'F', '^', 'A'},
{'<', 'v', '>'},
};

char get_char_dir(int row, int col) {
    return dirpad[row][col];
}

void move_numpad(char* output, char from, char to) {
    // These direction combinations should never move out of the frame
    // UP and then left
    // Right and then down
    // UP is negative direction
    int row_from = get_row_num(from);
    int row_to = get_row_num(to);
    int col_from = get_col_num(from);
    int col_to = get_col_num(to);
    // invalid input character
    if (row_from == -1 ||
        row_to   == -1 ||
        col_from == -1 ||
        col_to   == -1 \
    ) return;
    // Upwards
    while (row_to < row_from) {
        strcat(output, "^");
        row_from--;
    }
    // Right
    while (col_to > col_from) {
        strcat(output, ">");
        col_from++;
    }
    // Downwards
    while (row_to > row_from) {
        strcat(output, "v");
        row_from++;
    }
    // Left
    while (col_to < col_from) {
        strcat(output, "<");
        col_from--;
    }

    strcat(output, "A");
}

void move_dir(char* output, char from, char to) {
    // These direction combinations should never move out of the frame
    // RIGHT and DOWN and only then
    // LEFT and UP
    int row_from = get_row_dir(from);
    int row_to = get_row_dir(to);
    int col_from = get_col_dir(from);
    int col_to = get_col_dir(to);
    // invalid input character
    if (row_from == -1 ||
        row_to   == -1 ||
        col_from == -1 ||
        col_to   == -1 \
    ){
        printf("Invalid character found: %c %c\n", from, to);
        return;
    }
    // Downwards
    while (row_to > row_from) {
        strcat(output, "v");
        row_from++;
    }
    // Right
    while (col_to > col_from) {
        strcat(output, ">");
        col_from++;
    }
    // Upwards
    while (row_to < row_from) {
        strcat(output, "^");
        row_from--;
    }
    // Left
    while (col_to < col_from) {
        strcat(output, "<");
        col_from--;
    }
    strcat(output, "A");
}

#define MIN(a,b) (a < b ? a : b)

#define MAXPERMS (100)

void swap(char *a, char *b) { char t = *a; *a = *b; *b = t; }

int permutations_stored = 0;
char permutations[100][MAX_PATT_LEN];
void permute(char *a, int i, int n) {
    // If we are at the last letter, print it
    if (i >= (n-1)) {
        strcpy(permutations[permutations_stored], a);
        permutations_stored++;
    }
    else {
        // Show all the permutations with the first i-1 letters fixed and 
        // swapping the i'th letter for each of the remaining ones.
        for (int j = i; j < n; j++) {
            swap((a+i), (a+j));
            permute(a, i+1, n);
            swap((a+i), (a+j));
        }
    }
}

// Expects the string to have an unaltered A at the end
void prepare_permutations(char* a) {
    permutations_stored = 0;
    memset(permutations, 0, sizeof(permutations));
    permute(a, 0, strlen(a)-1);
}

void print_permutations() {
    for (int i = 0; i < permutations_stored; i++) {
        printf("%s\n", permutations[i]);
    }
    fflush(stdout);
}

int coords_valid(int row, int col) {
    if (row < 0 || col < 0) return 0;
    if (row > 1 || col > 2) return 0;
    if (row == 0 && col == 0) return 0;
    return 1;
}

// Checks if subsequence is valid in dir
int subsequence_valid(char start, char* sequence) {
    int row = get_row_dir(start);
    int col = get_col_dir(start);
    for (int i = 0; i < strlen(sequence); i++) {
        switch(sequence[i]) {
            case '^':
                row--;
                break;
            case '<':
                col--;
                break;
            case 'v':
                row++;
                break;
            case '>':
                col++;
                break;
            default:
                break;
        }
        if (coords_valid(row, col) == 0) return 0;
    }
    return 1;
}

int coords_valid_num(int row, int col) {
    if (row < 0 || col < 0) return 0;
    if (row > 3 || col > 2) return 0;
    if (row == 3 && col == 0) return 0;
    return 1;
}

// Checks if subsequence is valid in dir
int subsequence_valid_num(char start, char* sequence) {
    int row = get_row_num(start);
    int col = get_col_num(start);
    for (int i = 0; i < strlen(sequence); i++) {
        switch(sequence[i]) {
            case '^':
                row--;
                break;
            case '<':
                col--;
                break;
            case 'v':
                row++;
                break;
            case '>':
                col++;
                break;
            default:
                break;
        }
        if (coords_valid_num(row, col) == 0) return 0;
    }
    return 1;
}


// When these sequencies ending with A are counted they 
// produce a sequence where at the end all lower (higher?) levels end up at A
uint64_t count_min_moves(char* sequence, int layers_left) {
    if (strlen(sequence) == 0) return UINT64_MAX;
    cached_score* cached = get_cache(sequence, layers_left);
    if (cached) {
        return cached->score;
    }
    // printf("Count min moves called with sequence: %s\n", sequence);
    if (layers_left == 0) {
        return strlen(sequence);
    }
    // Otherwise, for each character in the sequence
    // generate all permutations that take from A or the previous to that character
    // This permutation will be a subsequence ending with A
    // and for each permutation get and keep only the minimal count

    uint64_t sum_min = 0;
    char start = 'A';
    for (int i = 0; i < strlen(sequence); i++){
        uint64_t min = UINT64_MAX;
        char base_subsequence[100];
        char subsequence[100];
        memset(base_subsequence, 0, sizeof(base_subsequence));
        move_dir(base_subsequence, start, sequence[i]);
        // printf("move dir called with from %c to %c\n", start, sequence[i]);
        // printf("This generated %s as a base sequence\n", base_subsequence);
        prepare_permutations(base_subsequence);
        // printf("Permutations for this round\n");
        // print_permutations();
        for (int j = 0; j < permutations_stored; j++) { // for each subsequence permutation
            strcpy(subsequence, permutations[j]); // subsequence
            // Check if this permutation is valid
            if (subsequence_valid(start, subsequence) == 0) continue;
            uint64_t subcount = count_min_moves(subsequence, layers_left - 1);
            if (subcount < min) {
                min = subcount;
                // printf("New minimum found for layer %i: %lu\n", layers_left, min);
            }
            prepare_permutations(base_subsequence); // ugly hack, i hate it
        }
        sum_min += min;
        // printf("Minimum %li needed\n", sum_min);
        start = sequence[i];
    }

    // Cache before return
    cached_score new_cache;
    new_cache.used = 1;
    new_cache.score = sum_min;
    new_cache.layers_left = layers_left;
    strcpy(new_cache.pattern, sequence);
    store_cache(new_cache);
    return sum_min;
}

uint64_t do_all_moves(char* input) {
    // printf("\n**************************************\n\n");
    uint64_t moves = 0;
    // Transform the main code to 1st robots instructions
    char start = 'A';
    for (int i = 0; i < strlen(input) - 1; i++){
        uint64_t min = UINT64_MAX;
        char base_subsequence[100];
        char subsequence[100];
        memset(base_subsequence, 0, sizeof(base_subsequence));
        move_numpad(base_subsequence, start, input[i]);
        prepare_permutations(base_subsequence);
        // print_permutations();
        for (int j = 0; j < permutations_stored; j++) { // for each subsequence permutation
            strcpy(subsequence, permutations[j]); // subsequence
            // Check if this permutation is valid
            if (subsequence_valid_num(start, subsequence) == 0){
                // printf("invalid sequence\n");
                continue;
            }
            // printf("Checking permutation %s\n", subsequence);
            uint64_t subcount = count_min_moves(subsequence, 25);
            if (subcount < min) {
                min = subcount;
                // printf("New minimum found for permutation %s, %li\n", subsequence, min);
            }
            prepare_permutations(base_subsequence); // ugly hack, i hate it
        }
        moves += min;
        start = input[i];
    }
    return moves;
}

void test_counter(char* str, int layers) {
    printf("\n\n\n");
    printf("%s %i: %li\n",str, layers, count_min_moves(str, layers));
}

int main(int argc, char** argv) {
    init_cache();
    char line[LINELEN];
    char str1[LINELEN];
    memset(str1, '\0', LINELEN);

    // printf("TESTS\n");
    // // test_counter("<A", 1);
    // // test_counter("<A", 0);
    // // test_counter("<A", 2);
    // // test_counter("v<<A", 1);
    // // test_counter(">>^A", 1);
    // test_counter("^A^^<<A>>AvvvA", 2);
    // test_counter("^A<<^^A>>AvvvA", 2);
    // printf("TESTS END\n");
    // return 0;

    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }

    uint64_t sum = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        uint64_t moves = do_all_moves(line);
        sum += moves * strtol(line, NULL, 10);
        printf("Moves: %li\n", moves);
        fgets(line, LINELEN, f);
    }

    printf("The total sum is %lu\n", sum);
    return 0;
}
