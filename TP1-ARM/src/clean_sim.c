#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "shell.h"

void execute_cond_eq(uint32_t instruction);
void execute_cond_ne(uint32_t instruction);
void execute_cond_gt(uint32_t instruction);
void execute_cond_lt(uint32_t instruction);
void execute_cond_ge(uint32_t instruction);
void execute_cond_le(uint32_t instruction);
void execute_bcond(uint32_t instruction);

void execute_addsr(uint32_t instruction);
void execute_addsi(uint32_t instruction);
void execute_subser(uint32_t instruction);
void execute_subsi(uint32_t instruction);
void execute_adder(uint32_t instruction);
void execute_addi(uint32_t instruction);
void execute_mul(uint32_t instruction);

void execute_cmper(uint32_t instruction);
void execute_cmpi(uint32_t instruction);
void execute_ands(uint32_t instruction);
void execute_eorsr(uint32_t instruction);
void execute_orr(uint32_t instruction);
void execute_b(uint32_t instruction);
void execute_br(uint32_t instruction);
void execute_lsli(uint32_t instruction);
void execute_lsri(uint32_t instruction);

void execute_stur(uint32_t instruction);
void execute_sturb(uint32_t instruction);
void execute_sturh(uint32_t instruction);
void execute_ldur(uint32_t instruction);
void execute_ldurh(uint32_t instruction);
void execute_ldurb(uint32_t instruction);
void execute_movz(uint32_t instruction);

void execute_cbz(uint32_t instruction);
void execute_cbnz(uint32_t instruction);

// Define la estructura para el diccionario
typedef struct {
    const char *instruction_name;
    uint32_t opcode;
    void (*execute_function)(uint32_t instruction);
} InstructionInfo;

InstructionInfo instruction_table[] = {
    {"ADDSR", 0b10101011000, execute_addsr},
    {"ADDSI", 0b10110001, execute_addsi},
    {"SUBSER", 0b11101011001, execute_subser},
    {"SUBSI", 0b11110001, execute_subsi},
    // Agrega más instrucciones aquí
};

uint32_t mask11 = 0b11111111111 ;
uint32_t mask8 = 0b11111111000000000000000000000000;
uint32_t mask6 = 0b11111100000000000000000000000000;
uint32_t mask22 = 0b11111111111111111111110000000000;
uint32_t mask9 = 0b11111111100000000000000000000000;

/*
uint32_t opcode_CMPER = 0b11101011001;
uint32_t opcode_CMPI = 0b11110001;
uint32_t opcode_ANDS = 0b11101010;
uint32_t opcode_EORSR = 0b11001010;
uint32_t opcode_ORRSR = 0b10101010;
uint32_t opcode_B = 0b000101;
uint32_t opcode_BR = 0b1101011000011111000000;
uint32_t opcode_bcond = 0b01010100;
uint32_t opcode_LSLI = 0b110100110;
uint32_t opcode_LSRI = 0b110100110;
uint32_t opcode_STUR = 0b11111000000;
uint32_t opcode_STURB = 0b00111000000;
uint32_t opcode_STURH = 0b01111000000;
uint32_t opcode_LDUR = 0b11111000010;
uint32_t opcode_LDURH = 0b0111000010;
uint32_t opcode_LDURB = 0b00111000010;
uint32_t opcode_MOVZ = 0b110100101;
uint32_t opcode_ADDER = 0b10001011001;
uint32_t opcode_ADDI = 0b10010001;
uint32_t opcode_MUL = 0b10011011000;
uint32_t opcode_CBZ = 0b10110100; 
uint32_t opcode_CBNZ = 0b101110;

uint32_t cond_EQ = 0b0000;
uint32_t cond_NE = 0b0001;
uint32_t cond_GT = 0b1100;
uint32_t cond_LT = 0b1011;
uint32_t cond_GE = 0b1010;
uint32_t cond_LE = 0b1101;
*/

uint32_t val5 = 0b11111;
uint32_t val11 = 0b11111111111;
uint32_t val3 = 0b111;
uint32_t val4 = 0b1111;
uint32_t val12 = 0b111111111111;
uint32_t val1 = 0b1;
uint32_t val6 = 0b111111;
uint32_t val2 = 0b11;
uint32_t val26 = 0b11111111111111111111111111;
uint32_t val9 = 0b111111111;
uint32_t val8 = 0b11111111;
uint32_t val16 = 0b1111111111111111;
uint32_t val19 = 0b1111111111111111111;


