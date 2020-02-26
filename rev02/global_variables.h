
#pragma once
#include "stddef.h"
#include <string.h>
#include <assert.h>
#define BHT_SIZE 16
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
	lui,auipc,jal	
};
// instruction_type is passed to everyone so only decode has to figure out
// branch history buffer for branch prediction
uint64_t btb[BHT_SIZE]; // btb  is stored like this: [destination:32bits][tag:28bits][low-order bits:4bits]

static uint8_t btb_size = 0; // Used to track btb size
bool bht[BHT_SIZE]; // extra credit

/*
 * btb_size is a global variable declared and defined in this file.
 * Some stages do not use btb_size.
 * It's for branching and prediction only. 
 * Use this function in files to suppress compiler warnings.
 */
static void suppress_btb_size_unused_warning() {
    (void) btb_size;
}

/*static void register_flush(void *st)
{
	memset(&st, 0, sizeof(st));
}*/

