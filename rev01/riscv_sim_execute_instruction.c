/*
 * CMPE 110
 * PA 1
 * Group 54
 * Source file for riscv_sim_execute_single_instruction()
 * Filename: riscv_sim_execute_instruction.c
 */

/**** INCLUDES ****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "riscv_sim_framework.h"
#include "riscv_sim_execute_instruction.h"

/**** DEFINES ****/
#define OPCODE_MASK 0x7F        // 0000 0000 0000 0000 0000 0000 0111 1111
#define RD_MASK 0xF80           // 0000 0000 0000 0000 0000 1111 1000 0000
#define FUNCT3_MASK 0x7000      // 0000 0000 0000 0000 0111 0000 0000 0000
#define RS1_MASK 0xF8000        // 0000 0000 0000 1111 1000 0000 0000 0000
#define RS2_MASK 0x0F00000      // 0000 0001 1111 0000 0000 0000 0000 0000
#define FUNCT7_MASK 0xFE000000  // 1111 1110 0000 0000 0000 0000 0000 0000

#define DEBUG // comment this out upon release

/****** MAIN SUBROUTINE TO WORK ON: *****/
void execute_single_instruction(const uint64_t pc, uint64_t *new_pc) 
{
    // int add = 0x00578833; //test instruction
    // int sub = 0x403102B3; //test instruction
    int i;
    new_pc[0] = pc + 4;
    memory_read(pc, &i, 4);
    char type = decode(i);
    execute(i, type, pc, new_pc);
    //new_pc[0] = pc + 4;
}

char decode(uint32_t instruction)
{
    int opcode = instruction & OPCODE_MASK;

    if ((opcode == 0x03) || (opcode == 0x13) || (opcode == 0x1B) || (opcode == 0x67)) { 
        return 'I';
    }
    else if ((opcode == 0x33) || (opcode == 0x3B)) {  
        return 'R';
    }
    else if (opcode == 0x23) {  
        return 'S';
    }
    else if ((opcode == 0x17) || (opcode == 0x37)) {  
        return 'U';
    }
    else if (opcode == 0x63) {  
        return 'B';  //SB-type
    }
    else if (opcode == 0x6F) {  
        return 'J';  //UJ-type
    }
    else {
        return 'Z';
    }
}

void execute(uint32_t instruction, char type, const uint64_t pc, uint64_t *new_pc) {
    if (type == 'I') {
        i_type(instruction, pc, new_pc);

    }
    else if (type == 'R') {
        r_type(instruction, pc, new_pc);


    }
    else if (type == 'S') {
        s_type(instruction, pc, new_pc);


    }
    else if (type == 'U') {
        u_type(instruction, pc, new_pc);
    }
    else if (type == 'B') {
        sb_type(instruction, pc, new_pc);


    }
    else if (type == 'J') {
        uj_type(instruction, pc, new_pc);
    }
}

