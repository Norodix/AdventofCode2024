#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (4096) // maximum length of lines to be read
#define MAX_COMPONENTS (1024) // maximum number of components
#define MAX_PATTERNS MAX_COMPONENTS
#define MAX_COMP_LEN (32) // max length of a single component
#define MAX_PATT_LEN (1024) // max length of a pattern

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

typedef struct {
    char pattern[MAX_PATT_LEN];
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
}

cached_score* get_cache(char* pattern) {
    for (int i = 0; i < CACHE_LEN; i++) {
        int used = cache[i].used;
        if (!used) break;
        int same = strncmp(pattern, cache[i].pattern, MAX_PATT_LEN);
        if (same == 0) {
            // printf("Cache hit for %s at %i\n", pattern, i);
            return &cache[i];
        }
    }
    return NULL;
}

void init_cache(cached_score* cache, int len) {
    // for (int i = 0; i < len; i++) {
    //     cache[i].used = 0;
    //     cache[i].score = 0;
    //     cache[i].pattern[0] = '\0';
    // }
    memset(cache, 0U, len * sizeof(cached_score));
}

// The pattern parameter contains the so far unmatched part of the pattern
uint64_t match_pattern(char* pattern, char components[MAX_COMPONENTS][MAX_COMP_LEN], int num_components) {
    uint64_t matching_combinations = 0;
    int patt_len = strlen(pattern);
    if (patt_len == 0) return 1; // Everything has been matched
    // Check if already seen this
    cached_score* cached = get_cache(pattern);
    if (cached != NULL) {
        return cached->score;
    }
    for (int i = 0; i < num_components; i++) {
        int comp_len = strlen(components[i]);
        if (comp_len > patt_len) continue; // component too long
        if (strncmp(pattern, components[i], comp_len) == 0) {
            matching_combinations += match_pattern(pattern+comp_len, components, num_components);
        }
    }
    cached_score new_cache;
    new_cache.score = matching_combinations;
    strncpy(new_cache.pattern, pattern, MAX_PATT_LEN);
    store_cache(new_cache);
    return matching_combinations;
}

int main(int argc, char** argv) {
    int num_components = 0;
    int num_patterns = 0;
    char components[MAX_COMPONENTS][MAX_COMP_LEN];
    char patterns[MAX_PATTERNS][MAX_PATT_LEN];
    int limit = 1024;
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

    memset(components, '\0', MAX_COMPONENTS * MAX_COMP_LEN);
    // uint64_t next = strtoull(rest, &cont, 10);
    // // If rest is empty, evaluate if target
    // if (cont == rest) {
    //     // No conversion took place
    //     return current == target;
    // }
    fgets(line, LINELEN, f);
    char* delim = "\n, ";
    char* tok;
    tok = strtok(line, delim);
    while (tok != NULL) {
        strncpy(components[num_components], tok, MAX_COMP_LEN);
        tok = strtok(NULL, delim);
        num_components++;
    }

    // for (int i = 0; i < num_components; i++) {
    //     printf("%i %s\n", i, components[i]);
    // }

    fgets(patterns[num_patterns], LINELEN, f); // eat empty line
    fgets(patterns[num_patterns], LINELEN, f);
    while (!feof(f)) {
        char* newline = strstr(patterns[num_patterns], "\n");
        if (newline) {
            *newline = '\0';
        }
        num_patterns++;
        fgets(patterns[num_patterns], LINELEN, f);
    }

    uint64_t num_possibles = 0;
    for (int i = 0; i < num_patterns; i++) {
        uint64_t matches = match_pattern(patterns[i], components, num_components);
        printf("%3i/%i Matching against pattern %s: %lu\n", i, num_patterns, patterns[i], matches);
        num_possibles += matches;
    }

    printf("The number of possible towel combinations is %lu\n", num_possibles);

    return 0;
}
