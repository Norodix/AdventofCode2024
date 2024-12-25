#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN    (2048) // maximum length of lines to be read

void print_devices(int* device, int num_devices) {
    for (int i = 0; i < num_devices; i++) {
        int n = i*5;
        printf("%i, %i, %i, %i, %i\n", device[n],
                                       device[n+1],
                                       device[n+2],
                                       device[n+3],
                                       device[n+4]);
    }
}

int main(int argc, char** argv) {
    char line[LINELEN];
    int locks[1000][5];
    int keys[1000][5];
    int num_locks = 0;
    int num_keys = 0;

    memset(locks, 0, sizeof(locks));
    memset(keys, 0, sizeof(keys));

    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }

    while (!feof(f)) {
        fgets(line, LINELEN, f);
        // If there is any # in the first row it is a lock
        int* index;
        if (line[0] == '#') {
            // This is a lock
            index = locks[num_locks];
            num_locks++;
        }
        else {
            index = keys[num_keys];
            num_keys++;
        }
        for (int i = 0; i < 5; i++) {
            fgets(line, LINELEN, f);
            for (int c = 0; c < 5; c++) {
                if (line[c] == '#') {
                    index[c]++;
                }
            }
        }
        fgets(line, LINELEN, f); // read the last . or # line
        fgets(line, LINELEN, f); // read the empty line
    }

    printf("Locks: \n");
    print_devices((int*)locks, num_locks);

    printf("Keys: \n");
    print_devices((int*)keys, num_keys);

    int matching = 0;

    for (int k = 0; k < num_keys; k++){
        for (int l = 0; l < num_locks; l++){
            int all_ok = 1;
            for (int c = 0; c < 5; c++) {
                if (locks[l][c] + keys[k][c] > 5) all_ok = 0;
            }
            if (all_ok) matching++;
        }
    }

    printf("The number of non-overlapping pairs is %i\n", matching);

    return 0;
}