void i_type(uint32_t instruction, const uint64_t pc, uint64_t *new_pc) {
    // 12 5 3 5 7
    int opcode = instruction & OPCODE_MASK;
    int rd = (instruction & RD_MASK) >> 7; 
    int funct3 = (instruction & FUNCT3_MASK) >> 12;
    int rs1 = (instruction & RS1_MASK) >> 15;
    int funct7 = (instruction & FUNCT7_MASK) >> 25;
    int32_t imm32 = (instruction & 0xFFF00000) >> 20;
    int64_t imm = (instruction & 0xFFF00000) >> 20;
    if (imm32 & 0x800) {
	    imm32 |= 0xFFFFF000;
    }
    if (imm & 0x800) {
	    imm |= 0xFFFFFFFFFFFFF800;
    }
    
    if (opcode == 0x03) {
        // reference page 35 of 145 in the riscv-spec-v2.2 pdf the professor provided us.
        if (funct3 == 0x00) {
            // execute lb
            // LB rd,offset(rs1)    Load byte   rd ← s8[rs1 + offset]
            // the offset here is imm.
            // the receiving address is rs1 + offset.
            // the target address is rd. insert that as the target address (i think).
            // size in bits is 8 (because it's lb)
            uint64_t rs1_val, rd_val, target_address, read_from_memory;
            int64_t target_s_address, rs1_s_val;
            register_read(rs1, rd, &rs1_val, &rd_val);
	    rs1_s_val = *(int64_t *) &rs1_val;
	    // address calculation
            target_s_address = rs1_s_val + imm;
	    *(int64_t * ) &target_address = target_s_address;
	    // memory reading
            memory_read(target_address, &read_from_memory, 1);
	    #ifdef DEBUG
		printf("LB called. imm = %ld, rs1 = %d, rd = %d, rs1 value: %ld,\r\nValue read from memory: %lu, target address: %lu\r\n", 
				   imm, rs1, rd, rs1_val, read_from_memory, target_address);
	    #endif
	    // writeback
            register_write(rd, read_from_memory);   
        }
        else if (funct3 == 0x01) {
            // execute lh
            // LH rd,offset(rs1)	Load Half	rd ← s16[rs1 + offset]
	    uint64_t rs1_val, rd_val, target_address, read_from_memory;
            int64_t target_s_address, rs1_s_val;
            register_read(rs1, rd, &rs1_val, &rd_val);
	    rs1_s_val = *(int64_t *) &rs1_val;
	    // address calculation
            target_s_address = rs1_s_val + imm;
	    *(int64_t * ) &target_address = target_s_address;
	    // memory reading
            memory_read(target_address, &read_from_memory, 2);
	    #ifdef DEBUG
		printf("LH called. imm = %ld, rs1 = %d, rd = %d, rs1 value: %ld,\r\nValue read from memory: %lu, target address: %lu\r\n", 
				   imm, rs1, rd, rs1_val, read_from_memory, target_address);
	    #endif
	    
	    // writeback
            register_write(rd, read_from_memory);   
        }
        else if (funct3 == 0x02) {
            // execute lw
            // LW rd,offset(rs1)	Load Word	rd ← s32[rs1 + offset]
            /*uint64_t rs1_val, rd_val;
	        int64_t read_from_memory;
	        register_read(rs1, rd, &rs1_val, &rd_val);
	        memory_read((rs1_val+imm), &read_from_memory, 4);
	        register_write(rd, read_from_memory);
	    */
            uint64_t rs1_val, rd_val, target_address, read_from_memory;
            int64_t target_s_address, rs1_s_val;
            register_read(rs1, rd, &rs1_val, &rd_val);
	    rs1_s_val = *(int64_t *) &rs1_val;
	    // address calculation
            target_s_address = rs1_s_val + imm;
	    *(int64_t * ) &target_address = target_s_address;
	    // memory reading
            memory_read(target_address, &read_from_memory, 4);
	     #ifdef DEBUG
		printf("LW called. imm = %ld, rs1 = %d, rd = %d, rs1 value: %ld,\r\nValue read from memory: %lu, target address: %lu\r\n", 
				   imm, rs1, rd, rs1_val, read_from_memory, target_address);
	    #endif
	    
	    // writeback
            register_write(rd, read_from_memory);   
        
	    }
        else if (funct3 == 0x03) {
            // execute ld
            // LD rd,offset(rs1)	Load Double	rd ← u64[rs1 + offset]
           /* uint64_t rs1_val, rd_val;
	        int64_t read_from_memory;
	        register_read(rs1, rd, &rs1_val, &rd_val);
	        memory_read((rs1_val+imm), &read_from_memory, 8);
	        register_write(rd, read_from_memory);
            */
	    uint64_t rs1_val, rd_val, target_address, read_from_memory;
            int64_t target_s_address, rs1_s_val;
            register_read(rs1, rd, &rs1_val, &rd_val);
	    rs1_s_val = *(int64_t *) &rs1_val;
	    // address calculation
            target_s_address = rs1_s_val + imm;
	    *(int64_t * ) &target_address = target_s_address;
	    // memory reading
            memory_read(target_address, &read_from_memory, 8);
	    // writeback
            register_write(rd, read_from_memory);   
            #ifdef DEBUG
		printf("LD called. imm = %ld, rs1 = %d, rd = %d, rs1 value: %ld,\r\nValue read from memory: %lu, target address: %lu\r\n", 
				   imm, rs1, rd, rs1_val, read_from_memory, target_address);
	    #endif
	     
	    }
        else if (funct3 == 0x04) {
            // execute lbu
            // LBU rd,offset(rs1)	Load Byte Unsigned	rd ← u8[rs1 + offset]
            uint64_t rs1_val, rd_val, read_from_memory;
	        register_read(rs1, rd, &rs1_val, &rd_val);
	        // memory reading
		memory_read((rs1_val+imm), &read_from_memory, 1);
		#ifdef DEBUG
		printf("LBU called. imm = %ld, rs1 = %d, rd = %d, rs1 value: %ld,\r\nValue read from memory: %lu, target address: %lu\r\n", 
				    imm, rs1, rd, rs1_val, read_from_memory, (rs1_val + imm));
		#endif
	        // writeback
		register_write(rd, read_from_memory);
        }
        else if (funct3 == 0x05) {
            // execute lhu
            // LHU rd,offset(rs1)	Load Half Unsigned	rd ← u16[rs1 + offset]
            uint64_t rs1_val, rd_val;
	    int64_t read_from_memory;
	    register_read(rs1, rd, &rs1_val, &rd_val);
	    
	    memory_read((rs1_val+imm), &read_from_memory, 2);
	    #ifdef DEBUG
		printf("LHU called. imm = %ld, rs1 = %d, rd = %d, rs1 value: %ld,\r\nValue read from memory: %lu, target address: %lu\r\n", 
				    imm, rs1, rd, rs1_val, read_from_memory, (rs1_val + imm));
            #endif
	        
	    register_write(rd, read_from_memory);
        
	}
        else if (funct3 == 0x06) {
            // execute lwu
            // LWU rd,offset(rs1)	Load Word Unsigned	rd ← u32[rs1 + offset]
            uint64_t rs1_val, rd_val;
	    int64_t read_from_memory;
	    register_read(rs1, rd, &rs1_val, &rd_val);
	    
	    memory_read((rs1_val+imm), &read_from_memory, 4);
		#ifdef DEBUG
		printf("LBU called. imm = %ld, rs1 = %d, rd = %d, rs1 value: %ld,\r\nValue read from memory: %lu, target address: %lu\r\n", 
				    imm, rs1, rd, rs1_val, read_from_memory, (rs1_val + imm));
		#endif
	        
	    register_write(rd, read_from_memory);
        
	}
    }
    else if (opcode == 0x13) {
        if (funct3 == 0x00) {
            // Execute ADDI
            uint64_t rs1_val;
	    #ifdef DEBUG
		printf("ADDI: imm: %ld, rs1: %d, rd: %d\r\n", imm, rs1, rd);
	    #endif
            register_read(rs1, rs1, &rs1_val, &rs1_val);
            register_write(rd, rs1_val + imm);
        }
        else if (funct3 == 0x01) {
            // Execute SLLI 
            uint64_t rs1_val;
	    #ifdef DEBUG
		printf("SLLI: imm: %ld, rs1: %d, rd: %d\r\n", imm, rs1, rd);
	    #endif
            register_read(rs1, rs1, &rs1_val, &rs1_val);
            register_write(rd, rs1_val << imm);
        }
        else if (funct3 == 0x02) {
            // Execute SLTI (signed comparison)
            uint64_t rs1_val;
	        int64_t rs1_s_val;
	        register_read(rs1, rs1, &rs1_val, &rs1_val);
            rs1_s_val = *(int64_t *)&rs1_val;
	        if (rs1_s_val < imm) {
                register_write(rd, 1);
            }
            else {
                register_write(rd, 0);
            }
        }
        else if (funct3 == 0x03) {
            // Execute SLTIU (unsigned comparison)
            uint64_t rs1_val;
            register_read(rs1, rs1, &rs1_val, &rs1_val);
            if (rs1_val < imm) {
                register_write(rd, 1);
            }
            else {
                register_write(rd, 0);
            }
        }
        else if (funct3 == 0x04) {
            // Execute XORI
            uint64_t rs1_val;
            register_read(rs1, rs1, &rs1_val, &rs1_val);
            register_write(rd, rs1_val ^ imm);
        }
        else if ((funct3 == 0x05) && (funct7 == 0x00)) {
            // Execute SRLI
            uint64_t rs1_val;
            register_read(rs1, rs1, &rs1_val, &rs1_val);
            register_write(rd, rs1_val >> imm);
        }
        else if ((funct3 == 0x05) && (funct7 != 0x00)) {
            // Execute SRAI
            uint64_t rs1_val, rd_val;
            int64_t rs1_s_val;
            register_read(rs1, rs1, &rs1_val, &rs1_val);
            rs1_s_val = *(int64_t *)&rs1_val;
            *(int64_t *)&rd_val = rs1_s_val >> imm;
            register_write(rd, rd_val);
        }
        else if (funct3 == 0x06) {
            // Execute ORI 
            uint64_t rs1_val;
            register_read(rs1, rs1, &rs1_val, &rs1_val);
            register_write(rd, rs1_val | imm);
        }
        else if (funct3 == 0x07) {
            // Execute ANDI
            uint64_t rs1_val;
            register_read(rs1, rs1, &rs1_val, &rs1_val);
            register_write(rd, rs1_val & imm);
        }
    }
    else if (opcode == 0x1B) {
        if (funct3 == 0x00) {
            // Execute ADDIW
            #ifdef DEBUG 
	    printf("ADDIW: \r\n imm=%ld, rs1=%d, rd=%d", 
		imm, rs1, rd);
	    #endif
            uint64_t rs1_val;
            register_read(rs1, rs1, &rs1_val, &rs1_val);
	    int32_t rs1_s_val;
	    rs1_s_val = *(int32_t *)&rs1_val;
	    // shaving off leftmost 32 bits
            // rs1_val = rs1_val << 32; 
            // rs1_val = rs1_val >> 32;

            register_write(rd, rs1_s_val + imm);
        }
        else if (funct3 == 0x01) {
            // Execute SLLIW 
            #ifdef DEBUG
            printf("SLLIW: imm=%ld, rs1=%d, rd=%d\r\n", imm, rs1, rd);
	    #endif
            uint64_t rs1_val;
            register_read(rs1, rs1, &rs1_val, &rs1_val);

            // shaving off leftmost 32 bits
            rs1_val = rs1_val << 32; 
            rs1_val = rs1_val >> 32;
            
            register_write(rd, rs1_val << imm);
        }
        else if ((funct3 == 0x05) && (funct7 == 0x00)) {
            // Execute SRLIW 
            #ifdef DEBUG
	    printf("SRLIW: imm=%ld, rs1=%d, rd=%d\r\n", imm, rs1, rd);
	    #endif
            uint64_t rs1_val;
            register_read(rs1, rs1, &rs1_val, &rs1_val);

            // shaving off leftmost 32 bits
            rs1_val = rs1_val << 32; 
            rs1_val = rs1_val >> 32;
            
            register_write(rd, rs1_val >> imm);
        }
        else if ((funct3 == 0x05) && (funct7 != 0x00)) {
            // Execute SRAIW
            #ifdef DEBUG
            printf("SRAIW: imm=%ld, rs1=%d, rd=%d\r\n", imm, rs1, rd);
            #endif
	    uint64_t rs1_val;
            register_read(rs1, rs1, &rs1_val, &rs1_val);

            // shaving off leftmost 32 bits
            rs1_val = rs1_val << 32; 
            rs1_val = rs1_val >> 32;

            register_write(rd, rs1_val >> imm);
        }
    }
    else {
        // execute jalr
        // JALR rd,rs1,offset	Jump and Link Register	rd ← pc + length(inst)
        // JALR rd,rs1,offset	Jump and Link Register	pc ← (rs1 + offset) ∧ -2
        // JALR branches to the address stored in rs1 + offset.
        // read page 99 of the textbook
        // also, the riscv-spec-v.2.2 pdf page 28 of 145

        if ((imm & 0x800) == 0x800) { // checking the sign bit on a 12 bit immediate
            imm = imm | 0xFFFFF800; // sign extending 1's
        }

        uint64_t rs1_value;
	register_read(rs1, rs1, &rs1_value, &rs1_value);
	register_write(rd, pc + 4);
        new_pc[0] = rs1_value + imm;
    }
}

