#include "stddef.h"
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define I_CACHE_SIZE 512
#define D_CACHE_SIZE 2048
#define I_TLB_SIZE 8
#define D_TLB_SIZE 8
#define ROOT_PTE_SIZE 1024
#define SECOND_PTE_SIZE 1024
#define STAGE_FETCH 'F'
#define STAGE_MEM 'M'

#pragma once

enum instruction_type {
	// r_types:
	add,sub,sll,slt,sltu,xor1,srl,sra,or1,and1,
	mul,mulh,mulhsu,mulhu,div,divu,rem,remu,
	addw,subw,sllw,srlw,sraw,mulw,divw,divuw,remw,remuw,
	// i_types:
	addi,slli,slti,sltiu,xori,srli,srai,ori,andi,
	lb,lh,lw,ld,lbu,lhu,lwu,
	addiw,slliw,srliw,sraiw,
	// jalr:
	jalr,
	// s_types:
	sb,sh,sw,sd,
	// branch types:
	beq,bne,blt,bge,bltu,bgeu,
	// u-types:
	lui,auipc,jal,
	invalid
};

// Branch target buffer (BTB) array
uint64_t btb[32];

/*
 * Instruction cache block
 *
 * Total cache size:    8 KiB (8192 bytes)
 * Block size:          16 bytes (4 words)
 * Block count:         512 (2^9) blocks
 * Type:                Direct-mapped
 * Address size:        32 bits
 * Tag size:            11 bits
 * Index size:          9 bits
 * Offset size:         12 bits
 */
struct i_cache_block {
    bool valid_bit;
    uint16_t tag;
    uint16_t index;
    uint16_t offset;
    uint64_t data;
};

/*
 * Data cache block
 *
 * Total cache size:    16 KiB (16384 bytes)
 * Block size:          8 bytes (2 words)
 * Block count:         2048 (2^11) blocks
 * Type:                Direct-mapped
 * Address size:        32 bits
 * Tag size:            9 bits
 * Index size:          11 bits
 * Offset size:         12 bits
 */
struct d_cache_block {
    bool valid_bit;
    uint16_t tag;
    uint16_t index;
    uint16_t offset;
    uint64_t data;
};

/*
 * TLB entry
 *
 * Generic TLB entry struct for instruction and data TLB
 *
 * Entry count:         8
 * Type:                Direct-mapped
 * Address size:        32 bits
 * Tag size:            11 bits
 * Index size:          9 bits
 * Physical frame:      32 bits
 * Offset size:         12 bits
 */
struct TLB_entry {
	bool valid;
	bool dirty;
	uint32_t vpn_tag;
	uint32_t vpn_index;
	uint32_t physical_frame_number; 
	uint16_t page_offset;
};

/* PTE Entry
 *
 * Entry Count: 2
 */
struct PTE_entry {
	bool valid;
	uint32_t pointer;
};

// Instruction and Data cache arrays
struct i_cache_block i_cache[I_CACHE_SIZE];
struct d_cache_block d_cache[D_CACHE_SIZE];

// Instruction and Data TLB arrays
struct TLB_entry I_TLB[I_TLB_SIZE];
struct TLB_entry D_TLB[D_TLB_SIZE];

// PTEs
struct PTE_entry root_PTE[ROOT_PTE_SIZE];
struct PTE_entry second_PTE[SECOND_PTE_SIZE];