void execute_cond_eq(uint32_t instruction) {
    int imm19 = (instruction >> 5) & val19;
    int cond = instruction & val4;  // Aunque para BEQ cond siempre es 0b0000
    int32_t offset = (imm19 << 2);  // Se multiplica por 4

    // Extensión de signo si el bit 18 (más significativo de imm19) es 1
    if (imm19 & (1 << 18)) {
        offset |= 0b11111111111110000000000000000000;  // Extiende a 32 bits con 1s
    }

    // Condición para BEQ: salta si Z == 1
    if (CURRENT_STATE.FLAG_Z == 1) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void execute_cond_ne(uint32_t instruction) {
    int imm19 = (instruction >> 5) & 0b1111111111111111111;  // Extrae los bits 23:5
    int32_t offset = imm19 << 2;  // Multiplica por 4 (inserta dos ceros a la derecha)

    // Sign-extend si el bit 18 está en 1
    if (imm19 & (1 << 18)) {
        offset |= 0b11111111111110000000000000000000;  // Rellena los bits altos con 1s
    }

    // Condición para BNE: salta si Z == 0
    if (CURRENT_STATE.FLAG_Z == 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void execute_cond_gt(uint32_t instruction) {
    int imm19 = (instruction >> 5) & 0b1111111111111111111;
    int32_t offset = (imm19 << 2);
    if (imm19 & (1 << 18)) {
        offset |= 0xFFFFE000;  // extensión de signo
    }

    if ((CURRENT_STATE.FLAG_Z == 0) && (CURRENT_STATE.FLAG_N == 0)) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void execute_cond_lt(uint32_t instruction) {
    int imm19 = (instruction >> 5) & 0b1111111111111111111;
    int32_t offset = (imm19 << 2);
    if (imm19 & (1 << 18)) {
        offset |= 0xFFFFE000;
    }

    if (CURRENT_STATE.FLAG_N == 1) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void execute_cond_ge(uint32_t instruction) {
    int imm19 = (instruction >> 5) & 0b1111111111111111111;
    int32_t offset = (imm19 << 2);
    if (imm19 & (1 << 18)) {
        offset |= 0xFFFFE000;
    }

    if (CURRENT_STATE.FLAG_N == 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void execute_cond_le(uint32_t instruction) {
    int imm19 = (instruction >> 5) & 0b1111111111111111111;
    int32_t offset = (imm19 << 2);
    if (imm19 & (1 << 18)) {
        offset |= 0xFFFFE000;
    }

    if ((CURRENT_STATE.FLAG_Z == 1) || (CURRENT_STATE.FLAG_N == 1)) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void execute_addsr(uint32_t instruction) {
    printf("Executing ADDSR\n");
    
    int rd = (instruction & val5);
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm3 = (instruction & (val3 << 10)) >> 10;
    int option = (instruction & (val3 << 13) >> 13);
    int rm = (instruction & (val5 << 16)) >> 16;

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] + CURRENT_STATE.REGS[rm];

    // Actualizar los flags
    if (NEXT_STATE.REGS[rd] < 0) {
        NEXT_STATE.FLAG_N = 1;
    } else {
        NEXT_STATE.FLAG_N = 0;
    }

    if (NEXT_STATE.REGS[rd] == 0) {
        NEXT_STATE.FLAG_Z = 1;
    } else {
        NEXT_STATE.FLAG_Z = 0;
    }

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_addsi(uint32_t instruction) {
    printf("Executing ADDSI\n");

    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm12 = (instruction & (val12 << 10)) >> 10;
    int sh = (instruction & (val1 << 22)) >> 22;

    uint64_t imm_value = sh ? (imm12 << 12) : imm12;

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] + imm_value;

    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[rd] < 0) ? 1 : 0;
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[rd] == 0) ? 1 : 0;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_subser(uint32_t instruction) {

    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm3 = (instruction & (val3 << 10)) >> 10;
    int option = (instruction & (val3 << 13)) >> 13;
    int rm = (instruction & (val5 << 16)) >> 16;

    // Realizar la resta entre registros extendidos
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] - CURRENT_STATE.REGS[rm];

    // Actualizar flags
    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[rd] < 0) ? 1 : 0;
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[rd] == 0) ? 1 : 0;

    // Avanzar PC
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_subsi(uint32_t instruction) {

    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm12 = (instruction & (val12 << 10)) >> 10;
    int shift = (instruction & (val1 << 22)) >> 22;

    uint64_t imm_value = shift ? (imm12 << 12) : imm12;

    int64_t result = CURRENT_STATE.REGS[rn] - imm_value;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_cmper(uint32_t instruction) {
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm3 = (instruction & (val3 << 10)) >> 10;
    int option = (instruction & (val3 << 13)) >> 13;
    int rm = (instruction & (val5 << 16)) >> 16;

    int64_t result = CURRENT_STATE.REGS[rn] - CURRENT_STATE.REGS[rm];

    NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_cmpi(uint32_t instruction) {
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm12 = (instruction & (val12 << 10)) >> 10;
    int shift = (instruction & (val1 << 22)) >> 22;

    uint64_t imm_value = shift ? (imm12 << 12) : imm12;

    int64_t result = CURRENT_STATE.REGS[rn] - imm_value;

    NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_ands(uint32_t instruction) {

    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm6 = (instruction & (val6 << 10)) >> 10;
    int rm = (instruction & (val5 << 16)) >> 16;
    int shift = (instruction & (val2 << 22)) >> 22;

    uint64_t operand2 = CURRENT_STATE.REGS[rm];
    if (shift == 0b00) {
        operand2 = operand2 << imm6; // LSL
    } else if (shift == 0b01) {
        operand2 = operand2 >> imm6; // LSR
    }

    int64_t result = CURRENT_STATE.REGS[rn] & operand2;
    NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_eorsr(uint32_t instruction) {
    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm6 = (instruction & (val6 << 10)) >> 10;
    int rm = (instruction & (val5 << 16)) >> 16;
    int shift = (instruction & (val2 << 22)) >> 22;

    uint64_t operand2 = CURRENT_STATE.REGS[rm];
    if (shift == 0b00) {
        operand2 = operand2 << imm6; // LSL
    } else if (shift == 0b01) {
        operand2 = operand2 >> imm6; // LSR
    }

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] ^ operand2;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_orr(uint32_t instruction) {
    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm6 = (instruction & (val6 << 10)) >> 10;
    int rm = (instruction & (val5 << 16)) >> 16;
    int shift = (instruction & (val2 << 22)) >> 22;

    uint64_t operand2 = CURRENT_STATE.REGS[rm];
    if (shift == 0b00) {
        operand2 = operand2 << imm6; // LSL
    } else if (shift == 0b01) {
        operand2 = operand2 >> imm6; // LSR
    }

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] | operand2;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_b(uint32_t instruction) {
    int32_t imm26 = instruction & val26;

    // Sign-extend a 32 bits si el bit 25 está en 1
    if (imm26 & (1 << 25)) {
        imm26 |= ~val26; // completa con 1s a la izquierda
    }

    int64_t offset = ((int64_t)imm26) << 2; // multiplicar por 4
    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
}
void execute_br(uint32_t instruction) {
    int rn = (instruction & (val5 << 5)) >> 5;

    NEXT_STATE.PC = CURRENT_STATE.REGS[rn];
}
void execute_lsli(uint32_t instruction) {
    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int immr = (instruction & (val6 << 16)) >> 16;

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] << immr;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_lsri(uint32_t instruction) {
    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int immr = (instruction & (val6 << 16)) >> 16;

    NEXT_STATE.REGS[rd] = (uint64_t)CURRENT_STATE.REGS[rn] >> immr;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_stur(uint32_t instruction) {
    int rt = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm9 = (instruction & (val9 << 12)) >> 12;

    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    mem_write_32(address, (uint32_t)CURRENT_STATE.REGS[rt]);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_sturb(uint32_t instruction) {
    int rt = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm9 = (instruction & (val9 << 12)) >> 12;

    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    uint8_t byte_value = (uint8_t)(CURRENT_STATE.REGS[rt] & 0xFF);
    mem_write_32(address, byte_value);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_sturh(uint32_t instruction) {
    int val5 = 0b11111;
    int val9 = 0b111111111;

    int rt = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm9 = (instruction & (val9 << 12)) >> 12;

    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    uint16_t halfword_value = (uint16_t)(CURRENT_STATE.REGS[rt] & 0xFFFF);
    mem_write_32(address, halfword_value);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_ldur(uint32_t instruction) {
    int rt = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm9 = (instruction & (val9 << 12)) >> 12;

    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    uint32_t word_value = mem_read_32(address);
    NEXT_STATE.REGS[rt] = (uint64_t)word_value;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_ldurh(uint32_t instruction) {
    int rt = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm9 = (instruction & (val9 << 12)) >> 12;

    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    uint16_t halfword_value = mem_read_32(address) & 0xFFFF;
    NEXT_STATE.REGS[rt] = (uint64_t)halfword_value;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_ldurb(uint32_t instruction) {
    int rt = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm9 = (instruction & (val9 << 12)) >> 12;

    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    uint8_t byte_value = mem_read_32(address) & 0xFF;
    NEXT_STATE.REGS[rt] = (uint64_t)byte_value;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_movz(uint32_t instruction) {
    int rd = instruction & val5;
    int imm16 = (instruction & (val16 << 5)) >> 5;
    int hw = (instruction & (0b11 << 21)) >> 21;

    if (hw == 0) {
        NEXT_STATE.REGS[rd] = (uint64_t)imm16;
    }

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_adder(uint32_t instruction) {
    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm3 = (instruction & (val3 << 10)) >> 10;
    int option = (instruction & (val3 << 13)) >> 13;
    int rm = (instruction & (val5 << 16)) >> 16;

    uint64_t shifted_rm = CURRENT_STATE.REGS[rm] << imm3;
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] + shifted_rm;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_addi(uint32_t instruction) {
    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int imm12 = (instruction & (val12 << 10)) >> 10;
    int shift = (instruction & (val1 << 22)) >> 22;

    uint64_t imm_value = shift ? (imm12 << 12) : imm12;

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] + imm_value;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_mul(uint32_t instruction) {
    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int rm = (instruction & (val5 << 16)) >> 16;

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] * CURRENT_STATE.REGS[rm];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_cbz(uint32_t instruction) {
    int rt = instruction & val5;
    int imm19 = (instruction >> 5) & val19;
    int32_t offset = (imm19 << 2);  // se multiplica por 4 (offset de palabra)
    
    // Sign-extend si el bit 18 está en 1
    if (imm19 & (1 << 18)) {
        offset |= 0xFFF80000;
    }

    if (CURRENT_STATE.REGS[rt] == 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void execute_cbnz(uint32_t instruction) {
    int rt = instruction & val5;
    int imm19 = (instruction >> 5) & val19;
    int64_t offset = ((int32_t)(imm19 << 13)) >> 11; // sign-extend de 19 bits y multiplicar por 4

    if (CURRENT_STATE.REGS[rt] != 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
// void execute_bcond(uint32_t instruction){
//     int cond = instruction & val4;
//     if (cond == cond_EQ){
//         execute_cond_eq(instruction);
//     }
//     if (cond == cond_NE){
//         execute_cond_ne(instruction);
//     }
//     if (cond == cond_GT){
//         execute_cond_gt(instruction);
//     }
//     if (cond == cond_LT){
//         execute_cond_lt(instruction);
//     }
//     if (cond == cond_GE){
//         execute_cond_ge(instruction);
//     }
//     if (cond == cond_LE){
//         execute_cond_le(instruction);
//     }
// }

// Número de instrucciones en el diccionario
#define INSTRUCTION_COUNT (sizeof(instruction_table) / sizeof(instruction_table[0]))

// Función para buscar y ejecutar una instrucción
void process_instruction(uint32_t instruction) {
    uint32_t opcode11 = (instruction >> 21) & 0b11111111111; // Extrae el opcode de 11 bits
    uint32_t opcode8 = (instruction >> 24) & 0b11111111;     // Extrae el opcode de 8 bits

    for (int i = 0; i < INSTRUCTION_COUNT; i++) {
        if (instruction_table[i].opcode == opcode11 || instruction_table[i].opcode == opcode8) {
            printf("Executing instruction: %s\n", instruction_table[i].instruction_name);
            instruction_table[i].execute_function(instruction);
            return;
        }
    }

    printf("Invalid instruction\n");
}