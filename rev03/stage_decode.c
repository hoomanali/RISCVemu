#include "riscv_pipeline_registers.h"
#include "riscv_sim_pipeline_framework.h"
#include "global_variables.h"

#define OPCODE_MASK 0x7F

void stage_decode (struct stage_reg_x *new_x_reg) {
    
    // if the memory stage is stalling due to memory, then stall the decode stage.
    if (current_stage_w_register->memoryread == 1) {
        return;
    }
    // if the execute stage determined we needed to branch, we flush the instruction.
    // if the fetch stage determined we needed to branch, we flush the instruction.
    if (current_stage_m_register->branch == 2 || current_stage_d_register->NOP == true) {
        new_x_reg->NOP = true;
        return;
    }
    
    uint32_t i = current_stage_d_register->instruction;
    uint64_t pc = current_stage_d_register->pc;
    uint16_t opcode = i & OPCODE_MASK;
    uint16_t rs1=0, rs2=0, rd=0, funct3=0, funct7=0;
    uint32_t imm=0;
    uint64_t rs1_val, rs2_val;

    // printf("Decode stage, instruction = 0x%x\r\n", i);
    
    // if we decided to stalll last time, we fetch the previous instruction. 
    if (current_stage_x_register->stall == 2) {
        new_x_reg->stall = 1;
        i = current_stage_x_register->stall_instruction;
        pc = current_stage_x_register->stall_pc;
    }

    // decode operation here.
    // reference the riscv-spec-v2.2 pdf for details.
    switch(opcode)
    {
        case 0x13:
            // immediate arithmetic
            rd =    (i >> 7) & 0b11111;
            funct3= (i >> 12) & 0b111;
            rs1 =   (i >> 15) & 0b11111;
            imm =   (i >> 20) & 0b111111111111;
            funct7 = (i >> 25);
            if ((imm >> 11) & 1 ) {
                imm |= 0xFFFFF800;
            }
            switch(funct3)
            {
                case 0x0:
                    new_x_reg->class = addi;
                    break;
                case 0x1:
                    new_x_reg->class= slli;
                    break;
                case 0x2:
                    new_x_reg->class = slti;
                    break;
                case 0x3:
                    new_x_reg->class = sltiu;
                    break;
                case 0x4:
                    new_x_reg->class = xori;
                    break;
                case 0x5:
                    if ((imm >> 5) == 0x20)
                    {
                        new_x_reg->class = srli;
                    }
                    else { new_x_reg->class = srai; }
                    break;
                case 0x6:
                    new_x_reg->class = ori;
                    break;
                case 0x07:
                    new_x_reg->class = andi;
                    break;
                default:
                    break;
            }
            break;
        case 0x03:
            // load instructions
            rd = 	(i >> 7) & 0b11111; 
            funct3= (i >> 12) & 0b111;
            rs1 = 	(i >> 15) & 0b11111; 
            imm = (i >> 20); 
            if ((imm >> 11) & 1 ) {
                imm |= 0xFFFFF800;
            }
            switch(funct3)
            {
                case 0x0:
                    new_x_reg->class = lb;
                    break;
                case 0x1:
                    new_x_reg->class = lh;
                    break;
                case 0x2:
                    new_x_reg->class = lw;
                    break;
                case 0x3:
                    new_x_reg->class = ld;
                    break;
                case 0x4:
                    new_x_reg->class = lbu;
                    break;
                case 0x5:
                    new_x_reg->class = lhu;
                    break;
                case 0x6:
                    new_x_reg->class = lwu;
                    break;
            }
            break;
        case 0x1B:
            // wide immediate arithmetic
            rd = 	(i >> 7) & 0b11111; 
            funct3= (i >> 12) & 0b111; 
            rs1 = 	(i >> 15) & 0b11111; 
            imm = 	(i >> 20) & 0b111111111111; 
            funct7 = (i >> 25);
            if ((imm >> 11) & 1 ) {
                imm |= 0xFFFFF800;
            }
            switch(funct3)
            {
                case 0x0:
                    new_x_reg->class = addiw;
                    break;
                case 0x1:
                    new_x_reg->class = slliw;
                    break;
                case 0x2:
                    new_x_reg->class = srliw;
                    break;
                case 0x3:
                    new_x_reg->class = sraiw;
                    break;
            }
            break;
        case 0x67:
            // jalr function
            rd = 	(i >> 7) & 0b11111; // 5 bits
            funct3= (i >> 12) & 0b111; // 3 bits
            rs1 = 	(i >> 15) & 0b11111; // 5 bits;
            imm = 	(i >> 20) & 0b111111111111; // 12 bits;
            if ((imm >> 11) & 1 ) {
                imm |= 0xFFFFF800;
            }
            new_x_reg->class = jalr;
            break;
        case 0x23:
            // stores
            funct3 = 	    (i >> 12) & 0b111; 
            uint8_t imm_4_0 = 	(i >> 7) & 0b11111;
            rs1 = 			(i >> 15) & 0b11111;
            rs2 = 			(i >> 20) & 0b11111;
            uint8_t imm_11_5 = 	(i >> 25);
            imm = imm_4_0 | (imm_11_5 << 4);
            if ((imm >> 11) & 1) {
                imm |= 0xFFFFF800;
            }
            switch(funct3)
            {
                case 0x0:
                    new_x_reg->class = sb;
                    break;
                case 0x1:
                    new_x_reg->class = sh;
                    break;
                case 0x02:
                    new_x_reg->class = sw;
                    break;
                case 0x03:
                    new_x_reg->class = sd;
                    break;
            }
            break;
        case 0x63:
            // branch instructions
            funct3 = 		(i >> 12) & 0b111;
            uint8_t imm_11 = 	(i >> 7) & 1; 
            uint8_t imm_4_1 = 	(i >> 8) & 0b1111; 
            rs1 = 			(i >> 15) & 0b11111; 
            rs2 = 			(i >> 20) & 0b11111;
            uint8_t imm_10_5 =	(i >> 25) & 0b111111;
            uint8_t imm_12 = 	(i >> 31) & 1; 
            imm = 0 | (imm_4_1 << 1) | (imm_10_5 << 4) | (imm_11 << 10) | (imm_12 << 11);
            if ((imm >> 11) & 1) {
                imm |= 0xFFFFF800;
            }
            switch(funct3)
            {
                case 0x0:
                    new_x_reg->class = beq;
                    break;
                case 0x1:
                    new_x_reg->class = bne;
                    break;
                case 0x4:
                    new_x_reg->class = blt;
                    break;
                case 0x5:
                    new_x_reg->class = bge;
                    break;
                case 0x6:
                    new_x_reg->class = bltu;
                    break;
                case 0x7:
                    new_x_reg->class = bgeu;
                    break;
            }
            break;
        case 0x17:
            // lui
            new_x_reg->class = lui;
            rd = (i >> 7) & 0b11111; 
            imm = (i >> 12);
            if ((imm >> 20) & 1 ) {
                imm |= 0xFFF00000;
            }
            new_x_reg->class = lui;
            break;
        case 0x37:
            // auipc
            rd =  (i >> 7) & 0b11111; 
            imm = (i >> 12);
            if ((imm >> 20) & 1 ) {
                imm |= 0xFFF00000;
            }
            new_x_reg->class = auipc;
            break;
        case 0x6F:
            // jal
            rd = 			        (i >> 7)  & 0b11111;
            uint8_t imm_19_12 = 	(i >> 12) & 0b11111111;
            imm_11 =		        (i >> 20) & 0b1;
            uint16_t imm_10_1 = 	(i >> 21) & 0b1111111111;
            uint8_t imm_20 = 	    (i >> 31) & 0b1; 
            imm = 0 | (imm_20 << 19) | (imm_19_12 << 11) | (imm_11 << 10) | (imm_10_1 << 1);
            if ((imm >> 20) & 1 ) {
                imm |= 0xFFF00000;
            }
            new_x_reg->class = jal;
            break;	
        case 0x33:
            // arithmetic
            rd = 		(i >> 7) & 0b11111;
            funct3 = 	(i >> 12) & 0b111;
            rs1 = 		(i >> 15) & 0b11111;
            rs2 = 		(i >> 20) & 0b11111;
            funct7 = 	(i >> 25);	
            if (funct7 != 0x1) { 
            switch(funct3) 
                {
                case 0x0:
                    if(funct7 == 0x20) {
                        new_x_reg->class = sub;
                    } else { new_x_reg->class = add; }
                    break;
                case 0x01:
                    new_x_reg->class = sll;
                    break;
                case 0x02:
                    new_x_reg->class = slt;
                    break;
                case 0x03:
                    new_x_reg->class = sltu;
                    break;
                case 0x04:
                    new_x_reg->class = xor1;
                    break;
                case 0x05:
                    if(funct7 == 0x20) {
                        new_x_reg->class = sra;
                    } else { new_x_reg->class = srl; }
                    break;
                case 0x06:
                    new_x_reg->class = or1;
                    break;
                case 0x07:
                    new_x_reg->class = and1;
                    break;
                default:
                    break;
                }
            } else {
                switch(funct3) 
                {
                case 0x0:
                    new_x_reg->class = mul;
                    break;
                case 0x01:
                    new_x_reg->class = mulh;
                    break;
                case 0x02:
                    new_x_reg->class = mulhsu;
                    break;
                case 0x03:
                    new_x_reg->class = mulhu;
                    break;
                case 0x04:
                    new_x_reg->class = div;
                    break;
                case 0x05:
                    new_x_reg->class = divu;
                    break;
                case 0x06:
                    new_x_reg->class = rem;
                    break;
                case 0x07:
                    new_x_reg->class = remu;
                    break;
                default:
                    break;
                }
            }
            break;
        case 0x3B:
            // wide arithmetic
            rd = 		(i >> 7) & 0b11111;
            funct3 = 	(i >> 12) & 0b111;
            rs1 = 		(i >> 15) & 0b11111;
            rs2 = 		(i >> 20) & 0b11111;
            funct7 = 	(i >> 25);
            if(funct7 != 0x1) {
                switch(funct3)
                {
                case 0x0:
                    if(funct7 == 0x20) {
                    new_x_reg->class = subw;
                    } else {
                    new_x_reg->class = addw; 
                    }
                    break;
                case 0x1:
                    new_x_reg->class = sllw;
                    break;	
                case 0x5:
                    if(funct7 == 0x20) {
                    new_x_reg->class = sraw;
                    } else {
                    new_x_reg->class = srlw; 
                    }
                    break;
                default:
                    break;
                }

            } else {
                switch(funct3)
                {
                    case 0x0:	
                        new_x_reg->class = mulw;
                        break;	
                    case 0x4:
                        new_x_reg->class = divw;
                        break;
                    case 0x5:
                        new_x_reg->class = divuw;
                        break;	
                    case 0x6:
                        new_x_reg->class = remw;
                        break;
                    case 0x7:
                        new_x_reg->class = remuw;
                        break;
                }
            }		
            break;
        default:
            new_x_reg->class = invalid;
            break;
    }

    // if one of the operands is currently being fetched from memory,
    // stall the decode stage until they have been loaded.

    if (rs1 == current_stage_x_register->rd || rs2 == current_stage_x_register->rd) {
        if (current_stage_x_register->class == lb ||
        current_stage_x_register->class == lh ||
        current_stage_x_register->class == lw ||
        current_stage_x_register->class == ld)
        {
            //stall
            new_x_reg->stall_instruction = i; 
            new_x_reg->stall_pc = pc;
            new_x_reg->stall = 2;
            set_pc(get_pc() - 4); 
            new_x_reg->NOP = true;
        }
        else {
            new_x_reg->NOP = false;
        }
    }
    else {
        new_x_reg->NOP = false;
    }

    register_read(rs1, rs2, &rs1_val, &rs2_val);

    new_x_reg->instruction = i;
    new_x_reg->pc = pc;
    new_x_reg->rs1 = rs1;
    new_x_reg->rs2 = rs2;
    new_x_reg->rd = rd;
    new_x_reg->imm = imm;
    new_x_reg->rs1_val = rs1_val;
    new_x_reg->rs2_val = rs2_val;
}
