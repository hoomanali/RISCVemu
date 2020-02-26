/*
 * External routines for use in CMPE 110, Fall 2018
 *
 * (c) 2018 Ethan L. Miller
 * Redistribution not permitted without explicit permission from the copyright holder.
 *
 */

/*
 * IMPORTANT: rename this file to riscv_pipeline_registers.h
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

/*
 * These pipeline stage registers are loaded at the end of the cycle with whatever
 * values were filled in by the relevant pipeline stage.
 *
 * Add fields to these stage registers for whatever values you'd like to have passed from
 * one stage to the next.  You may have as many values as you like in each stage.  However,
 * this is the ONLY information that may be passed from one stage to stages in the next
 * cycle.
 */


struct stage_reg_d {
    uint64_t    pc;
    uint32_t    instruction;
    uint32_t    stalled;
    bool        NOP;
    int         memoryread;
};

struct stage_reg_x {
    uint64_t    pc;
    uint32_t    instruction;
    uint8_t     flags;
    bool        NOP;
    uint32_t    class;
    uint16_t    rs1;
    uint16_t    rs2;
    uint64_t    rs1_val;
    uint64_t    rs2_val;
    uint16_t    rd;
    uint32_t    imm;
    int         stall;
    uint32_t    stall_instruction;
    uint64_t    stall_pc;
    
};

struct stage_reg_m {
    uint64_t    pc;
    uint32_t    instruction;
    bool        NOP;
    int         branch;     // 2 = branch happened, 1 = branch did not happen.
    uint32_t    class;
    uint16_t    rs1;
    uint16_t    rs2;
    uint64_t    rs1_val;
    uint64_t    rs2_val;
    uint16_t    rd;
    uint32_t    imm;
    uint64_t    value;
    bool        terminateWriteback;
    
    
};

struct stage_reg_w {
    uint32_t    instruction;
    uint64_t    pc;
    uint64_t    result;
    bool        NOP;
    uint32_t    class;
    uint16_t    rd;
    uint64_t    value;
    int         memoryread;
};
