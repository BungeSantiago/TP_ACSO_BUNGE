#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <shell.h>


extern CPU_State;

int mask11 = 0b1111111111100000000000000000;
int mask8 = 0b11111111000000000000000000000000;
int mask6 = 0b11111100000000000000000000000000;
int mask22 = 0b11111111111111111111110000000000;
int mask9 = 0b11111111100000000000000000000000;

int opcode_ADDSR = 0b10101011001;
int opcode_ADDSI = 0b10110001;
int opcode_SUBSER = 0b11101011001;
int opcode_SUBSI = 0b11110001;
int opcode_HLT = 0b11010100010;
int opcode_CMPER = 0b11101011001;
int opcode_CMPI = 0b11110001;
int opcode_ANDS = 0b11101010;
int opcode_EORSR = 0b11001010;
int opcode_ORRSR = 0b10101010;
int opcode_B = 0b000101;
int opcode_BR = 0b1101011000011111000000;
int opcode_bcond = 0b01010100;
int opcode_LSLI = 0b110100110;
int opcode_LSRI = 0b110100110;
int opcode_STUR;
int opcode_STURB;
int opcode_STURH = 0b01111000000;
int opcode_LDURH;
int opcode_LDURB = 0b00111000010;
int opcode_MOVZ = 0b110100101;
int opcode_ADDER = 0b10001011001;
int opcode_ADDI = 0b10010001;
int opcode_MUL = 0b10011011000;
int opcode_CBZ = 0b101100; // tengo mis dudas aca.
int opcode_CBNZ = 0b101110;

int cond_EQ = 0b0000;
int cond_NE = 0b0001;
int cond_GT = 0b1100;
int cond_LT = 0b1011;
int cond_GE = 0b1010;
int cond_LE = 0b1101;

int val5 = 0b11111;
int val3 = 0b111;
int val4 = 0b1111;
int val12 = 0b111111111111;
int val1 = 0b1;


void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. 
     * */
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    int opcode11 = instruction & mask11;
    opcode11 = opcode11 >> 21;
    int opcode8 = instruction & mask8;
    opcode8 = opcode8 >> 24;
    int opcode6 = instruction & mask6;
    opcode6 = opcode6 >> 26;
    int opcode22 = instruction & mask22;
    opcode22 = opcode22 >> 10;
    int opcode9 = instruction & mask9;
    opcode9 = opcode9 >> 23;



    if (opcode_ADDSR == opcode11){
        execute_addsr(instruction);
    }
    if (opcode_ADDSI == opcode8){
        execute_addsi(instruction);
    }
    if(opcode_SUBSER == opcode11){
        execute_subser(instruction);
    }
    if (opcode_SUBSI == opcode8){
        execute_subsi(instruction);
    }
    if (opcode_HLT == opcode11){
        printf("HLT\n");
        RUN_BIT = FALSE;
    }
    if (opcode_CMPER == opcode11){
        execute_cmper(instruction);
    }
    if (opcode_CMPI == opcode8){
        execute_cmpi(instruction);
    }
    if (opcode_ANDS == opcode8){
        execute_ands(instruction);
    }
    if (opcode_EORSR == opcode8){
        execute_eor(instruction);
    }
    if (opcode_ORRSR == opcode8){
        execute_orr(instruction);
    }
    if (opcode_B == opcode6){
        execute_b(instruction);
    }
    if (opcode_BR == opcode22){
        execute_br(instruction);
    }
    if (opcode_bcond == opcode8){
        execute_bcond(instruction);
    }
    if (opcode_LSLI == opcode9){
        execute_lsl(instruction);
    }
    if (opcode_LSRI == opcode9){
        execute_lsr(instruction);
    }
    if (opcode_STUR == opcode11){
        execute_stur(instruction);
    }
    if (opcode_STURB == opcode11){
        execute_sturb(instruction);
    }
    if (opcode_STURH == opcode11){
        execute_sturh(instruction);
    }
    if (opcode_LDURH == opcode11){
        execute_ldurh(instruction);
    }
    if (opcode_LDURB == opcode11){
        execute_ldurb(instruction);
    }
    if (opcode_MOVZ == opcode9){
        execute_movz(instruction);
    }
    if (opcode_ADDER == opcode11){
        execute_adder(instruction);
    }
    if (opcode_ADDI == opcode8){
        execute_addi(instruction);
    }
    if (opcode_MUL == opcode11){
        execute_mul(instruction);
    }
    if (opcode_CBZ == opcode6){
        execute_cbz(instruction);
    }
    if (opcode_CBNZ == opcode6){
        execute_cbnz(instruction);
    }

    else{
        printf("Invalid instruction\n");
    }
    
}


// Función para ejecutar ADDS (suma con flags)
void execute_addsr(uint32_t instruction) {
    
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


void execute_bcond(uint32_t instruction){
    int cond = instruction & val4;
    if (cond = cond_EQ){
        execute_cond_eq(instruction);
    }
    if (cond = cond_NE){
        execute_cond_ne(instruction);
    }
    if (cond = cond_GT){
        execute_cond_gt(instruction);
    }
    if (cond = cond_LT){
        execute_cond_lt(instruction);
    }
    if (cond = cond_GE){
        execute_cond_ge(instruction);
    }
    if (cond = cond_LE){
        execute_cond_le(instruction);
    }
}