void r_type(uint32_t instruction, const uint64_t pc, uint64_t *new_pc) {
    // 7 5 5 3 5 7
    int opcode = instruction & OPCODE_MASK;
    int rd = (instruction & RD_MASK) >> 7; 
    int funct3 = (instruction & FUNCT3_MASK) >> 12;
    int rs1 = (instruction & RS1_MASK) >> 15;
    int rs2 = (instruction & RS2_MASK) >> 20;
    int funct7 = (instruction & FUNCT7_MASK) >> 25;

    if (opcode == 0x33 && funct7 != 0x01) {
        if ((funct3 == 0x00) && (funct7 == 0x00)) {
            // Execute ADD 
            uint64_t rs1_val, rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
	    #ifdef DEBUG
		printf("ADD: rs1: %d, rs2: %d, rd: %d, rs1 value: 0x%lX, rs2 value: 0x%lX\r\n", 
				  rs1, rs2, rd, rs1_val, rs2_val);
	    #endif
            register_write(rd, rs1_val + rs2_val);
        }
        else if ((funct3 == 0x00) && (funct7 == 0x20)) {
            // Execute SUB 
            uint64_t rs1_val, rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
	    #ifdef DEBUG
		printf("SUB: rs1: %d, rs2: %d, rd: %d, rs1 value: 0x%lX, rs2 value: 0x%lX\r\n", 
				  rs1, rs2, rd, rs1_val, rs2_val);
	    #endif
            
            register_write(rd, rs1_val - rs2_val);
        }
        else if (funct3 == 0x01) {
            // Execute SLL 
            uint64_t rs1_val, rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
	    #ifdef DEBUG
		printf("SLL: rs1: %d, rs2: %d, rd: %d, rs1 value: 0x%lX, rs2 value: 0x%lX\r\n", 
				  rs1, rs2, rd, rs1_val, rs2_val);
	    #endif
            
            register_write(rd, rs1_val << rs2_val);
        }
        else if (funct3 == 0x02) {
            // Execute SLT (signed comparison)
            uint64_t rs1_val, rs2_val;
	        int64_t rs1_s_val, rs2_s_val;
	        register_read(rs1, rs2, &rs1_val, &rs2_val);
            rs1_s_val = *(int64_t *)&rs1_val;
	        rs2_s_val = *(int64_t *)&rs2_val;
	        if (rs1_s_val < rs2_s_val) {
                register_write(rd, 1);
            }
            else {
                register_write(rd, 0);
            }
        }
        else if (funct3 == 0x03) {
            // Execute SLTU (unsigned comparison)
            uint64_t rs1_val, rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
            if (rs1_val < rs2_val) {
                register_write(rd, 1);
            }
            else {
                register_write(rd, 0);
            }
        }
        else if (funct3 == 0x04) {
            // Execute XOR
            uint64_t rs1_val, rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
            register_write(rd, rs1_val ^ rs2_val);
        }
        else if ((funct3 == 0x05) && (funct7 == 0x00)) {
            // Execute SRL 
            uint64_t rs1_val, rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
            register_write(rd, rs1_val >> rs2_val);
        }
        else if ((funct3 == 0x05) && (funct7 == 0x20)) {
            // execute SRA
            uint64_t rs1_val, rs2_val, rd_val;
            int64_t rs1_s_val, rs2_s_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
            rs1_s_val = *(int64_t *)&rs1_val;
            rs2_s_val = *(int64_t *)&rs2_val;
            *(int64_t *)&rd_val = rs1_s_val >> rs2_s_val;
            register_write(rd, rd_val);
        }
        else if (funct3 == 0x06) {
            // Execute OR
            uint64_t rs1_val, rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
	    #ifdef DEBUG
		printf("OR: rs1: %d, rs2: %d, rd: %d, rs1 value: 0x%lX, rs2 value: 0x%lX, result: 0x%lX\r\n", 
				  rs1, rs2, rd, rs1_val, rs2_val, rs1_val | rs2_val);
	    #endif
            
            register_write(rd, rs1_val | rs2_val);
        }
        else if (funct3 == 0x07) {
            // Execute AND
            uint64_t rs1_val, rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
            register_write(rd, rs1_val & rs2_val);
        }
    }
    else if (opcode == 0x33 && funct7 == 0x01) {
        uint64_t rs1_val, rs2_val;
	int64_t rs1_s_val, rs2_s_val;
	register_read(rs1, rs2, &rs1_val, &rs2_val);
	// assuming we're ignoring overflow as per the specs:
	if (funct3 == 0x00) {
            // execute MUL
            // MUL rd,rs1,rs2	Multiply	rd ← ux(rs1) × ux(rs2)
            rs1_s_val = *(int64_t *)&rs1_val;
	    rs2_s_val = *(int64_t *)&rs2_val;
	    __int128_t result = rs1_s_val * rs2_s_val;
	    #ifdef DEBUG
		printf("MUL: rs1: %d rs2: %d, rd: %d\r\n, rs1_val: %lu, rs2_val: %lu, result: %lu\r\n",
				rs1, rs2, rd, rs1_val, rs2_val, (uint64_t) result); 
	    #endif
	    // shave off first 64 bits. 
	    result <<= 64;
	    result >>= 64;
	    uint64_t w;
	    *(__int128_t *)&w = result;
	    register_write(rd, w);
        }
        else if (funct3 == 0x01) {
            // execute mulh
            // MULH rd,rs1,rs2	Multiply High Signed Signed	rd ← (sx(rs1) × sx(rs2)) » xlen
            rs1_s_val = *(int64_t *)&rs1_val;
	    rs2_s_val = *(int64_t *)&rs2_val;
	    __uint128_t result = rs1_val * rs2_val;
	    register_write(rd, (uint64_t) (result >> 64));
	}
        else if (funct3 == 0x02) {
            // execute mulhsu
            // MULHSU rd,rs1,rs2	Multiply High Signed Unsigned	rd ← (sx(rs1) × ux(rs2)) » xlen
            // this one's tricky. Which one is signed, and which one is unsigned?
            // rs1_val is signed, and rs2_val is unsigned
            rs1_s_val = *(int64_t *)&rs1_val;
            register_write(rd, (uint64_t) (rs1_s_val * rs2_val));
	}
        else if (funct3 == 0x03) {
            // execute mulhu
            // MULHU rd,rs1,rs2	Multiply High Unsigned Unsigned	rd ← (ux(rs1) × ux(rs2)) » xlen
            register_write(rd, (uint64_t) (rs1_val * rs2_val));
        }
        else if (funct3 == 0x04) {
            // execute div
            // DIV rd,rs1,rs2	Divide Signed	rd ← sx(rs1) ÷ sx(rs2)
            rs1_s_val = *(int64_t *)&rs1_val;
	    rs2_s_val = *(int64_t *)&rs2_val;
	    if(rs2_s_val == 0)
	    {
		register_write(rd, (uint64_t) -1);
	    }
	    else if( rs1_val == (uint64_t) -1 && rs2_val == (uint64_t) -1)
	    {
		register_write(rd, (uint64_t) -1);
	    } 
	    else {
	    register_write(rd, (uint64_t) (rs1_s_val / rs2_s_val));
	    }
	}
        else if (funct3 == 0x05) {
            // execute divu
            // DIVU rd,rs1,rs2	Divide Unsigned	rd ← ux(rs1) ÷ ux(rs2)
            if(rs2_val == 0) 
	    { 
		register_write(rd, (uint64_t) -1);
	    }
	    else {
	    register_write(rd, (uint64_t) (rs1_val / rs2_val));
	    }
        }
        else if (funct3 == 0x06) {
            // execute rem
            // REM rd,rs1,rs2	Remainder Signed	rd ← sx(rs1) mod sx(rs2)
	    rs1_s_val = *(int64_t *)&rs1_val;
	    rs2_s_val = *(int64_t *)&rs2_val;
	    if(rs2_s_val == 0)
	    {
		 register_write(rd, rs1_val);
	    } else if (rs1_val == (uint64_t) -1 && rs2_val == (uint64_t) -1)
	    {
		register_write(rd, 0);
	    }
	    else {
            register_write(rd, (uint64_t) (rs1_s_val % rs2_s_val));
	    }
        }
        else if (funct3 == 0x07) {
            // execute remu
            // REMU rd,rs1,rs2	Remainder Unsigned	rd ← ux(rs1) mod ux(rs2)
            if(rs2_val == 0) {
		register_write(rd, rs1_val);
	    }
	    else {
	    	register_write(rd, rs1_val % rs2_val);
            }
	}
    }
    else if (opcode == 0x3B && funct7 != 0x01) {
        if ((funct3 == 0x00) && (funct7 == 0x00)) {
            // execute addw
            uint64_t rs1_val, rs2_val;
	    int32_t rs1_s_val, rs2_s_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
	    rs1_s_val = *(int32_t *)&rs1_val;
	    rs2_s_val = *(int32_t *)&rs2_val;
	    
            // shaving off leftmost 32 bits
	    /*rs1_val = rs1_val << 32; 
            rs1_val = rs1_val >> 32;
            rs2_val = rs2_val << 32; 
            rs2_val = rs2_val >> 32;
		*/
            register_write(rd, (uint64_t)(rs1_s_val + rs2_s_val));
        }
        else if ((funct3 == 0x00) && (funct7 == 0x20)) {
/*
            // execute subw
            uint64_t rs1_val;
            uint64_t rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);

            // shaving off leftmost 32 bits
            rs1_val = rs1_val << 32; 
            rs1_val = rs1_val >> 32;
            rs2_val = rs2_val << 32; 
            rs2_val = rs2_val >> 32;

            register_write(rd, rs1_val - rs2_val);
*/
            // execute subw
            uint64_t rs1_val;
            uint64_t rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);

            int32_t rs1_s_val, rs2_s_val;
            rs1_s_val = *(int32_t *)&rs1_val;
            rs2_s_val = *(int32_t *)&rs2_val;
            register_write(rd, (uint32_t) rs1_s_val - rs2_s_val);
        }
        else if (funct3 == 0x01) {
            // execute sllw
            uint64_t rs1_val;
            uint64_t rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);

            // shaving off leftmost 32 bits
            rs1_val = rs1_val << 32; 
            rs1_val = rs1_val >> 32;
            rs2_val = rs2_val << 32; 
            rs2_val = rs2_val >> 32;

            register_write(rd, rs1_val << rs2_val);
        }
        else if ((funct3 == 0x05) && (funct7 == 0x00)) {
            // execute srlw
            uint64_t rs1_val;
            uint64_t rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);

            // shaving off leftmost 32 bits
            rs1_val = rs1_val << 32; 
            rs1_val = rs1_val >> 32;
            rs2_val = rs2_val << 32; 
            rs2_val = rs2_val >> 32;

            register_write(rd, rs1_val >> rs2_val);
        }
        else if ((funct3 == 0x05) && (funct7 == 0x20)) {
            // execute sraw
            uint64_t rs1_val;
            uint64_t rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);

            // shaving off leftmost 32 bits
            rs1_val = rs1_val << 32; 
            rs1_val = rs1_val >> 32;
            rs2_val = rs2_val << 32; 
            rs2_val = rs2_val >> 32;

            register_write(rd, rs1_val >> rs2_val);
        }
    }
    else if (opcode == 0x3B && funct7 == 0x01) {
        uint64_t rs1_val, rs2_val;
	int64_t rs1_s_val, rs2_s_val;
	register_read(rs1, rs2, &rs1_val, &rs2_val);
	// all of these are word functions. Shave off the last 32 bits.
	rs1_val <<= 32;
	rs1_val >>= 32;
	rs2_val <<= 32;
	rs2_val >>= 32;
	if (funct3 == 0x00) {
            // execute mulw
            // MULW rd,rs1,rs2	Multiple Word	rd ← u32(rs1) × u32(rs2)
            rs1_s_val = *(int64_t *)&rs1_val;
	    rs2_s_val = *(int64_t *)&rs2_val;
            register_write(rd, (uint64_t)(rs1_s_val * rs2_s_val));
        }
        else if (funct3 == 0x04) {
            // execute divw
            // DIVW rd,rs1,rs2	Divide Signed Word	rd ← s32(rs1) ÷ s32(rs2)
            rs1_s_val = *(int64_t *)&rs1_val;
	    rs2_s_val = *(int64_t *)&rs2_val;
	    if(rs2_s_val == 0)
	    {
		register_write(rd, 0xFFFFFFFF);
	    } else {
            register_write(rd, (uint64_t) (rs1_s_val / rs2_s_val));
	    }
        }
        else if (funct3 == 0x05) {
            // execute divuw
            // DIVUW rd,rs1,rs2	Divide Unsigned Word	rd ← u32(rs1) ÷ u32(rs2)
            
	    if(rs2_val == 0)
	    {
		register_write(rd, 0xFFFFFFFF);
            } else { register_write(rd, rs1_val / rs2_val); }
	}
        else if (funct3 == 0x06) {
            // execute remw
            // REMW rd,rs1,rs2	Remainder Signed Word	rd ← s32(rs1) mod s32(rs2)
            rs1_s_val = *(int64_t *)&rs1_val;
	    rs2_s_val = *(int64_t *)&rs2_val;
	    if(rs2_s_val == 0)
	    {
            register_write(rd, (uint32_t) (rs1_val));
	    } else { register_write(rd, (uint32_t)(rs1_val % rs2_val)); }
	}
        else if (funct3 == 0x07) {
            // execute remuw
            // REMUW rd,rs1,rs2	Remainder Unsigned Word	rd ← u32(rs1) mod u32(rs2)
            if(rs2_val == 0) {
	    register_write(rd, (uint32_t) rs1_val);
	    } else { 
	     register_write(rd, rs1_val % rs2_val);
	    }
        }
    }
}

