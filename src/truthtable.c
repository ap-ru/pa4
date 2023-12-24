#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

struct Directive { 
    char gate[17]; 
    int n; 
    int s;
    int *inputs; 
    int *outputs; 
    int *selectors;
};

void initialize(int size, int *Var) {
    for(int i = 0; i < size; i++) { 
        Var[i] = 0;
    } 
    Var[1] = 1;
}

int indexOf(int size, char **arr, char *gateVar) {
    for(int i = 0; i < size; i++) {
        if(!strcmp(arr[i], gateVar)) { 
            return i; 
        } 
    } 
    return -1;
}

void NOT(int *Var, int input, int output) { 
    Var[output] = !Var[input]; 
}

void PASS(int *Var, int input, int output) { 
    Var[output] = Var[input]; 
}

void AND(int *Var, int input1, int input2, int output) { 
    Var[output] = Var[input1] && Var[input2]; 
}

void OR(int *Var, int input1, int input2, int output) { 
    Var[output] = Var[input1] || Var[input2]; 
}

void NAND(int *Var, int in1, int in2, int output) { 
    Var[output] = !(Var[in1] && Var[in2]); 
}

void NOR(int *Var, int in1, int in2, int output) { 
    Var[output] = !(Var[in1] || Var[in2]); 
}

void XOR(int *Var, int in1, int in2, int output) { 
    Var[output] = Var[in1] ^ Var[in2]; 
}

void DECODER(int *Var, int n, int *inputs, int *outputs) { 

    for (int i = 0; i < (1 << n); i++) { 
        Var[outputs[i]] = 0; 
    }
    int select = 0;
    for (int i = 0; i < n; i++) {
        int dec = n-i-1;
        select += Var[inputs[i]] * (1 << dec); 
    }

    Var[outputs[select]] = 1;
}

void MUX(int *Var, int n, int *inputs, int *selectors, int output) { 
    int s = 0;

    for (int i = 0; i < n; i++) {
        int m = n-i-1;
        s += Var[selectors[i]] * (1 << m); 
    }

    Var[output] = Var[inputs[s]]; 
}

int increment(int *Var, int counter) { 
    for (int i = counter + 1; i >= 2; i--) { 
        Var[i] = !Var[i];
        if (Var[i] == 1) { 
            return 1; 
        }
    }
    return 0;
}

int main(int argc, char **argv) { 

    FILE *file = fopen(argv[1], "r");

    char command[17];
    int stepcounter = 0; 
    int size = 2; 
    int inputcounter = 0, outputcounter = 0;
    struct Directive *steps = NULL; 
    char **arr;
    int *Var = malloc(size * sizeof(int));

    fscanf(file, " %s", command);
    fscanf(file, "%d", &inputcounter);

    size += inputcounter;
    arr = malloc(size * sizeof(char*));
    arr[0] = malloc(2 * sizeof(char));
    strcpy(arr[0], "0\0");
    arr[1] = malloc(2 * sizeof(char));
    strcpy(arr[1], "1\0");

    for (int i = 0; i < inputcounter; i++) {
        arr[i + 2] = malloc(17 * sizeof(char));
        fscanf(file, "%*[: ]%16s", arr[i + 2]);
    }

    fscanf(file, " %s", command);
    fscanf(file, "%d", &outputcounter);

    size += outputcounter;

    arr = realloc(arr, size * sizeof(char*));

    for(int o = 2; o < outputcounter + 2; o++) {
        arr[o+inputcounter] = (char *)malloc(17 * sizeof(char));
        fscanf(file, "%*[: ]%16s", arr[o+inputcounter]);
    }

    while(!feof(file)) { 
        int numOfInputs = 2, numOfOutputs = 1;
        struct Directive current;

        int scanner = fscanf(file, " %s", command);
        if (scanner != 1) { 
            break;
        }

        stepcounter += 1; current.n = 0; current.s = 0;
        strcpy(current.gate, command);

        if (!strcmp(command, "PASS")) { 
            numOfInputs = 1; 
        }
        if (!strcmp(command, "NOT")) { 
            numOfInputs = 1; 
        }
        if (!strcmp(command, "DECODER")) {
            fscanf(file, "%d", &numOfInputs);
            current.n = numOfInputs;
            numOfOutputs = 1 << numOfInputs;
        }
        if (!strcmp(command, "MULTIPLEXER")) {
            fscanf(file, "%d", &numOfInputs);
            current.s = numOfInputs;
            numOfInputs = 1 << numOfInputs;
        }

        current.outputs = malloc(numOfOutputs * sizeof(int));
        current.selectors = malloc(current.s * sizeof(int));
        current.inputs = malloc(numOfInputs * sizeof(int));

        char gateVar[17];

        for (int x = 0; x < numOfInputs; x++) {
            fscanf(file, "%*[: ]%16s", gateVar);
            current.inputs[x] = indexOf(size, arr, gateVar);
        }

        for (int x = 0; x < current.s; x++) {
            fscanf(file, "%*[: ]%16s", gateVar);
            current.selectors[x] = indexOf(size, arr, gateVar);
        }

        for (int x = 0; x < numOfOutputs; x++) {
            fscanf(file, "%*[: ]%16s", gateVar);
            int storeVal = indexOf(size, arr, gateVar);

            if (storeVal == -1) { 
                size += 1;
                arr = realloc(arr, size * sizeof(char*));
                arr[size-1] = (char *)malloc(17 * sizeof(char));

                strcpy(arr[size-1], gateVar); 
                current.outputs[x] = size-1;
            }

            else { 
                current.outputs[x] = storeVal; 
            }
        }

        steps = realloc(steps, stepcounter * sizeof(struct Directive)); 

        steps[stepcounter-1] = current;
    }

    free(Var); 
    Var = (int *)malloc(size * sizeof(int));
    initialize(size, Var);

    while (1<2) { 
        for (int i = 0; i < inputcounter; i++) {
            printf("%d ", Var[i+2]); 
        } 

        printf("|");

        for (int s = 0; s < stepcounter; s++) {
            struct Directive current = steps[s];

            if (!strcmp(current.gate, "NOT")) { 
                NOT(Var, current.inputs[0], current.outputs[0]); 
            }

            if (!strcmp(current.gate, "PASS")) { 
                PASS(Var, current.inputs[0], current.outputs[0]); 
            }

            if (!strcmp(current.gate, "AND")) { 
                AND(Var, current.inputs[0], current.inputs[1], current.outputs[0]); 
            }

            if (!strcmp(current.gate, "OR")) { 
                OR(Var, current.inputs[0], current.inputs[1], current.outputs[0]); 
            }

            if (!strcmp(current.gate, "NAND")) { 
                NAND(Var, current.inputs[0], current.inputs[1], current.outputs[0]); 
            }
            
            if (!strcmp(current.gate, "NOR")) { 
                NOR(Var, current.inputs[0], current.inputs[1], current.outputs[0]); 
            }
            
            if (!strcmp(current.gate, "XOR")) { 
                XOR(Var, current.inputs[0], current.inputs[1], current.outputs[0]); 
            }
            
            if (!strcmp(current.gate, "DECODER")) { 
                DECODER(Var, current.n, current.inputs, current.outputs); 
            }
            
            if (!strcmp(current.gate, "MULTIPLEXER")) {
                MUX(Var, current.s, current.inputs, current.selectors, current.outputs[0]);
            }
        }

        for (int o = 0; o < outputcounter; o++) {
            printf(" %d", Var[inputcounter+o+2]); 
        } 
        printf("\n");

        if(!increment(Var, inputcounter)) { 
            break; 
        }
    }
    
    exit(0);
}