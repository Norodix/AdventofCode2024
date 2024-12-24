#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN    (2048) // maximum length of lines to be read
#define PRUNE   (16777216)
#define NAMELEN (8)
#define OPLEN (4)
#define XOR 'x'
#define AND 'a'
#define OR  'o'

typedef struct gate gate;

struct gate {
    char name[NAMELEN];
    char operation;
    gate* input1;
    gate* input2;
    uint8_t output;
    uint8_t ready;
    uint8_t is_fixed;
    uint8_t fixed_output;
};

void reset_gates(gate* gates, int num_gates) {
    for (int i = 0; i < num_gates; i++) {
        gates[i].ready = gates[i].is_fixed;
        gates[i].output = gates[i].is_fixed & gates[i].fixed_output;
    }
}

// Should be using binary search to improve performance
gate* get_gate_by_name(gate* gates, int num_gates, char* name) {
    for (int i = num_gates-1; i >= 0; i--) {
        if (strncmp(gates[i].name, name, NAMELEN) == 0) return &gates[i];
    }
    return NULL;
}

gate* add_gate_by_name(gate* gates, int* num_gates, char* name) {
    gate* g = get_gate_by_name(gates, *num_gates, name);
    if (g != NULL) return g;

    // printf("Adding gate %s to position %i\n", name, *num_gates);
    g = &gates[*num_gates];
    memset(g, 0U, sizeof(gate));
    strncpy(g->name, name, NAMELEN);
    *num_gates = *num_gates + 1;
    return g;
}

void set_gate_inputs(gate* output, gate* in1, gate* in2) {
    output->input1 = in1;
    output->input2 = in2;
}

void set_gate_inputs_by_name(gate* gates, int* num_gates, char* name, char* in1, char* in2) {
    gate* output = add_gate_by_name(gates, num_gates, name);
    gate* input1 = add_gate_by_name(gates, num_gates, in1);
    gate* input2 = add_gate_by_name(gates, num_gates, in2);
    set_gate_inputs(output, input1, input2);
}

void print_gates(gate* gates, int num_gates) {
    int offset;
    for (int i = 0; i < num_gates; i++) {
        gate* g = &gates[i];
        if (g->is_fixed) {
            printf("%s -> %i\n", g->name, g->fixed_output);
        }
        else {
        char in1[NAMELEN];
        char in2[NAMELEN];
        if (g->input1) {
            strncpy(in1, g->input1->name, NAMELEN);
        }
        else {
            strncpy(in1, "NULL", NAMELEN);
        }
        if (g->input2) {
            strncpy(in2, g->input2->name, NAMELEN);
        }
        else {
            strncpy(in2, "NULL", NAMELEN);
        }
        printf("%s%n <- %s %c\n", g->name, &offset, in1, g->operation);
        printf("%*s <- %s\n", offset, "", in2);
        if (g->ready) {
                printf("%*s -> %i\n", offset, "", g->output);
            }
        printf("\n");
        }
    }
}

void operate_gate(gate* g) {
    if (!g)
    {
        printf("Operate gate called with NULL\n");
        return;
    }
    if (!(g->input1 && g->input2)) return;
    if (!(g->input1->ready && g->input2->ready)) return;
    if (g->is_fixed) return;
    if (g->ready) return;
    // Both are ready so read their output
    switch (g->operation) {
        case AND:
            g->output = g->input1->output & g->input2->output;
            break;
        case XOR:
            g->output = g->input1->output ^ g->input2->output;
            break;
        case OR:
            g->output = g->input1->output | g->input2->output;
            break;
    }
    g->ready = 1;
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

    int lines = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        lines++;
        fgets(line, LINELEN, f);
    }

    // Overestimation of the number of gates
    gate* gates = (gate*) malloc(sizeof(gate) * lines);
    int num_gates = 0;

    char name[NAMELEN];
    char in1[NAMELEN];
    char in2[NAMELEN];
    char op[OPLEN];
    rewind(f);
    fgets(line, LINELEN, f);
    while(!feof(f)) {
        if (strcmp(line, "\n") == 0) {
            break;
            // End of initial inputs
        }
        strncpy(name, line, NAMELEN);
        name[3] = '\0'; // Fixed format of the names
        gate* g = add_gate_by_name(gates, &num_gates, name);
        g->is_fixed = 1;
        g->fixed_output = line[5] - '0'; // hax
        fgets(line, LINELEN, f);
    }

    // Start of connections
    fgets(line, LINELEN, f);
    while(!feof(f)) {
        sscanf(line, "%s %s %s -> %s", in1, op, in2, name);
        set_gate_inputs_by_name(gates, &num_gates, name, in1, in2);
        gate* g = get_gate_by_name(gates, num_gates, name);
        if (strncmp(op, "XOR", OPLEN) == 0) {
            g->operation = XOR;
        }
        else if (strncmp(op, "AND", OPLEN) == 0) {
            g->operation = AND;
        }
        else if (strncmp(op, "OR", OPLEN) == 0) {
            g->operation = OR;
        }
        else {
            printf("Unexpected operation found %s, exit\n", op);
            exit(-1);
        }
        fgets(line, LINELEN, f);
    }
    print_gates(gates, num_gates);
    reset_gates(gates, num_gates);

    // Could be recursive or sth but I can just loop through every gate repeatedly as well
    for (int i = 0; i < num_gates; i++) {
        for (int j = 0; j < num_gates; j++) {
            operate_gate(&gates[j]);
        }
    }
    print_gates(gates, num_gates);

    uint64_t out = 0;
    for (int i = 63; i >= 0; i--) {
        char zname[NAMELEN];
        sprintf(zname, "z%02i", i);
        gate* zg = get_gate_by_name(gates, num_gates, zname);
        if (!zg) continue;
        uint8_t zout = zg->output;
        out |= ((uint64_t)zout) << i;
        printf("%i", zout);
    }
    printf("\n");
    printf("The resulting number is %lu\n", out);
    return 0;
}
