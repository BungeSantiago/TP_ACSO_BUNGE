#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "shell.h"

typedef struct {
    const char *instruction_name;
    uint32_t opcode;
    void (*execute_function)(uint32_t instruction);
} InstructionInfo;

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
void execute_eor(uint32_t instruction);
void execute_orr(uint32_t instruction);
void execute_b(uint32_t instruction);
void execute_br(uint32_t instruction);
void execute_shift(uint32_t instruction);

void execute_stur(uint32_t instruction);
void execute_sturb(uint32_t instruction);
void execute_sturh(uint32_t instruction);
void execute_ldur(uint32_t instruction);
void execute_ldurh(uint32_t instruction);
void execute_ldurb(uint32_t instruction);
void execute_movz(uint32_t instruction);

void execute_cbz(uint32_t instruction);
void execute_cbnz(uint32_t instruction);

InstructionInfo instruction_table[] = {
    {"ADDS (EXTENDED REGISTER)", 0b10101011000, execute_addsr},
    {"ADDS (IMMEDIATE)", 0b10110001, execute_addsi},
    {"SUBS (EXTENDED REGISTER)", 0b11101011000, execute_subser},
    {"SUBS (IMMEDIATE)", 0b11110001, execute_subsi},
    {"HLT", 0b11010100010, NULL},
    {"CMP (EXTENDED REGISTER)", 0b11101011001, execute_cmper},
    {"CMP (IMMEDIATE)", 0b11110001, execute_cmpi},
    {"ANDS", 0b11101010, execute_ands},
    {"EORSR", 0b11001010, execute_eor},
    {"ORRSR", 0b10101010, execute_orr},
    {"SHIFT", 0b110100110, execute_shift},
    {"STUR", 0b11111000000, execute_stur},
    {"STURB", 0b00111000000, execute_sturb},
    {"LDUR", 0b11111000010, execute_ldur},
    {"LDURB", 0b00111000010, execute_ldurb},
    {"MOVZ", 0b110100101, execute_movz},
    {"MUL", 0b10011011000, execute_mul},
    {"ADDER", 0b10001011000, execute_adder},
    {"ADDI", 0b10010001, execute_addi},
    {"B", 0b000101, execute_b},
    {"CBZ", 0b10110100, execute_cbz},
    {"CBNZ", 0b10110101, execute_cbnz},
    {"BR", 0b1101011000011111000000, execute_br}, //test
    {"BCOND", 0b01010100, execute_bcond},
    {"STURH", 0b01111000000, execute_sturh},
    {"LDURH", 0b0111000010, execute_ldurh},
};

#define INSTRUCTION_COUNT (sizeof(instruction_table) / sizeof(InstructionInfo))

uint32_t mask11 = 0b11111111111 ;
uint32_t mask8 = 0b11111111000000000000000000000000;
uint32_t mask6 = 0b11111100000000000000000000000000;
uint32_t mask22 = 0b11111111111111111111110000000000;
uint32_t mask9 = 0b11111111100000000000000000000000;

uint32_t cond_EQ = 0b0000;
uint32_t cond_NE = 0b0001;
uint32_t cond_GT = 0b1100;
uint32_t cond_LT = 0b1011;
uint32_t cond_GE = 0b1010;
uint32_t cond_LE = 0b1101;

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
//m,antener el ultimo registor en 0

