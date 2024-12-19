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

// Recursively try to match the pattern with the components available
// Return 1 if match is found, 0 otherwise
// The pattern parameter contains the so far unmatched part of the pattern
int match_pattern(char* pattern, char components[MAX_COMPONENTS][MAX_COMP_LEN], int num_components) {
    // printf("Matching pattern: %s\n", pattern);
    fflush(stdout);
    int patt_len = strlen(pattern);
    if (patt_len == 0) return 1; // Everything has been matched
    for (int i = 0; i < num_components; i++) {
        int comp_len = strlen(components[i]);
        if (comp_len > patt_len) continue; // component too long
        if (strncmp(pattern, components[i], comp_len) == 0) {
            int all_match = match_pattern(pattern+comp_len, components, num_components);
            if (all_match) return 1;
        }
    }
    // No matching part was found, return 0
    return 0;
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

    int num_possibles = 0;
    for (int i = 0; i < num_patterns; i++) {
        int matches = match_pattern(patterns[i], components, num_components);
        printf("%i %s %s\n", i, patterns[i], matches ? "matches" : "is impossible");
        num_possibles += matches;
    }

    printf("The number of possible towel combinations is %i\n", num_possibles);

    return 0;
}
