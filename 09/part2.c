#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (40000) // maximum length of lines to be read
#define EMPTY (-1)

static int get_space(int64_t* blocks) {
    int space = 0;
    int max = 10;
    while (space < max) {
        if (*(blocks+space) == EMPTY)
        {
            space++;
        }
        else break;
    }

    return space;
}

static void move_files(int64_t* to, int64_t* from, int len) {
    for (int i = 0; i < len; i++) {
        to[i] = from[i];
        from[i] = EMPTY;
    }
}

static void print_fs(int64_t* fs, int len) {
    return; // do not use for actual input
    for (int i = 0; i < len; i++) {
        if (fs[i] == EMPTY) printf(".");
        else printf("%li", fs[i]);
    }
    printf("\n");
}

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

    // File compacting could happen here before the expansion, but meh

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

    print_fs(blocks, len);

    // Record which files have been moved already
    int* moved = (int*) malloc(file_id*sizeof(int));
    memset(moved, 0U, file_id*sizeof(int));

    // Compact the files
    int file_len;
    int64_t* p_end = &blocks[len-1];
    int64_t* p_start = &blocks[0];
    while (p_end > blocks) {
        // Go to the last character of the file
        while (*p_end == EMPTY){
            p_end--;
        }
        file_id = *p_end;
        file_len = 0;
        // Go to the first character of the file
        while (*p_end == file_id) {
            // Special file ID 0 -> first file
            if (file_id == 0) break;
            file_len++;
            p_end--;
        }
        if (file_id == 0) break;
        if (moved[file_id]) continue;
        // printf("Found file id: %i\n", file_id);
        // The length is in file_len and the start is p_end+1
        int64_t* file_start = p_end+1;
        // printf("File length is %i\n", file_len);

        p_start = &blocks[0];
        // This <= is important, sometimes the only space is the one to the left of the file directly
        while(p_start <= p_end) {
            int space = get_space(p_start);
            if (space >= file_len) {
                move_files(p_start, file_start, file_len);
                moved[file_id] = 1;
                print_fs(blocks, len);
                break;
            }
            p_start++;
        }
    }


    // Calculate the checksum
    uint64_t sum = 0;
    for (int i = 0; i < len; i++) {
        if (blocks[i] == EMPTY) continue;
        sum += i*blocks[i];
    }
    printf("The checksum is: %lu\n", sum);


    return 0;
}