void s_type(uint32_t instruction, const uint64_t pc, uint64_t *new_pc) {
    int opcode = instruction & OPCODE_MASK;
    int imm_4_0 = (instruction & 0xF80) >> 7;
    int funct3 = (instruction & FUNCT3_MASK) >> 12;
    int rs1 = (instruction & RS1_MASK) >> 15;
    int rs2 = (instruction & RS2_MASK) >> 20;
    int imm_11_5 = (instruction & 0xFE000000) >> 25;

    int32_t imm = (imm_11_5 << 5) | imm_4_0;
    if(imm & 0x800)
    {
	imm |= 0xFFFFF000;
    }
    if (opcode == 0x23) {
        if (funct3 == 0x00) {
            // execute sb
            // SB rs2,offset(rs1)	Store Byte	u8[rs1 + offset] ← rs2
            uint64_t rs1_val, rs2_val, target_address;
	    int64_t target_s_address, rs1_s_val;
	    register_read(rs1, rs2, &rs1_val, &rs2_val);
	    rs1_s_val = *(int64_t *) &rs1_val;
	    // address calculation
	    target_s_address = rs1_s_val + imm;
	    *(int64_t *) &target_address = target_s_address;
	    #ifdef DEBUG
		printf("SB called. imm = %d, rs1 = %d, rs2 = %d, rs1 value: %lu, rs2 value: %lu,\r\nitem written to memory: %lu, address written to: %lu\r\n", 
					imm, rs1, rs2, rs1_val, rs2_val, rs2_val, target_address);
	    #endif
	    memory_write(target_address, rs2_val, 1);
        }
        else if (funct3 == 0x01) {
            // execute sh
            // SH rs2,offset(rs1)	Store Half	u16[rs1 + offset] ← rs2
            uint64_t rs1_val, rs2_val, target_address;
	    int64_t target_s_address, rs1_s_val;
	    register_read(rs1, rs2, &rs1_val, &rs2_val);
	    rs1_s_val = *(int64_t *) &rs1_val;
	    target_s_address = rs1_s_val + imm;
	    *(int64_t *) &target_address = target_s_address;
	    #ifdef DEBUG
		printf("SH called. imm = %d, rs1 = %d, rs2 = %d, rs1 value: %lu, rs2 value: %lu,\r\nitem written to memory: %lu, address written to: %lu \r\n", 
					imm, rs1, rs2, rs1_val, rs2_val, rs2_val, target_address);
	    #endif
	    memory_write(target_address, rs2_val, 2);
        }
        else if (funct3 == 0x02) {
            // execute sw
            // SW rs2,offset(rs1)	Store Word	u32[rs1 + offset] ← rs2
            uint64_t rs1_val, rs2_val, target_address;
	    int64_t target_s_address, rs1_s_val;
	    register_read(rs1, rs2, &rs1_val, &rs2_val);
	    rs1_s_val = *(int64_t *) &rs1_val;
	    target_s_address = rs1_s_val + imm;
	    *(int64_t *) &target_address = target_s_address;
	    #ifdef DEBUG
		printf("SW called. imm = %d, rs1 = %d, rs2 = %d, rs1 value: %lu, rs2 value: %lu,\r\nitem written to memory: %lu, address written to: %lu \r\n", 
					imm, rs1, rs2, rs1_val, rs2_val, rs2_val, target_address);
	    #endif
	    memory_write(target_address, rs2_val, 4); 
	    // printf(" SW ");
        }
        else if (funct3 == 0x03) {
            // execute sd
            // SD rs2,offset(rs1)	Store Double	u64[rs1 + offset] ← rs2
            uint64_t rs1_val, rs2_val, target_address;
	    int64_t rs1_s_val, target_s_address;
	    register_read(rs1, rs2, &rs1_val, &rs2_val);
	    rs1_s_val = *(int64_t *)&rs1_val;
	    // address calculation:
	    target_s_address = rs1_s_val + imm;
	    *(int64_t *)&target_address = target_s_address;
	    // memory writing:
	    #ifdef DEBUG
		printf("SD called. imm = %d, rs1 = %d, rs2 = %d, rs1 value: %lu, rs2 value: %lu,\r\nitem written to memory: %lu, address written to: %lu \r\n", 
					imm, rs1, rs2, rs1_val, rs2_val, rs2_val, target_address);
	    #endif
	    memory_write(target_address,rs2_val,8);
        }
    }
}

