#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINELEN (1024) // maximum length of lines to be read
#define MAX_PAGE_NUMS (128)

typedef struct {
    int before;
    int after;
} dep; // dependency encoded in the manual

// Check if dependency is satisfied. Returns 1 if yes.
// d dependency
// n numbers in nums array that are to be checked
int dep_satisfied(dep d, int* nums, int n) {
    int b = -1;
    int a = -1;
    int retval = 1;
    for (int i = 0; i < n; i++) {
        if (d.before == nums[i]) b = i;
        if (d.after == nums[i]) a = i;
    }
    if (a >= 0 && b >= 0) {
        if (a < b) retval = 0;
    }
    return retval;
}

static int dep_num = 0;
static dep* deps;

// comparison function for qsort
// if any of the dependencies give an order to these 2, return respectively, otherwise 0
int compar (const void* smaller, const void* bigger){
    for (int i = 0; i < dep_num; i++) {
        dep* d = &(deps[i]);
        int s = *(int*)smaller;
        int b = *(int*)bigger;
        if (d->before == s && d->after == b) return 1;
        if (d->before == b && d->after == s) return -1;
    }
    return 0;
}

// line must be terminated by \n or by \0
// for practical reason, returns the middle number of the correct page, 0 otherwise
int page_invalid(char* line, dep* deps, int n_deps) {
    int nums[MAX_PAGE_NUMS];
    int index = 0;
    char* delim = ",\n";
    int valid = 1;

    memset(nums, 0, MAX_PAGE_NUMS);
    // printf("Line: %s", line);

    const char* tok;
    tok = strtok(line, delim);
    while (tok != NULL) {
        sscanf(tok, "%d", &(nums[index]));
        index++;
        tok = strtok(NULL, delim);
    }

    // for (int i = 0; i < index; i++) {
    //     printf("%i ", nums[i]);
    // }
    // printf("\n");

    for (int i = 0; i < n_deps; i++) {
        int is_valid = dep_satisfied(deps[i], nums, index);
        valid &= is_valid;
        if (!is_valid) {
            // printf("Line fails -> %d | %d\n", deps[i].before, deps[i].after);
            qsort(nums, index, sizeof(int), compar);
        }
    }

    if (!valid) {
        return nums[index/2];
    }
    return 0;
}

int main(int argc, char** argv) {
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
    fgets(line, sizeof(line), f);
    int deps_array_size = 8;
    deps = (dep*) malloc(sizeof(dep)*deps_array_size);
    // while the empty line is not reached
    while (line[0] != '\n') {
        dep* d = &deps[dep_num];
        sscanf(line, "%d|%d", &(d->before), &(d->after));
        dep_num++;
        if (dep_num > deps_array_size) {
            // dynamically resize the array when necessary
            deps_array_size *= 2;
            printf("Realloc to size %i\n", deps_array_size);
            deps = (dep*) realloc(deps, deps_array_size * sizeof(dep));
        }
        // get next line
        fgets(line, sizeof(line), f);
    }
    // At the end, dep num is the number of dependencies read

    // Read the page updates and check the dependencies for each of them
    int sum = 0;
    fgets(line, sizeof(line), f);
    while (!feof(f)) {
        sum += page_invalid(line, deps, dep_num);
        fgets(line, sizeof(line), f);
    }

    printf("The sum of the middle numbers is: %d\n", sum);

    return 0;
}
