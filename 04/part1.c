#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    int w; // width
    int h; //height
    char k[4][4]; // the effective kernel, always squished to the top left
} kern;

kern k1 = {
    .h = 1,
    .w = 4,
    .k = {
        {'X', 'M', 'A', 'S',},
        {'.', '.', '.', '.',},
        {'.', '.', '.', '.',},
        {'.', '.', '.', '.',},
    },
};
kern k2 = {
    .h = 1,
    .w = 4,
    .k = {
        {'S', 'A', 'M', 'X',},
        {'.', '.', '.', '.',},
        {'.', '.', '.', '.',},
        {'.', '.', '.', '.',},
    },
};
kern k3 = {
    .h = 4,
    .w = 1,
    .k = {
        {'X', '.', '.', '.',},
        {'M', '.', '.', '.',},
        {'A', '.', '.', '.',},
        {'S', '.', '.', '.',},
    },
};
kern k4 = {
    .h = 4,
    .w = 1,
    .k = {
        {'S', '.', '.', '.',},
        {'A', '.', '.', '.',},
        {'M', '.', '.', '.',},
        {'X', '.', '.', '.',},
    },
};
kern k5 = {
    .h = 4,
    .w = 4,
    .k = {
        {'X', '.', '.', '.',},
        {'.', 'M', '.', '.',},
        {'.', '.', 'A', '.',},
        {'.', '.', '.', 'S',},
    },
};
kern k6 = {
    .h = 4,
    .w = 4,
    .k = {
        {'S', '.', '.', '.',},
        {'.', 'A', '.', '.',},
        {'.', '.', 'M', '.',},
        {'.', '.', '.', 'X',},
    },
};
kern k7 = {
    .h = 4,
    .w = 4,
    .k = {
        {'.', '.', '.', 'X',},
        {'.', '.', 'M', '.',},
        {'.', 'A', '.', '.',},
        {'S', '.', '.', '.',},
    },
};
kern k8 = {
    .h = 4,
    .w = 4,
    .k = {
        {'.', '.', '.', 'S',},
        {'.', '.', 'A', '.',},
        {'.', 'M', '.', '.',},
        {'X', '.', '.', '.',},
    },
};

int count_kernel(char** table, int w, int h, kern kernel){
    int sum = 0;
    for (int y = 0; y < h - kernel.h + 1; y++){
        for (int x = 0; x < w - kernel.w + 1; x++){
            int match = 1;
            for (int ky = 0; ky < kernel.h; ky++){
                for (int kx = 0; kx < kernel.w; kx++){
                    // Skip if . is encountered, this indicates the dont care in the kernel
                    if (kernel.k[ky][kx] == '.') continue;
                    if (kernel.k[ky][kx] != table[y+ky][x+kx])
                    {
                        match = 0;
                        // printf("Kernel fails at row %i, col %i\n", y, x);
                        // goto used for double break, early return from already failed kernel
                        goto kern_fail;
                    }
                }
            }
        kern_fail:
            sum += match;
        }
    }
    return sum;
}


int main(int argc, char** argv){
    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }
    char line[1024];
    int lines = 0;
    fgets(line, sizeof(line), f);
    int x = strlen(line)-1;
    while (!feof(f)){
        lines++;
        fgets(line, sizeof(line), f);
    }
    rewind(f);

    char** table = (char**)malloc(lines * sizeof(char*));
    for (int i = 0; i < lines; i++) {
        fgets(line, sizeof(line), f);
        table[i] = (char*)malloc(x * sizeof(char));
        for (int j = 0; j < x; j++) {
            table[i][j] =  line[j];
        }
    }


    kern kernels[] = {k1, k2, k3, k4, k5, k6, k7, k8};
    int sum = 0;
    for (int i = 0; i < sizeof(kernels)/sizeof(kernels[0]); i++){
        sum += count_kernel(table, x, lines, kernels[i]);
    }

    printf("Number of XMAS in table: %i\n", sum);
}
