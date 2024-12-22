#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN    (2048) // maximum length of lines to be read
#define PRUNE   (16777216)

uint64_t simulate_secret(uint64_t initial, int n) {
    uint64_t secret = initial;
    uint64_t tmp;
    for (int i = 0; i < n; i++) {
        // Calculate the result of multiplying the secret number by 64. 
        tmp = secret * 64;
        // Then, mix this result into the secret number. Finally, prune the secret number.
        secret ^= tmp;
        secret %= PRUNE;
        // Calculate the result of dividing the secret number by 32. Round the result down to the nearest integer.
        tmp = secret / 32;
        // Then, mix this result into the secret number. Finally, prune the secret number.
        secret ^= tmp;
        secret %= PRUNE;
        // Calculate the result of multiplying the secret number by 2048. 
        tmp = secret * 2048;
        // Then, mix this result into the secret number. Finally, prune the secret number.
        secret ^= tmp;
        secret %= PRUNE;
    }
    return secret;
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

    // printf("Tests\n");
    // printf("%li\n", simulate_secret(123, 1));
    // printf("%li\n", simulate_secret(123, 2));
    // printf("%li\n", simulate_secret(123, 3));
    // printf("%li\n", simulate_secret(123, 4));
    // printf("%li\n", simulate_secret(123, 5));
    // printf("%li\n", simulate_secret(123, 6));
    // printf("%li\n", simulate_secret(123, 7));
    // printf("%li\n", simulate_secret(123, 8));
    // printf("%li\n", simulate_secret(123, 9));
    // printf("%li\n", simulate_secret(123, 10));
    // printf("End Tests\n\n\n");

    uint64_t sum = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        uint64_t secret_2000 = simulate_secret(strtoul(line, NULL, 10), 2000);
        sum += secret_2000;
        fgets(line, LINELEN, f);
    }

    printf("The total sum is %lu\n", sum);
    return 0;
}
