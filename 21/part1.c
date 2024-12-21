#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN    (2048) // maximum length of lines to be read

int get_row_num(char c) {
    if (c >= '7' && c <= '9') return 0;
    if (c >= '4' && c <= '6') return 1;
    if (c >= '1' && c <= '3') return 2;
    if (c == '0' || c == 'A') return 3;
    return -1;
}

int get_col_num(char c) {
    if (c == '7' || c == '4' || c == '1')           return 0;
    if (c == '8' || c == '5' || c == '2' || c == '0') return 1;
    if (c == '9' || c == '6' || c == '3' || c == 'A') return 2;
    return -1;
}

int get_row_dir(char c) {
    if (c == '^' || c == 'A') return 0;
    if (c == '<' || c == 'v' || c == '>') return 1;
    return -1;
}

int get_col_dir(char c) {
    if (c == '<')             return 0;
    if (c == '^' || c == 'v') return 1;
    if (c == 'A' || c == '>') return 2;
    return -1;
}

const char numpad[4][3] = {
{'7', '8', '9'},
{'4', '5', '6'},
{'1', '2', '3'},
{'F', '0', 'A'},
};

const char dirpad[2][3] = {
{'F', '^', 'A'},
{'<', 'v', '>'},
};

char get_char_dir(int row, int col) {
    return dirpad[row][col];
}

void move_numpad(char* output, char from, char to) {
    // These direction combinations should never move out of the frame
    // UP and then left
    // Right and then down
    // UP is negative direction
    int row_from = get_row_num(from);
    int row_to = get_row_num(to);
    int col_from = get_col_num(from);
    int col_to = get_col_num(to);
    // invalid input character
    if (row_from == -1 ||
        row_to   == -1 ||
        col_from == -1 ||
        col_to   == -1 \
    ) return;
    // Upwards
    while (row_to < row_from) {
        strcat(output, "^");
        row_from--;
    }
    // Right
    while (col_to > col_from) {
        strcat(output, ">");
        col_from++;
    }
    // Downwards
    while (row_to > row_from) {
        strcat(output, "v");
        row_from++;
    }
    // Left
    while (col_to < col_from) {
        strcat(output, "<");
        col_from--;
    }

    strcat(output, "A");
}

void move_dir(char* output, char from, char to) {
    // These direction combinations should never move out of the frame
    // RIGHT and DOWN and only then
    // LEFT and UP
    int row_from = get_row_dir(from);
    int row_to = get_row_dir(to);
    int col_from = get_col_dir(from);
    int col_to = get_col_dir(to);
    // invalid input character
    if (row_from == -1 ||
        row_to   == -1 ||
        col_from == -1 ||
        col_to   == -1 \
    ){
        printf("Invalid character found: %c %c\n", from, to);
        return;
    }
    // Downwards
    while (row_to > row_from) {
        strcat(output, "v");
        row_from++;
    }
    // Right
    while (col_to > col_from) {
        strcat(output, ">");
        col_from++;
    }
    // Upwards
    while (row_to < row_from) {
        strcat(output, "^");
        row_from--;
    }
    // Left
    while (col_to < col_from) {
        strcat(output, "<");
        col_from--;
    }
    strcat(output, "A");
}

void do_all_moves(char* output, char* input) {
    char robot1[LINELEN];
    char robot2[LINELEN];
    char robot3[LINELEN]; // Yourself
    memset(robot1, '\0', LINELEN);
    memset(robot2, '\0', LINELEN);
    memset(robot3, '\0', LINELEN);
    // Transform the main code to 1st robots instructions
    char start = 'A';
    for (int i = 0; i < strlen(input); i++){
        move_numpad(robot1, start, input[i]);
        start = input[i];
    }
    printf("Robot1: %s\n", robot1);
    // HERE we should check the permutations of the steps before the next A, to choose the one that produces the shortest output
    // Transform the 1st robots instructions to 2nd robots instructions
    start = 'A';
    for (int i = 0; i < strlen(robot1); i++){
        move_dir(robot2, start, robot1[i]);
        start = robot1[i];
    }
    printf("Robot2: %s\n", robot2);
    // Transform the 2nd robots instructions to 3rd robots instructions
    start = 'A';
    for (int i = 0; i < strlen(robot2); i++){
        move_dir(robot3, start, robot2[i]);
        start = robot2[i];
    }
    printf("Robot3: %s\n", robot3);
    strcpy(output, robot3);
}

int main(int argc, char** argv) {
    char line[LINELEN];
    char str1[LINELEN];
    memset(str1, '\0', LINELEN);

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
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        do_all_moves(str1, line);
        int subsum = strtol(line, NULL, 10) * strlen(str1);
        printf("%s: %li * %i = %i\n", line, strtol(line, NULL, 10), strlen(str1), subsum);
        sum += subsum;
        fgets(line, LINELEN, f);
    }

    printf("The total sum is %i\n", sum);
    return 0;
}
