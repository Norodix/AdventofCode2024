#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN (1024) // maximum length of lines to be read

// Lets use a line intersection formula to find the intersection point
// This can then be checked if it is an integer multiple of the generator vectors
// https://wikimedia.org/api/rest_v1/media/math/render/svg/013a4193eae935296d58199bf902574a99c34a5f
int minimal_path(int px, int py, int ax, int ay, int bx, int by) {
    int x1 = 0;
    int y1 = 0;
    int x2 = ax;
    int y2 = ay;
    int x4 = px;
    int y4 = py;
    int x3 = px-bx;
    int y3 = py-by;

    int denom = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);
    if (denom == 0) {
        printf("Collinear lines found\n");
        printf("This means that the whole solution is invalid\n");
        return 0;
    }

    int A = x1 * y2 - y1 * x2;
    int B = x3 * y4 - y3 * x4;
    int nom_x = A * (x3-x4) - B * (x1 - x2);
    int nom_y = A * (y3-y4) - B * (y1 - y2);
    // printf("A: %i, B: %i, nom_x: %i, nom_y: %i\n", A, B, nom_x, nom_y);
    // printf("Denom: %i\n", denom);


    // If x or y of intersection doesnt lie on an integer point, it cannot be done
    if (nom_x % denom){
        // printf("X not on integer point\n");
        return 0;
    }
    if (nom_y % denom) {
        // printf("Y not on integer point\n");
    }

    int cross_x = nom_x / denom;
    int cross_y = nom_y / denom;
    // printf("Intersection point found: (%i,%i)\n", cross_x, cross_y);

    // Make sure these points are integer multiples of the generator vectors
    if (cross_x % ax) return 0;
    if (cross_y % ay) return 0;
    if ((px - cross_x) % bx) return 0;
    if ((py - cross_y) % by) return 0;

    // If every test passed, return the cost
    return (cross_x / ax) * 3 + ((px - cross_x) / bx);
}

int main(int argc, char** argv) {
    int cols, rows;
    uint8_t** tiles;
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

    int sum = 0;
    int px, py, ax, ay, bx, by;
    while(!feof(f)) {
        fscanf(f, "Button A: X%i, Y%i\n", &ax, &ay);
        fscanf(f, "Button B: X%i, Y%i\n", &bx, &by);
        fscanf(f, "Prize: X=%i, Y=%i\n", &px, &py);
        sum += minimal_path(px, py, ax, ay, bx, by);
    }

    printf("The sum of the minimal paths: %i\n", sum);

    // printf("Minimal path example 1: %i\n", minimal_path(8400, 5400, 94, 34, 22, 67));
    // printf("Minimal path example 1: %i\n", minimal_path(12748, 12176, 26, 66, 67, 21)); 
    // printf("Minimal path example 1: %i\n", minimal_path(7870, 6450, 17, 86, 84 ,37));
    // printf("Minimal path example 1: %i\n", minimal_path(18641, 10279, 69, 23, 27, 71));


   return 0;
}
