#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NUM_REGISTERS 32
#define MEMORY_SIZE 4096
#define MAX_INSTRUCTIONS 100

const char* REGISTER_NAMES[32] = {
    "$zero", // 0
    "$at", // 1
    "$v0", "$v1", // 2-3
    "$a0", "$a1", "$a2", "$a3", // 4-7
    "$t0", "$t1", "$t2", "$t3", // 8-11
    "$t4", "$t5", "$t6", "$t7", // 12-15
    "$s0", "$s1", "$s2", "$s3", // 16-19
    "$s4", "$s5", "$s6", "$s7", // 20-23
    "$t8", "$t9", // 24-25 
    "$k0", "$k1", // 26-27     
    "$gp", // 28   
    "$sp", // 29   
    "$fp", // 30   
    "$ra"  // 31
};

typedef struct {
    char name[50];
    int address;
}LABEL;

typedef struct {
    char name[50];
    int address;
}DATA;

typedef struct {
    char opcode[10];
    char operands[3][10];
    int binary;
}INSTRUCTION;

// Global variables

int registers[NUM_REGISTERS];
char memory[MEMORY_SIZE];
LABEL labels[100];
DATA data[100];
INSTRUCTION instructions[MAX_INSTRUCTIONS];
int label_count = 0;
int data_count = 0;
int instruction_count = 0;
int pc = 0;

// Numero do registrador pelo nome
int getRegisterNumber(const char* reg_name) {
    for(int i = 0; i < NUM_REGISTERS; i++) {
        if(strcmp(REGISTER_NAMES[i], reg_name) == 0) {
            return i;
        }
    }
    return -1; // Caso não encontre
}

// Limpa tela
void clearScreem() {
    system("clear");
}

int InstructionToBinary(INSTRUCTION *inst) {
    if(strcmp(inst->opcode, "add") == 0) {
        return 0x00000020;
    }
    else if(strcmp(inst->opcode, "addi") == 0) {
        return 0x20000000;
    }
    else if(strcmp(inst->opcode, "sub") == 0) {
        return 0x00000022;
    }
    else if(strcmp(inst->opcode, "mult") == 0) {
        return 0x00000018;
    }
    else if(strcmp(inst->opcode, "and") == 0) {
        return 0x00000024;
    }
    else if(strcmp(inst->opcode, "or") == 0) {
        return 0x00000025;
    }
    else if(strcmp(inst->opcode, "sll") == 0) {
        return 0x00000000;
    }
    else if(strcmp(inst->opcode, "syscall") == 0) {
        return 0x0000000c;
    }
    else if(strcmp(inst->opcode, "lw") == 0) {
        return 0x8c000000;
    }
    else if(strcmp(inst->opcode, "sw") == 0) {
        return 0xac000000;
    }
    else if(strcmp(inst->opcode, "lui") == 0) {
        return 0x3c000000;
    }
    else if(strcmp(inst->opcode, "li") == 0) {
        return 0x24000000;
    }
    else if(strcmp(inst->opcode, "la") == 0) {
        return 0x3c000000;
    }

    return 0; 
}

int getBinary(char *instruction_name) {
    if(strcmp(instruction_name, "add") == 0) {
        return 0x00000020;
    }
    else if(strcmp(instruction_name, "addi") == 0) {
        return 0x20000000;
    }
    else if(strcmp(instruction_name, "sub") == 0) {
        return 0x00000022;
    }
    else if(strcmp(instruction_name, "mult") == 0) {
        return 0x00000018;
    }
    else if(strcmp(instruction_name, "and") == 0) {
        return 0x00000024;
    }
    else if(strcmp(instruction_name, "or") == 0) {
        return 0x00000025;
    }
    else if(strcmp(instruction_name, "sll") == 0) {
        return 0x00000000;
    }
    else if(strcmp(instruction_name, "syscall") == 0) {
        return 0x0000000c;
    }
    else if(strcmp(instruction_name, "lw") == 0) {
        return 0x8c000000;
    }
    else if(strcmp(instruction_name, "sw") == 0) {
        return 0xac000000;
    }
    else if(strcmp(instruction_name, "lui") == 0) {
        return 0x3c000000;
    }
    else if(strcmp(instruction_name, "li") == 0) {
        return 0x24000000;
    }
    else if(strcmp(instruction_name, "la") == 0) {
        return 0x3c000000;
    }
    return 0;
}

void displayInstruction(int pc) {
    instructions[pc].binary = getBinary(instructions[pc].opcode);
    printf("\n=== Instrução ===\n");
    printf("Instrução: %s %s %s %s\n", instructions[pc].opcode, instructions[pc].operands[0], instructions[pc].operands[1], instructions[pc].operands[2]);
    printf("Instrução em binário: %08x\n", instructions[pc].binary);
}