void u_type(uint32_t instruction, const uint64_t pc, uint64_t *new_pc) {
    int opcode = instruction & OPCODE_MASK;
    int rd = (instruction & RD_MASK) >> 7;
    int imm = (instruction & 0xFFFFF000) >> 12;

    if (opcode == 0x17) {
        // execute auipc
        // AUIPC rd,offset	Add Upper Immediate to PC	rd ← pc + offset
        new_pc[0] = pc + imm;
        register_write(rd, new_pc[0]);
    }
    else {
        // execute lui
        // LUI rd,imm	Load Upper Immediate	rd ← imm
        register_write(rd, imm);
    }
}

void sb_type(uint32_t instruction, const uint64_t pc, uint64_t *new_pc) {
    int opcode = instruction & OPCODE_MASK;
    int imm_11 = (instruction & 0x80) >> 7;
    int imm_4_1 = (instruction & 0xF00) >> 8;
    int funct3 = (instruction & FUNCT3_MASK) >> 12;
    int rs1 = (instruction & RS1_MASK) >> 15;
    int rs2 = (instruction & RS2_MASK) >> 20;
    int imm_10_5 = (instruction & 0x7E000000) >> 25;
    int imm_12 = (instruction & 0x80000000) >> 31;

    // Concatenated immediate value
    int32_t imm = ((imm_12 << 11) | (imm_11 << 10) | (imm_10_5 << 4) | imm_4_1) << 1;
    if(imm & 0x800)
    {
	imm |= 0xFFFFF000;
    }
    if (opcode == 0x63) {
        if (funct3 == 0x00) {
        // execute beq
        // BEQ rs1,rs2,offset Branch Equal if rs1 = rs2 then pc ← pc + offset
            uint64_t rs1_val, rs2_val;
            int64_t rs1_s_val, rs2_s_val;
	    register_read(rs1, rs2, &rs1_val, &rs2_val);
            rs1_s_val = *(int64_t *) &rs1_val;
	    rs2_s_val = *(int64_t *) &rs2_val;
            
	    #ifdef DEBUG
	    printf("BEQ called. imm=%d, rs1=%d, rs2=%d. ", imm, rs1, rs2);
            #endif
	    if (rs1_s_val == rs2_s_val) {
                new_pc[0] = pc + imm;
                #ifdef DEBUG
	        printf("Branch to: %lX\r\n", new_pc[0]);
                #endif
	    }
            else {
                new_pc[0] = pc + 4; 
            }
        }
        else if (funct3 == 0x01) {
        // execute bne
        // BNE rs1,rs2,offset Branch Not Equal if rs1 ≠ rs2 then pc ← pc + offset
            uint64_t rs1_val, rs2_val;
            int64_t rs1_s_val, rs2_s_val;
	    register_read(rs1, rs2, &rs1_val, &rs2_val);
	    rs1_s_val = *(int64_t *) &rs1_val;
	    rs2_s_val = *(int64_t *) &rs2_val;
            
            #ifdef DEBUG
	    printf("BNE called. imm=%d, rs1=%d, rs2=%d, rs1 value: %lu, rs2 value: %lu \r\n", imm, rs1, rs2, rs1_val, rs2_val);
            #endif
	    if (rs1_s_val != rs2_s_val) {
                new_pc[0] = pc + imm;
                #ifdef DEBUG
		printf("Branch to: %lX\r\n", new_pc[0]);
            	#endif
	    }
            else {
                new_pc[0] = pc + 4; 
            }

        }
        else if (funct3 == 0x04) {
        // execute blt
        // BLT rs1,rs2,offset Branch Less Than if rs1 < rs2 then pc ← pc + offset
            uint64_t rs1_val, rs2_val;
	    int64_t rs1_s_val, rs2_s_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
	    rs1_s_val = *(int64_t *) &rs1_val;
	    rs2_s_val = *(int64_t *) &rs2_val;
            #ifdef DEBUG
            printf("BLT called. imm=%d, rs1=%d, rs2=%d, rs1 value: %lu, rs2 value: %lu\r\n", imm, rs1, rs2, rs1_val, rs2_val);
            #endif
	    if (rs1_s_val < rs2_s_val) {
                new_pc[0] = pc + imm;
                #ifdef DEBUG
		printf("Branch to: %lX\r\n", new_pc[0]);
		#endif
            }
            else {
	        // go to the next insruction.
                new_pc[0] = pc + 4; 
            }

        }
        else if (funct3 == 0x05) {
        // execute bge
        // BGE rs1,rs2,offset Branch Greater than Equal 
        // if rs1 ≥ rs2 then pc ← pc + offset
            uint64_t rs1_val, rs2_val;
	    int64_t rs1_s_val, rs2_s_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
	    rs1_s_val = *(int64_t *) &rs1_val;
	    rs2_s_val = *(int64_t *) &rs2_val;
            #ifdef DEBUG
	    printf("BGE called. imm=%d, rs1=%d, rs2=%d, rs1 value: %lu, rs2 value: %lu\r\n", imm, rs1, rs2, rs1_val, rs2_val);
            #endif
	    if (rs1_s_val >= rs2_s_val) {
                new_pc[0] = pc + imm;
                #ifdef DEBUG
		printf("Branch to: 0x%lX\r\n", new_pc[0]);
		#endif
            }
            else {
                new_pc[0] = pc + 4; 
            }
        }
        else if (funct3 == 0x06) {
            // execute bltu
            // BLTU rs1,rs2,offset	Branch Less Than Unsigned
            // if rs1 < rs2 then pc ← pc + offset
            uint64_t rs1_val, rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
            #ifdef DEBUG
	    printf("BLTU called. imm=%d, rs1=%d, rs2=%d, rs1 value: %lu, rs2 value: %lu\r\n", imm, rs1, rs2, rs1_val, rs2_val);
            #endif
	    if (rs1_val < rs2_val) {
                new_pc[0] = pc + imm;
                #ifdef DEBUG
	        printf("Branch to: %lX\r\n", new_pc[0]);
                #endif
	    }
            else {
                new_pc[0] = pc + 4; 
            }
        }
        else if (funct3 == 0x07) {
            // execute bgeu
            // BGEU rs1,rs2,offset	Branch Greater than Equal Unsigned	
            // if rs1 ≥ rs2 then pc ← pc + offset
            uint64_t rs1_val, rs2_val;
            register_read(rs1, rs2, &rs1_val, &rs2_val);
            #ifdef DEBUG
	    printf("BGEU called. imm=%d, rs1=%d, rs2=%d, rs1 value: %lu, rs2 value: %lu\r\n", imm, rs1, rs2, rs1_val, rs2_val);
            #endif
	    if (rs1_val >= rs2_val) {
                new_pc[0] = pc + imm;
                #ifdef DEBUG
		printf("Branch to: %lX\r\n", new_pc[0]);
            	#endif
	    }
            else {
                new_pc[0] = pc + 4; 
            }
	}
    }
}

