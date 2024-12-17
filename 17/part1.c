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

uint8_t instructions[1000];

void halt() {
    printf("Program halted\n");
    fflush(stdout);
    exit(0);
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
    printf("%i,", operand);
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

    while (1) {
        // execute instruction
        lut[instructions[regs.pc]]();
        // increment pc
        regs.pc += 2;
    }

    return 0;
}
