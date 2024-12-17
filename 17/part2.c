#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define ADV (0)
#define BXL (1)
#define BST (2)
#define JNZ (3)
#define BXC (4)
#define OUT (5)
#define BDV (6)
#define CDV (7)
#define HLT (8)

static struct registers {
    uint64_t A;
    uint64_t B;
    uint64_t C;
    uint64_t pc;
} regs;

uint8_t instructions[100];
int out_index = 0;
uint8_t output[100];

void halt() {
    // printf("Program halted\n");
    // fflush(stdout);
    // This can break out of the later loop
    out_index = 99;
}

uint64_t get_combo(uint8_t operand) {
    if (operand >= 7) halt();
    if (operand <= 3) return operand;
    return *((uint64_t*)&regs + (operand-4));
}

void adv() {
    uint64_t numerator = regs.A;
    uint8_t operand = get_combo(instructions[regs.pc+1]);
    uint64_t denom = 1<<operand;
    regs.A = numerator / denom;
}

void bdv() {
    uint64_t numerator = regs.A;
    uint8_t operand = get_combo(instructions[regs.pc+1]);
    uint64_t denom = 1<<operand;
    regs.B = numerator / denom;
}

void cdv() {
    uint64_t numerator = regs.A;
    uint8_t operand = get_combo(instructions[regs.pc+1]);
    uint64_t denom = 1<<operand;
    regs.C = numerator / denom;
}

void bxl() {
    regs.B ^= instructions[regs.pc+1];
}

void bst() {
    uint8_t operand = get_combo(instructions[regs.pc+1]) & 0x07;
    regs.B = operand;
}

void bxc() {
    regs.B = regs.B ^ regs.C;
}

void jnz() {
    if (regs.A == 0) return;
    regs.pc = instructions[regs.pc+1] - 2;
}

void out() {
    uint8_t operand = get_combo(instructions[regs.pc+1]) & 0x07;
    // printf("%i,", operand);
    output[out_index++] = operand;
}

void (*lut[])() = {
    adv,
    bxl,
    bst,
    jnz,
    bxc,
    out,
    bdv,
    cdv,
    halt,
};

void test() {
    regs.C = 9;
    regs.B = 0;
    regs.pc = 0;
    instructions[0] = 2;
    instructions[1] = 6;
    lut[instructions[regs.pc]]();
    printf("%lu\n", regs.B);

    regs.A = 10;
    regs.B=0;
    regs.C=0;
    regs.pc=0;
    memset(instructions, HLT, sizeof(instructions));
    memcpy(instructions, (uint8_t[]){5,0,5,1,5,4}, 6);
    lut[instructions[regs.pc]]();
    regs.pc += 2;
    lut[instructions[regs.pc]]();
    regs.pc += 2;
    lut[instructions[regs.pc]]();
    regs.pc += 2;

    regs.A = 2024;
    regs.B=0;
    regs.C=0;
    regs.pc=0;
    memset(instructions, HLT, sizeof(instructions));
    memcpy(instructions, (uint8_t[]){0,1,5,4,3,0}, 6);
    while (1) {
        // execute instruction
        lut[instructions[regs.pc]]();
        // increment pc
        regs.pc += 2;
    }
}

int check_match(uint64_t A, int n, int instruction_count) {
    printf("A: %li\n", A);
    out_index = 0; // Incremented by the output instruction
    regs.A = A;
    regs.B = 0;
    regs.C = 0;
    regs.pc = 0;
    int loop_counter = 0;
    memset(output, '\xFF', sizeof(output));
    while (1) {
        lut[instructions[regs.pc]]();
        regs.pc += 2;
        if (loop_counter++ > 1000) break; // Safety exit
        if (out_index >= instruction_count) break;
    }
    for (int i = 0; i < instruction_count; i++) {
        printf("%i ", instructions[i]);
    }
    printf("\n");
    for (int i = 0; i < instruction_count; i++) {
        printf("%i ", output[i]);
    }
    printf("\n");
    if (memcmp(&output[instruction_count-n],
               &instructions[instruction_count-n],
               n * sizeof(instructions[0]))
        == 0) return 1;
    return 0;
}

// Does it match the output for the last n digits
// The first 3 bits of A (left shifted by instruction length - 1 * 3)
// Determine what the last digit will be
// The method:
//   Check if matches so far
//   If not, return 0
//   If matches and n is already all digits, return 1
//   If matches, go deeper and return 1 if something returned 1
//   Otherwise return 0, this is a dead end
int search_A(uint64_t A, int n, int instruction_count) {
    printf("Testing %li for %i digits\n", A, n);
    // Check if matches so far
    int match = check_match(A, n, instruction_count);
    // If not, return 0
    if (!match) {
        printf("No match\n");
        return 0;
    }
    // If matches and n is already all digits, return 1
    if (n == instruction_count) {
        printf("Found a working number, %li\n", A);
        return 1;
    }
    // If matches, go deeper and return 1 if something returned 1
    for (uint64_t i = 0; i < 8; i++) {
        // Add the new 3 bits to A
        uint64_t new_A = A;
        new_A |= (i << ((instruction_count - n - 1)*3));
        if (search_A(new_A, n+1, instruction_count)) return 1;
    }
    // Otherwise return 0, this is a dead end, none of the possible As worked
    return 0;
}

int main(int argc, char** argv) {
    regs.pc=0;
    memset(instructions, HLT, sizeof(instructions));

    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }

    fscanf(f, "Register A: %li\n", &regs.A);
    fscanf(f, "Register B: %li\n", &regs.B);
    fscanf(f, "Register C: %li\n", &regs.C);

    int ip=0;
    while(!feof(f)) {
        char c = fgetc(f);
        if (c <= '7' && c >= '0') instructions[ip++] = c - '0';
    }

    // printf("Matching for the expected input: %i\n", check_match(117440, ip, ip));
    // printf("Matching for the truncated input: %i\n", check_match(117440, 4, ip));

    // General case is not solvable
    // Special input, the following instructions are executed
    // Start:
    // B = A mod 8
    // B = B ^ 5
    // C = A / (2<<B)
    // A = A / 8
    // B = B ^ C
    // output B
    // if (A!=0) goto start;

    // From last to first output number, search for the last 3 digits of A that solve up to that many digits
    // Because there is some backwards dependency because of the C register, I need a backtracking solution
    printf("Number of instructions found: %i\n", ip);
    search_A(0, 0, ip);

    return 0;
}
