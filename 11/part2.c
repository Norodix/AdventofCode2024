#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "hashmap.c/hashmap.h"

// To solve part 2 it must be transformed into a memoizable recursive form
// The memoizable function is:
// "How many stones from this number with this number of steps left"
// The reason it is memoizable is because 
// a stone splitting any number of times doesn't affect other stones

#define LINELEN (1024) // maximum length of lines to be read

// Use a linked list of stones

int count_digits(uint64_t in) {
    static int max = 0;
    if (in > max) {
        printf("New max: %li\n", in);
        max = in; 
    }
    int digits = 0;
    while (in > 0) {
        digits++;
        in /= 10;
    }
    return digits;
}

uint64_t pow_10(int n) {
    uint64_t ret = 1;
    while (n--) ret*=10;
    return ret;
}

typedef struct stonehash {
    uint64_t value;
    int steps_left;
    uint64_t children;
} stonehash;

static int user_compare(const void *a, const void *b, void *udata) {
    stonehash *ua = (stonehash*) a;
    stonehash *ub = (stonehash*) b;
    return (ua->value == ub->value && ua->steps_left == ub->steps_left) ? 0 : 1;
}

static uint64_t user_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    stonehash s = *((stonehash*) item);
    s.children = 0;
    return hashmap_sip(&s, sizeof(stonehash), seed0, seed1);
}

struct hashmap* map = NULL;

uint64_t count_children(uint64_t value, int steps_left) {
    // printf("Counting children of %li with %i steps left\n", value, steps_left);
    fflush(stdout);
    uint64_t children = 0;
    if (steps_left == 0) return 1;

    // Check if already in the hashmap
    stonehash s = {value, steps_left, 0};
    stonehash* saved = (stonehash*) hashmap_get(map, &s);
    if (saved != NULL) {
        return saved->children;
    }

    int digits = count_digits(value);
    // rules
    // If the stone is engraved with the number 0, it is replaced by a stone engraved with the number 1.
    if (value == 0) {
        children += count_children(1, steps_left - 1);
    }
    // If the stone is engraved with a number that has an even number of digits,
    // it is replaced by two stones. The left half of the digits are engraved on the new left stone,
    // and the right half of the digits are engraved on the new right stone.
    // (The new numbers don't keep extra leading zeroes: 1000 would become stones 10 and 0.)
    else if (digits % 2 == 0) {
        // uint64_t div = pow(10, digits/2);
        uint64_t div = pow_10(digits/2);
        children += count_children(value / div, steps_left - 1);
        children += count_children(value % div, steps_left - 1);
    }
    // If none of the other rules apply, the stone is replaced by a new stone;
    // the old stone's number multiplied by 2024 is engraved on the new stone.
    else {
        children += count_children(value * 2024, steps_left - 1);
    }

    // Store the new discovered value in the hashmap
    s.children = children;
    hashmap_set(map, &s);

    return children;
}

int main(int argc, char** argv) {
    int cols, rows;
    int** tiles;
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

    map = hashmap_new(sizeof(stonehash),
                      0, 0, 0,
                      user_hash,
                      user_compare,
                      NULL,
                      NULL);

    fgets(line, LINELEN, f);
    char* next = line;
    uint64_t sum = 0;
    // Do it until there are no more values to convert
    while (1) {
        char* end_ptr;
        uint64_t val = strtoul(next, &end_ptr, 10);
        if (end_ptr == next) break;
        next = end_ptr;
        uint64_t children = count_children(val, 75);
        sum += children;
        printf("The number of created stones is: %lu\n", children);
    }

    printf("The number of created stones in total is: %lu\n", sum);
}