void displayRegisters() {
    printf("\n=== Registradores === \n");
    printf("Registradores de Propósito Especial:\n");
    printf("%-8s: %8d   %-8s: %8d   %-8s: %8d   %-8s: %8d\n",
           REGISTER_NAMES[0], registers[0],   // $zero
           REGISTER_NAMES[1], registers[1],   // $at
           REGISTER_NAMES[26], registers[26], // $k0
           REGISTER_NAMES[27], registers[27]); // $k1


    printf("Registradores de Retorno de Valores e de Argumentos:\n");
    printf("%-8s: %8d   %-8s: %8d\n",
           REGISTER_NAMES[2], registers[2],   // $v0
           REGISTER_NAMES[3], registers[3]);  // $v1
    
    printf("Argumentos:\n");
    for (int i = 4; i < 8; i++) {
        printf("%-8s: %8d%s", 
               REGISTER_NAMES[i], registers[i],
               (i % 4 == 3) ? "\n" : "   "); // a's
    }


    printf("\nTemporarios:\n");
    for (int i = 8; i < 16; i++) {
        printf("%-8s: %8d%s", 
               REGISTER_NAMES[i], registers[i],
               (i % 4 == 3) ? "\n" : "   "); // $t's
    }
    printf("%-8s: %8d   %-8s: %8d\n",
           REGISTER_NAMES[24], registers[24], // $t8
           REGISTER_NAMES[25], registers[25]); // $t9

    printf("\nSalvos:\n");
    for (int i = 16; i < 24; i++) {
        printf("%-8s: %8d%s", 
               REGISTER_NAMES[i], registers[i],
               (i % 4 == 3) ? "\n" : "   "); // $s's
    }

    printf("\nEspeciais:\n");
    printf("%-8s: %8d   %-8s: %8d   %-8s: %8d   %-8s: %8d\n",
           REGISTER_NAMES[28], registers[28], // $gp
           REGISTER_NAMES[29], registers[29], // $sp
           REGISTER_NAMES[30], registers[30], // $fp
           REGISTER_NAMES[31], registers[31]); // $ra
}

void displayDataSegment() {
    printf("\n=== Data Segment ===\n");
    for(int i = 0; i < data_count; i++) {
        printf("%s (0x%04x): ", data[i].name, data[i].address);

        if(strchr(&memory[data[i].address], '\0') != NULL) { // if is string
            printf("\"%s\"\n", &memory[data[i].address]);
        } else { // if is word
            printf("%d\n", *((int*)&memory[data[i].address]));
        }
    }
}

// get the address of the informed label
int getLabelAddress(const char *label_name) {
    for(int i = 0; i < data_count; i++) {
        if(strcmp(data[i].name, label_name) == 0) {
            return data[i].address;
        }
    }
    return -1;
}

// parseOffset

void parseOffset(const char *operand, int *offset, char *base_reg) {
    *offset = 0;
    strcpy(base_reg, "");

    // find where's opening parenthesis
    char *open_paren = strchr(operand, '(');
    if(!open_paren) return;

    // get the offset value
    char offset_str[10] = {0};
    strncpy(offset_str, operand, open_paren - operand);
    *offset = atoi(offset_str);

    // get register name
    char *close_paren = strchr(operand, ')');
    if(!close_paren) return;

    strncpy(base_reg, open_paren + 1, close_paren - open_paren + 1);
    base_reg[close_paren - open_paren - 1] = '\0';
}

