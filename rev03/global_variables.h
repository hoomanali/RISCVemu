#include "stddef.h"
#include <string.h>
#include <assert.h>

#define I_CACHE_SIZE 512
#define D_CACHE_SIZE 2048

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
 * Address size:        64 bits
 * Tag size:            51 bits
 * Index size:          9 bits
 * Offset size:         4 bits
 */
struct i_cache_block {
    bool valid_bit;
    uint64_t tag;
    uint16_t index;
    uint8_t offset;
    uint64_t data;
};

/*
 * Data cache block
 *
 * Total cache size:    16 KiB (16384 bytes)
 * Block size:          8 bytes (2 words)
 * Block count:         2048 (2^11) blocks
 * Type:                Direct-mapped
 * Address size:        64 bits
 * Tag size:            50 bits
 * Index size:          11 bits
 * Offset size:         3 bits
 */
struct d_cache_block {
    bool valid_bit;
    uint64_t tag;
    uint16_t index;
    uint8_t offset;
    uint32_t data;
};

// Instruction and Data cache arrays
struct i_cache_block i_cache[I_CACHE_SIZE];
struct d_cache_block d_cache[D_CACHE_SIZE];

