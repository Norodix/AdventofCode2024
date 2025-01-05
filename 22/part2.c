#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (2048) // maximum length of lines to be read
#define PRUNE   (16777216)
#define NUM_PRICES  (2000)
#define MAX(a, b) (a > b ? a : b)

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


uint64_t simulate_next(uint64_t initial) {
    return simulate_secret(initial, 1);
}

void get_prices(int initial, int* out) {
    uint64_t score = initial;
    for (int i = 0; i < NUM_PRICES; i++) {
        out[i] = score % 10;
        score = simulate_next(score);
    }
}

int get_index(int* sequcence) {
    int a, b, c, d;
    a = (sequcence[0] + 10) * 20 * 20 * 20;
    b = (sequcence[1] + 10) * 20 * 20;
    c = (sequcence[2] + 10) * 20;
    d = (sequcence[3] + 10);
    return a+b+c+d;
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

    int rows = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        rows++;
        fgets(line, LINELEN, f);
    }
    rewind(f);

    int** prices = (int**) malloc(sizeof(int*) * rows);
    for (int i = 0; i < rows; i++) {
        prices[i] = (int*) malloc(sizeof(int) * NUM_PRICES);
    }

    int** diffs = (int**) malloc(sizeof(int*) * rows);
    for (int i = 0; i < rows; i++) {
        diffs[i] = (int*) malloc(sizeof(int) * NUM_PRICES - 1);
    }

    uint64_t sum = 0;
    int row = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        get_prices(strtoul(line, NULL, 10), prices[row]);
        fgets(line, LINELEN, f);
        row++;
    }

    // Generate the diffs
    for (int i = 0; i < rows; i++) {
        // For the i-th monkey
        for (int j = 0; j < NUM_PRICES - 1; j++) {
            diffs[i][j] = (prices[i][j+1]) - (prices[i][j]);
            // printf("%i\n", diffs[i][j]);
        }
    }


    int tested[160000];
    int value[160000];
    memset(tested, 0, sizeof(tested));
    memset(value, 0, sizeof(value));
    // int sequence[] = {-2, 1, -1, 3};
    // int max_score = get_score_for_sequence(sequence, diffs, prices, rows);
    int max_score = 0;
    for (int m = 0; m < rows; m++) {
        int tested_index = m*rows+1;
        for (int d = 0; d < NUM_PRICES - 5; d++) {
            // printf(" %i %i\r", m, d);
            // fflush(stdout);
            int* sequence = &diffs[m][d];
            int index = get_index(sequence);
            if (index >= 160000){
                printf("Invalid index\n");
                exit(-1);
            }
            // Check if this monkey has already used up this sequence
            if (tested[index] == tested_index) {
                continue;
            }
            // The monkey already used up this sequence
            tested[index] = tested_index;
            // The value needs to increase by the current amount
            value[index] += prices[m][d+4];
            if (value[index] > max_score) max_score = value[index];
            // max_score = MAX(max_score, get_score_for_sequence(&diffs[m][d], diffs, prices, rows));
        }
        // memset(tested, 0, sizeof(tested));
    }
    printf("\n");

    // for (int i = 0; i < 160000; i++) {
    //     if (value[i] > max_score) {
    //         max_score = value[i];
    //     }
    // }

    printf("We got %i bananas!\n", max_score);
    return 0;
}