void executeInstruction(int pc) {
    
    INSTRUCTION *instruction = &instructions[pc];

    // li instruction
    if(strcmp(instruction->opcode, "li") == 0) {
        int rd = getRegisterNumber(instruction->operands[0]);  // destination register
        int immediate = atoi(instruction->operands[1]);        // immediate value
        
        if(rd != -1) {
            registers[rd] = immediate;
        }
        else {
            printf("Erro: Nome de registrador inválido\n");
            return;
        }
    } 

    // la instruction
    if(strcmp(instruction->opcode, "la") == 0) {
        int rd = getRegisterNumber(instruction->operands[0]);

        char label[50];
        strncpy(label, instruction->operands[1], sizeof(label) - 1);
        label[sizeof(label) - 1] = '\0';

        int address = getLabelAddress(label);

        if(rd != -1 && address != -1) {
            if(rd == 0) {
                printf("Erro. Registrador $zero não pode ser modificado");
                return;
            }
            registers[rd] = address;
        }
        
        else {
            if(rd == -1) {
                printf("Erro, nome de registrador inválido: %s\n", instruction->operands[0]);
                return;
            }
            if(address == -1) {
                printf("Erro, label não encontrada: %s\n", label);
                return;
            }
        }
    }
    
    // add instruction
    else if(strcmp(instruction->opcode, "add") == 0) {
        int rd = getRegisterNumber(instruction->operands[0]);
        int rs = getRegisterNumber(instruction->operands[1]);
        int rt = getRegisterNumber(instruction->operands[2]);

        if(rd != -1 && rs != -1 && rt != -1) {
            registers[rd] = registers[rs] + registers[rt];
        }
        else {
            printf("Erro: Nome de registrador inválido\n");
            return;
        }
    }

    // addi instruction
    else if(strcmp(instruction->opcode, "addi") == 0) {
        int rd = getRegisterNumber(instruction->operands[0]);
        int rs = getRegisterNumber(instruction->operands[1]);
        int immediate = atoi(instruction->operands[2]);

        if(rd != -1 && rs != -1) {
            registers[rd] = registers[rs] + immediate;
        }
        else {
            printf("Erro: Nome de registrador inválido\n");
            return;
        }
    }

    // instruction sub
    if(strcmp(instruction->opcode, "sub") == 0) {
        int rd = getRegisterNumber(instruction->operands[0]);
        int rs = getRegisterNumber(instruction->operands[1]);
        int rt = getRegisterNumber(instruction->operands[2]);
        
        if(rd != -1 && rs != -1 && rt != -1) {
            registers[rd] = registers[rs] - registers[rt];
        }

        else {
            printf("Erro: Nome de registrador inválido");
            return;
        }
    }

    // instruction mult
    if(strcmp(instruction->opcode, "mult") == 0) {
        int rd = getRegisterNumber(instruction->operands[0]);
        int rs = getRegisterNumber(instruction->operands[1]);
        int rt = getRegisterNumber(instruction->operands[2]);
        
        if(rd != -1 && rs != -1 && rt != -1) {
            registers[rd] = registers[rs] * registers[rt];
        }

        else {
            printf("Erro: Nome de registrador inválido");
            return;
        }
    }

    // instruction and
    if(strcmp(instruction->opcode, "and") == 0) {
        int rd = getRegisterNumber(instruction->operands[0]);
        int rs = getRegisterNumber(instruction->operands[1]);
        int rt = getRegisterNumber(instruction->operands[2]);
        
        if(rd != -1 && rs != -1 && rt != -1) {
            registers[rd] = registers[rs] & registers[rt];
            return;
        }

        else {
            printf("Erro: Nome de registrador inválido");
            return;
        }
    }

    // instruction or
    if(strcmp(instruction->opcode, "or") == 0) {
        int rd = getRegisterNumber(instruction->operands[0]);
        int rs = getRegisterNumber(instruction->operands[1]);
        int rt = getRegisterNumber(instruction->operands[2]);
        
        if(rd != -1 && rs != -1 && rt != -1) {
            registers[rd] = registers[rs] | registers[rt];
            
        }

        else {
            printf("Erro: Nome de registrador inválido");
            return;
        }
    }

    // instruction shift to left
    if(strcmp(instruction->opcode, "sll") == 0) {
        int rd = getRegisterNumber(instruction->operands[0]);
        int rs = getRegisterNumber(instruction->operands[1]);
        int rt = getRegisterNumber(instruction->operands[2]);
        
        if(rd != -1 && rs != -1 && rt != -1) {
            registers[rd] = registers[rs] << registers[rt];
        }

        else {
            printf("Erro: Nome de registrador inválido");
            return;
        }
    }

    // syscall
    else if (strcmp(instruction->opcode, "syscall") == 0) {
            if (registers[2] == 4) { // Print string
                int addr = registers[4];
                printf("%s", &memory[addr]);
            }

            if (registers[2] == 1) { // print integer
                int print = registers[4];
                printf("%d", print);
            }

            if (registers[2] == 10) { // exit
                exit(0);
            }
    }

    // instruction lw
    if(strcmp(instruction->opcode, "lw") == 0) {
        int rd = getRegisterNumber(instruction->operands[0]);
        if(rd == -1) {
            printf("Erro: Nome de registrador inválido");
            return;
        }

        // parse offset and base register
        int offset;
        char base_reg[10];
        parseOffset(instruction->operands[1], &offset, base_reg);

        int base_reg_num = getRegisterNumber(base_reg);
        if(base_reg_num == -1) {
            printf("Erro: Registrador base inválido");
            return;
        }

        int effective_address = registers[base_reg_num] + offset;

        if(effective_address < 0 || effective_address >= MEMORY_SIZE - 3) {
            printf("Erro: Acesso à memória fora do limite");
            return;
        }

        // load
        registers[rd] = *((int *)&memory[effective_address]);
    }

    // instruction sw
    if(strcmp(instruction->opcode, "sw") == 0) {
        int rs = getRegisterNumber(instruction->operands[0]);
        if(rs == -1) {
            printf("Erro: Nome de registrador inválido");
            return;
        }

        int offset;
        char base_reg[10];
        parseOffset(instruction->operands[1], &offset, base_reg);

        int base_reg_num = getRegisterNumber(base_reg);
        if(base_reg_num == -1) {
            printf("Erro: Registrador base inválido");
            return;
        }

        int effective_address = registers[base_reg_num] + offset;
        
        if(effective_address < 0 || effective_address >= MEMORY_SIZE - 3) {
            printf("Erro: Acesso à memória fora do limite");
            return;
        }

        *((int *)&memory[effective_address]) = registers[rs];
    }

    // instruction lui
    if(strcmp(instruction->operands, "lui") == 0) {
        int rt = getRegisterNumber(instruction->operands[0]);

        if(rt == -1) {
            printf("Erro: Nome de registrador inválido");
            return;
        }

        if(rt == 0) {
            printf("Erro: Não pode-se modificar o registrador $zero");
        }

        int immediate = atoi(instruction->operands[1]);

        registers[rt] = immediate << 16;
    }
}


