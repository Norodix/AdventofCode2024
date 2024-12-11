#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (1024) // maximum length of lines to be read

// Use a linked list of stones

typedef struct stone {
    uint64_t value;
    struct stone* prev;
    struct stone* next;
} stone;

void insert_after(stone* insert, stone* after) {
    stone *a, *b; // insert between stone a and b
    a = after;
    b = after->next;
    a->next = insert;
    insert->prev = a;
    if (b != NULL) {
        b->prev = insert;
    }
    insert->next = b;
}

void print_stones(stone* first_stone) {
    stone* s = first_stone;
    do {
        printf("%lu ", s->value);
        s = s->next;
    } while (s != NULL);
    printf("\n");
}

uint64_t count_stones(stone* first_stone) {
    uint64_t count = 0;
    stone* s = first_stone;
    do {
        count++;
        s = s->next;
    } while (s != NULL);
    return count;
}

stone* new_stone() {
    stone* retval = malloc(sizeof(stone));
    memset(retval, 0U, sizeof(stone));
    return retval;
}

int count_digits(uint64_t in) {
    int digits = 0;
    while (in > 0) {
        digits++;
        in /= 10;
    }
    return digits;
}

void transform_stones(stone* first_stone) {
    stone* s = first_stone;
    // Must be saved before, since it is possible that next is modified
    stone* next = s;
    while (next != NULL) {
        s = next;
        next = s->next;
        int digits = count_digits(s->value);
        // rules
        // If the stone is engraved with the number 0, it is replaced by a stone engraved with the number 1.
        if (s->value == 0) {
            s->value = 1;
        }
        // If the stone is engraved with a number that has an even number of digits,
        // it is replaced by two stones. The left half of the digits are engraved on the new left stone,
        // and the right half of the digits are engraved on the new right stone.
        // (The new numbers don't keep extra leading zeroes: 1000 would become stones 10 and 0.)
        else if (digits % 2 == 0) {
            uint64_t div = pow(10, digits/2);
            insert_after(new_stone(), s);
            s->next->value = s->value;
            s->value /= div;
            s->next->value -= s->value * div;
        }
        // If none of the other rules apply, the stone is replaced by a new stone;
        // the old stone's number multiplied by 2024 is engraved on the new stone.
        else {
            s->value *= 2024;
        }
        // rules end
    }
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

    // create a stone linked list
    stone* first_stone = NULL;
    stone* last_stone = NULL;
    fgets(line, LINELEN, f);
    char* next = line;
    // Do it until there are no more values to convert
    while (1) {
        char* end_ptr;
        uint64_t val = strtoul(next, &end_ptr, 10);
        if (end_ptr == next) break;
        next = end_ptr;
        stone* s = new_stone();
        s->value = val;
        if (last_stone) {
            insert_after(s, last_stone);
            last_stone = s;
        }
        else {
            // Special case for the very first stone in the list
            first_stone = s;
            last_stone = s;
        }
    }
    print_stones(first_stone);

    for (int i = 0; i < 25; i++) {
        transform_stones(first_stone);
        // print_stones(first_stone);
    }

    printf("The number of created stones is: %lu\n", count_stones(first_stone));
}
