#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

#define LINELEN (1024) // maximum length of lines to be read

// char operators[] = {'*', '+'};

// evaluates the string recursively
// Returns 1 if found, 0 otherwise
int possible(char* rest, uint64_t current, uint64_t target) {
    char* cont;
    errno = 0;
    uint64_t next = strtoull(rest, &cont, 10);
    // If rest is empty, evaluate if target
    if (cont == rest) {
        // No conversion took place
        return current == target;
    }

    // Test recursively
    int maybe = 0;
    maybe |= possible(cont, current * next, target);
    maybe |= possible(cont, current + next, target);

    return maybe;
}

int main(int argc, char** argv) {
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

    printf("Hello from part1\n");
    fflush(stdout);
    uint64_t sum = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        uint64_t target, first;
        char* rest;
        target = strtoull(line, &rest, 10);
        rest++; // skip :
        first = strtoull(rest, &rest, 10);
        if (possible(rest, first, target)) {
            sum += target;
        }
        fgets(line, LINELEN, f);
    }

    printf("The sum of possible lines: %lu\n", sum);
    fflush(stdout);

    return 0;
}