uint64_t read_register(int reg) {
    return (reg == 31) ? 0 : CURRENT_STATE.REGS[reg];
}
void write_register(int reg, uint64_t value) {
    if (reg != 31) {
        NEXT_STATE.REGS[reg] = value;
    }
}
void execute_cond_eq(uint32_t instruction) {
    int imm19 = (instruction >> 5) & val19;
    int cond = instruction & val4; 
    int32_t offset = (imm19 << 2);  

    if (imm19 & (1 << 18)) {
        offset |= 0b11111111111110000000000000000000; 
    }

    if (CURRENT_STATE.FLAG_Z == 1) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void execute_cond_ne(uint32_t instruction) {
    int imm19 = (instruction >> 5) & 0b1111111111111111111; 
    int32_t offset = imm19 << 2; 

    if (imm19 & (1 << 18)) {
        offset |= 0b11111111111110000000000000000000;
    }

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
        offset |= 0xFFFFE000;
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
    
    int rd = (instruction & val5);
    int rn = (instruction & (val5 << 5)) >> 5;
    int rm = (instruction & (val5 << 16)) >> 16;

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] + CURRENT_STATE.REGS[rm];

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

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] - CURRENT_STATE.REGS[rm];

    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[rd] < 0) ? 1 : 0;
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[rd] == 0) ? 1 : 0;

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
    int rm = (instruction & (val5 << 16)) >> 16;

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] & CURRENT_STATE.REGS[rm];

    NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[rd] < 0) ? 1 : 0;
    NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[rd] == 0) ? 1 : 0;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_eor(uint32_t instruction) {
    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int rm = (instruction & (val5 << 16)) >> 16;

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] ^ CURRENT_STATE.REGS[rm];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_orr(uint32_t instruction) {
    int rd = instruction & val5;
    int rn = (instruction & (val5 << 5)) >> 5;
    int rm = (instruction & (val5 << 16)) >> 16;

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] | CURRENT_STATE.REGS[rm];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_b(uint32_t instruction) {
    int32_t imm26 = instruction & val26;

    if (imm26 & (1 << 25)) {
        imm26 |= ~val26; 
    }

    int64_t offset = ((int64_t)imm26) << 2; 
    NEXT_STATE.PC = CURRENT_STATE.PC + offset;
}
void execute_br(uint32_t instruction) {
    int rn = (instruction & (val5 << 5)) >> 5;

    NEXT_STATE.PC = CURRENT_STATE.REGS[rn];
}
void execute_shift(uint32_t instruction) {
    uint32_t Rd = instruction & val5;           
    uint32_t Rn = (instruction >> 5) & val5;   
    uint32_t immr = (instruction >> 16) & val6;    
    uint32_t imms = (instruction >> 10) & val6;
    
    uint64_t source = CURRENT_STATE.REGS[Rn];
    uint64_t result;
    
    if (imms != 63) {
        uint32_t shift = 63 - imms;
        result = (shift >= 64) ? 0 : (source << shift);
    } else {
        uint32_t shift = immr;
        result = (shift >= 64) ? 0 : (source >> shift);
    }
    
    NEXT_STATE.REGS[Rd] = result;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_stur(uint32_t instruction) {
    uint32_t rt = instruction & val5;
    uint32_t rn = (instruction & (val5 << 5)) >> 5;
    int32_t imm9 = (instruction & (val9 << 12)) >> 12;

    if (imm9 & (1 << 8)) {
        imm9 |= ~val9;
    }
    uint64_t address = CURRENT_STATE.REGS[rn] + imm9;
    if (address < 0x10000000 || address >= 0x10100000) {
        printf("La dirección está fuera de rango: 0x%llx\n", address);
        return;
    }
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

    uint32_t low = mem_read_32(address);
    uint32_t high = mem_read_32(address + 4);
        
    NEXT_STATE.REGS[rt] = ((uint64_t) high << 32) | low;;
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
    uint32_t rd = instruction & val5;
    uint32_t imm16 = (instruction & (val16 << 5)) >> 5;
    uint32_t hw = (instruction & (0b11 << 21)) >> 21;

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
    uint32_t rd = instruction & val5;
    uint32_t rn = (instruction & (val5 << 5)) >> 5;
    uint32_t rm = (instruction & (val5 << 16)) >> 16;

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] * CURRENT_STATE.REGS[rm];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void execute_cbz(uint32_t instruction) {
    int rt = instruction & val5;
    int imm19 = (instruction >> 5) & val19;
    int32_t offset = (imm19 << 2);  
    
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
    int64_t offset = ((int32_t)(imm19 << 13)) >> 11;

    if (CURRENT_STATE.REGS[rt] != 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void execute_bcond(uint32_t instruction){
    int cond = instruction & val4;
    if (cond == cond_EQ){
        execute_cond_eq(instruction);
    }
    else if (cond == cond_NE){
        execute_cond_ne(instruction);
    }
    else if (cond == cond_GT){
        execute_cond_gt(instruction);
    }
    else if (cond == cond_LT){
        execute_cond_lt(instruction);
    }
    else if (cond == cond_GE){
        execute_cond_ge(instruction);
    }
    else if (cond == cond_LE){
        execute_cond_le(instruction);
    }
}
void process_instruction() {
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    printf("instruction: %x\n", instruction);

    // Extract opcodes
    uint32_t opcode11 = (instruction >> 21) & 0b11111111111; // 11-bit opcode
    uint32_t opcode8 = (instruction >> 24) & 0b11111111;     // 8-bit opcode
    uint32_t opcode6 = (instruction >> 26) & 0b111111;       // 6-bit opcode
    uint32_t opcode22 = (instruction >> 10) & 0b1111111111111111111111; // 22-bit opcode
    uint32_t opcode9 = (instruction >> 23) & 0b111111111;    // 9-bit opcode

    // Search the instruction table
    for (int i = 0; i < INSTRUCTION_COUNT; i++) {
        if (instruction_table[i].opcode == opcode11 || 
            instruction_table[i].opcode == opcode8 || 
            instruction_table[i].opcode == opcode6 || 
            instruction_table[i].opcode == opcode22 || 
            instruction_table[i].opcode == opcode9) {
            
            printf("Executing instruction: %s\n", instruction_table[i].instruction_name);

            // Execute the function if it exists
            if (instruction_table[i].execute_function != NULL) {
                instruction_table[i].execute_function(instruction);
            } else if (strcmp(instruction_table[i].instruction_name, "HLT") == 0) {
                printf("HLT\n");
                RUN_BIT = FALSE;
            }
            return;
        }
    }

    // If no match is found
    printf("Invalid instruction\n");
    NEXT_STATE.PC = CURRENT_STATE.PC + 4; // Skip invalid instruction
}