void uj_type(uint32_t instruction, const uint64_t pc, uint64_t *new_pc) {
    //int opcode = instruction & OPCODE_MASK;
    int rd = (instruction & RD_MASK) >> 7;
    int imm_19_12 = (instruction & 0xFF000) >> 12;
    int imm_11 = (instruction & 0x100000) >> 13;
    int imm_10_1 = (instruction & 0x7FE00000) >> 21;
    int imm_20 = (instruction & 0x80000000 ) >> 31;

    int imm = ((imm_20 << 19) | (imm_19_12 << 11) | (imm_11 << 10) | imm_10_1) << 1; 

    // execute jal
    // JAL rd,offset	Jump and Link	rd ← pc + length(inst)
    // JAL rd,offset	Jump and Link   pc ← pc + offset
    // form an address that points to the calling site to allow function to return to proper address
    // writes return address to rd.
    // read page 98 of the textbook.
    
    if ((imm & 0x800) == 0x800) { // checking the sign bit on a 12 bit immediate
        imm = imm | 0xFFFFF800; // sign extending 1's
    }
    
    uint64_t target_address;
    int64_t target_s_address, pc_s;
    pc_s = *(int64_t *) &pc;
    target_s_address = pc_s + imm; 
    *(int64_t *) &target_address = target_s_address;
    #ifdef DEBUG
    printf("JAL called. imm: %d\r\n0x%lX written to register %d to return to. \r\nGoing to 0x%lX.\r\n", 
			imm, (pc+4), rd, target_address); 
    #endif
    register_write(rd, pc + 4);
    *new_pc = target_address;
}
