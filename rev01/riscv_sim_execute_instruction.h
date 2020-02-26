/*
 * CMPE 110
 * PA 1
 * Group 54
 * Header file for riscv_sim_execute_single_instruction()
 * Filename: riscv_sim_execute_instruction.h
 *
 */

#include <stdbool.h>
#include <stdint.h>

char decode(uint32_t instruction);
void i_type(uint32_t instruction, const uint64_t pc, uint64_t *new_pc);
void r_type(uint32_t instruction, const uint64_t pc, uint64_t *new_pc);
void sb_type(uint32_t instruction, const uint64_t pc, uint64_t *new_pc);
void uj_type(uint32_t instruction, const uint64_t pc, uint64_t *new_pc);
void u_type(uint32_t instructionv, const uint64_t pc, uint64_t *new_pc);
void s_type(uint32_t instruction, const uint64_t pc, uint64_t *new_pc);
void execute(uint32_t instruction, char type, const uint64_t pc, uint64_t *new_pc);
