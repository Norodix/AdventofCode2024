#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (40000) // maximum length of lines to be read
#define EMPTY (-1)

int main(int argc, char** argv) {
    int64_t* blocks;
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

    // Only one line to read
    fgets(line, LINELEN, f);

    // Read the number of characters and spaces to create the "filesystem"
    int len = 0;
    for (int i = 0; i < strlen(line)-1; i++) {
        len += line[i] - '0';
    }
    blocks = (int64_t*) malloc(len*sizeof(int64_t));
    for (int i = 0; i < len; i++) {
        blocks[i] = EMPTY;
    }

    // for each character add so many spaces or file blocks
    int file_id = 0;
    int block_index = 0;
    for (int i = 0; i < strlen(line)-1; i++) {
        // For so many blocks as the character says
        int digit = line[i] - '0';
        if (!(i%2)) {
            // Add the file id
            for (int n = 0; n < digit; n++) {
                blocks[block_index] = file_id;
                block_index++;
            }
            file_id++;
        }
        else {
            // Add empty space
            for (int n = 0; n < digit; n++) {
                blocks[block_index] = EMPTY;
                block_index++;
            }
        }
    }

    // for (int i = 0; i < len; i++) {
    //     if (blocks[i] == EMPTY) printf(".");
    //     else printf("%i", blocks[i]);
    // }
    // printf("\n");

    // Compact the files
    int64_t* p_end = &blocks[len-1];
    int64_t* p_start = &blocks[0];
    while(p_end > p_start) {
        if (*p_start != EMPTY) {
            p_start++;
            continue;
        }
        if (*p_end == EMPTY) {
            p_end--;
            continue;
        }
        // At this point p_end stand on a non empty and p_start on an empty
        *p_start = *p_end;
        *p_end=EMPTY;
        p_start++;
        p_end--;
    }

    // for (int i = 0; i < len; i++) {
    //     if (blocks[i] == EMPTY) printf(".");
    //     else printf("%i", blocks[i]);
    // }
    // printf("\n");

    // Calculate the checksum
    uint64_t sum = 0;
    for (int i = 0; i < len; i++) {
        if (blocks[i] == EMPTY) break;
        sum += i*blocks[i];
    }
    printf("The checksum is: %lu\n", sum);


    return 0;
}