void loadProgram(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    char line[256];
    int in_data = 0, in_text = 0;
    int data_address = 0;

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, " \t\n");
        if (!token) continue;

        if (strcmp(token, ".data") == 0) {
            in_data = 1;
            in_text = 0;
        } else if (strcmp(token, ".text") == 0) {
            in_data = 0;
            in_text = 1;
        } else if (in_data) {
            char *label = token;
            if(label[strlen(label) - 1] == ':') {
                label[strlen(label) - 1] = '\0';

                token = strtok(NULL, " \t\n");
                if(!token) continue;

                if(strcmp(token, ".word") == 0) {
                    strcpy(data[data_count].name, label);
                    data[data_count].address = data_address;
                    
                    token = strtok(NULL, " \t\n");
                    
                    while(token != NULL) {
                        int value = atoi(token);
                        *((int*)&memory[data_address]) = value;
                        
                        data_address += 4;

                        token = strtok(NULL, " ,\t\n");
                    }

                    data_count++;
                }

                else if(strcmp(token, ".asciiz") == 0) {
                    token = strtok(NULL, "\n");
                    if(!token) continue;

                    char *string = token;
                    if(string[0] == '"') {
                        string++;
                        if(string[strlen(string) - 1]) {
                            string[strlen(string) - 1] = '\0';
                        }
                    }

                    strcpy(data[data_count].name, label);
                    data[data_count].address = data_address;

                    strcpy(&memory[data_address], string);
                    data_address += strlen(string) + 1;
                    data_count++;
                }
            }
        } else if (in_text) {
            if (strchr(token, ':')) {
                token[strlen(token) - 1] = '\0';
                strcpy(labels[label_count].name, token);
                labels[label_count].address = instruction_count;
                label_count++;
            } else {
                strcpy(instructions[instruction_count].opcode, token);
                int op_count = 0;
                while ((token = strtok(NULL, ", \t\n")) && op_count < 3) {
                    strcpy(instructions[instruction_count].operands[op_count], token);
                    op_count++;
                }
                instruction_count++;
            }
        }
    }
    fclose(file);
}

void runSimulator() {
    char choice;
    int running = 1;

    while(running) {
        //clearScreem();
        displayRegisters();
        displayInstruction(pc);

        printf("\n=== Simulador MIPS ===\n");
        printf("p: Próxima instrução\n");
        printf("r: Rodar todo o código\n");
        printf("s: Sair\n");
        printf("Escolha: ");
        scanf("%c", &choice);
        getc(stdin);

        switch (choice) {
            case 'p':
                if(pc < instruction_count) {
                    executeInstruction(pc);
                    pc++;
                }
                else running = 0;
                break;
            
            case 'r': 
                while(pc < instruction_count) {
                    executeInstruction(pc);
                    pc++;
                }
                break;
            
            case 's':
                running = 0;
                break;

            default:
                printf("\nErro.");
                printf("\nDigite uma opção válida.");
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Erro. Passe o arquivo mips a ser aberto");
        return EXIT_FAILURE;
    }

    // Inicializando registradores
    memset(registers, 0, sizeof(registers));
    memset(memory, 0, sizeof(memory));

    // Abrindo arquivo

    loadProgram(argv[1]);

    runSimulator();
    

    return EXIT_SUCCESS;
}