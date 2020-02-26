#include "riscv_pipeline_registers.h"
#include "riscv_sim_pipeline_framework.h"
#include "global_variables.h"

#define BTB_MASK 0xFFFFFFFF

void stage_execute (struct stage_reg_m *new_m_reg) {
    // if the memory is reading, then stall the execute stage.
    if (current_stage_w_register->memoryread == 1) {
        return;
    }
    // if we terminated the writeback last time, don't terminate the writeback this time.
    //
    if (current_stage_m_register->terminateWriteback == true) {
        new_m_reg->terminateWriteback = false;
    }

    uint32_t i = current_stage_x_register->instruction;
    uint64_t pc = current_stage_x_register->pc;
    uint32_t class = current_stage_x_register->class;
    uint16_t rs1 = current_stage_x_register->rs1;
    uint16_t rs2 = current_stage_x_register->rs2;
    uint16_t rd = current_stage_x_register->rd;
    uint32_t imm = current_stage_x_register->imm;
    uint64_t rs1_val = current_stage_x_register->rs1_val;
    uint64_t rs2_val = current_stage_x_register->rs2_val;
    // printf("Execute stage, instruction = 0x%x, c = %d rs1 = %d, rs2 = %d, rd = %d, imm = %d\r\n", i, class, rs1, rs2, rd, imm);

    int64_t rs1_s_val, rs2_s_val;
    int32_t rs1_s32_val, rs2_s32_val;
    uint32_t r32;
    uint64_t r64;
    __int128_t r1;
    __uint128_t r2;
    rs1_s_val = *(int64_t *) &rs1_val;
    rs2_s_val = *(int64_t *) &rs2_val;
    rs1_s32_val = *(int32_t *)&rs1_val;
    rs2_s32_val = *(int32_t *)&rs2_val;

    uint64_t branchTarget;
    // if we decided to stall last time, stop here.
    if (current_stage_m_register->branch == 2) {
        new_m_reg->NOP = true;
        new_m_reg->branch = 1;
        return;
    }

    if (current_stage_x_register->NOP == true) {
        new_m_reg->NOP = true;
        return;
    }

    // WX Forwarding
    if (rs1 == current_stage_w_register->rd) {
        rs1_val = current_stage_w_register->value;
    }
    if (rs2 == current_stage_w_register->rd) {
        rs2_val = current_stage_w_register->value;
    }

    // MX Forwarding
    if (rs1 == current_stage_m_register->rd) {
        rs1_val = current_stage_m_register->value;
    }
    if (rs2 == current_stage_m_register->rd) {
        rs2_val = current_stage_m_register->value;
    }

    // Isolate branches and jumps from rest of instructions
    if (class == beq || class == bne || class == blt || class == bge || class == bltu || class == bgeu) {
        branchTarget = pc + imm;
        if (branchTarget < pc) {
            int i, j;
            for (i = 0; i < 32; i++) {
                if ((btb[i] & BTB_MASK) == pc) {
                    switch(class) {
                        case beq:
                            if (rs1_s_val != rs2_s_val) {
                                new_m_reg->terminateWriteback = true;
                                btb[i] = 0;
                                set_pc(pc);
                                new_m_reg->branch = 2;
                            }
                            break;
                        case bne:
                            if (rs1_s_val == rs2_s_val) {
                                new_m_reg->terminateWriteback = true;
                                btb[i] = 0;
                                set_pc(pc);
                                new_m_reg->branch = 2;
                            }
                            break;
                        case blt:
                            if (rs1_s_val >= rs2_s_val) {
                                new_m_reg->terminateWriteback = true;
                                btb[i] = 0;
                                set_pc(pc);
                                new_m_reg->branch = 2;
                            }
                            break;
                        case bge:
                            if (rs1_s_val < rs2_s_val) {
                                new_m_reg->terminateWriteback = true;
                                btb[i] = 0;
                                set_pc(pc);
                                new_m_reg->branch = 2;
                            }
                            break;
                        case bltu:
                            if (rs1_val >= rs2_val) {
                                new_m_reg->terminateWriteback = true;
                                btb[i] = 0;
                                set_pc(pc);
                                new_m_reg->branch = 2;
                            }
                            break;
                        case bgeu:
                            if (rs1_val < rs2_val) {
                                new_m_reg->terminateWriteback = true;
                                btb[i] = 0;
                                set_pc(pc);
                                new_m_reg->branch = 2;
                            }
                            break;
                        default:
                            break;
                    }
                    new_m_reg->NOP = true;
                    return;
                }
            }
            for (j = 0; j < 32; j++) {
                if (btb[j] == 0) {
                    btb[j] = branchTarget << 32;
                    btb[j] |= pc;
                    break;
                }
            }    
        }
        switch(class) {
            case beq:
	            if (rs1_s_val == rs2_s_val) {
                    set_pc(branchTarget - 4);
                    new_m_reg->branch = 2;
                }
                break;
            case bne:
	            if (rs1_s_val != rs2_s_val) {
                    set_pc(branchTarget - 4);
                    new_m_reg->branch = 2;
                }
                break;
            case blt:
	            if (rs1_s_val < rs2_s_val) {
                    set_pc(branchTarget - 4);
                    new_m_reg->branch = 2;
                }
                break;
            case bge:
	            if (rs1_s_val >= rs2_s_val) {
                    set_pc(branchTarget - 4);
                    new_m_reg->branch = 2;
                }
                break;
            case bltu:
                if (rs1_val < rs2_val) {
                    set_pc(branchTarget - 4);
                    new_m_reg->branch = 2;
                }
                break;
            case bgeu:
                if (rs1_val >= rs2_val) {
                    set_pc(branchTarget - 4);
                    new_m_reg->branch = 2;
                }
                break;
            default:
                break;
        }
        new_m_reg->NOP = true;
        return;
    }
    else if (class == jal || class == jalr) {
        switch(class) {
            case jal:
                set_pc(pc + imm - 4);
                break;
            case jalr:
                set_pc(rs1_val + imm - 4);
                break;
            default:
                break;
        }
        new_m_reg->rd = rd;
        new_m_reg->value = pc + 4;
        new_m_reg->branch = 2;
        new_m_reg->NOP = false;
        return;
    }
    else {
        switch(class) {
            case lui:
                new_m_reg->value = imm << 12;
                break;
            case auipc:
                new_m_reg->value = (imm << 12) + pc;
                break;
            case sb:
                break;
		    case sh:
                break;
		    case sw:
                break;
		    case sd:
                break;
            case lb:
                break;
		    case lh:
                break;
		    case lw:
                break;
		    case ld:
                break;
            case lbu:
                break;
            case lhu:
                break;
            case lwu:
                break;
            case add:
			    new_m_reg->value = rs1_val + rs2_val;
			    break;
            case sub:
                new_m_reg->value = rs1_val - rs2_val;
                break;
            case sll:
                new_m_reg->value = rs1_val << rs2_val;
                break;
            case slt:
                if (rs1_s_val < rs2_s_val) {
                    new_m_reg->value = 1;
                }
                else {
                    new_m_reg->value = 0;
                }
                break;
            case sltu:
                if (rs1_val < rs2_val) {
                    new_m_reg->value = 1;
                }
                else {
                    new_m_reg->value = 0;
                }
                break;
            case xor1:
                new_m_reg->value = rs1_val ^ rs2_val;
                break;
            case srl:
                new_m_reg->value = rs1_val >> rs2_val;
                break;
            case sra:
                *(int64_t *)&r64 = rs1_s_val >> rs2_s_val;
                new_m_reg->value = r64;
            case or1:
                new_m_reg->value = rs1_val | rs2_val;
                break;
            case and1:
                new_m_reg->value = rs1_val & rs2_val;
                break;
            case mul:
                r1 = rs1_s_val * rs2_s_val;
                r1 <<= 64;
                r1 >>= 64;	
                new_m_reg->value = r1;
                break;
            case mulh: 
                r2 = rs1_s_val * rs2_s_val;
                new_m_reg->value = (uint64_t) (r2 >> 64);
                break;
            case mulhsu:
                new_m_reg->value = (uint64_t) (rs1_s_val * rs2_val);
                break;
            case mulhu:
                new_m_reg->value = rs1_val * rs2_val;
                break;
            case div:
                if (rs2_s_val == 0) {
                    new_m_reg->value = -1; 
                } else if (rs1_s_val == -1 && rs2_s_val == -1) {
                    new_m_reg->value = -1;
                } else {
                    new_m_reg->value = (uint64_t) (rs1_s_val / rs2_s_val);
                }
                break;	
            case divu:
                if (rs2_val == 0) {
                    new_m_reg->value = -1;
                } else {
                    new_m_reg->value = rs1_val / rs2_val;
                }
                break;
            case rem:
                if(rs2_s_val == 0) { 
                    new_m_reg->value = rs1_val; 
                }
                else if (rs1_s_val == -1 && rs2_s_val == -1) {
                    new_m_reg->value = 0;
                } else {
                    new_m_reg->value = rs1_s_val % rs2_s_val;
                }
                break;
            case remu:
                if(rs2_val == 0) {
                    new_m_reg->value = rs1_val;
                } else {
                    new_m_reg->value = rs1_val % rs2_val;
                }
                break;
            case addw:
                new_m_reg->value = rs1_s32_val + rs2_s32_val;
                break;
            case subw:
                new_m_reg->value = rs1_s32_val - rs2_s32_val;
                break;
            case sllw:
                new_m_reg->value = rs1_s32_val << rs2_s32_val;
                break;
            case srlw:
                new_m_reg->value = rs1_s32_val >> rs2_s32_val;
                break;
            case sraw:
                *(int32_t *)&r32 = rs1_s32_val >> rs2_s32_val;
                new_m_reg->value = r32;
            case mulw:
                r64 = rs1_s32_val * rs2_s32_val;
                r64 <<= 32;
                r64 >>= 32;	
                new_m_reg->value = r64;
                break;
            case divw:
                if (rs2_s32_val == 0) {
                    new_m_reg->value = -1; 
                } else if (rs1_s32_val == -1 && rs2_s32_val == -1) {
                    new_m_reg->value = -1;
                } else {
                    new_m_reg->value = (uint32_t) (rs1_s32_val / rs2_s32_val);
                }
                break;
            case divuw:
                if (rs2_val == 0) {
                    new_m_reg->value = -1;
                } else {
                    new_m_reg->value = rs1_val / rs2_val;
                }
                break;
            case remw:
                if(rs2_s32_val == 0) { 
                    new_m_reg->value = rs1_val; 
                }
                else if (rs1_s32_val == -1 && rs2_s32_val == -1) {
                    new_m_reg->value = 0;
                } else {
                    new_m_reg->value = rs1_s32_val % rs2_s32_val;
                }
            case remuw:
                if(rs2_val == 0) {
                    new_m_reg->value = rs1_val;
                } else {
                    new_m_reg->value = rs1_val % rs2_val;
                }
            case addiw:
                new_m_reg->value = rs1_val + imm;
                break;
            case slliw:
                new_m_reg->value = rs1_val << imm;
                break;
            case srliw:
                new_m_reg->value = rs1_val >> imm;
                break;
            case sraiw:
                *(int32_t *)&r32 = rs1_s32_val >> imm;
                new_m_reg->value = r32;
                break;
            case addi:
                new_m_reg->value = rs1_val + imm;
                break;
            case slli:
                new_m_reg->value = rs1_val << imm;
                break;
            case slti:
                if (rs1_s_val < imm) {
                    new_m_reg->value = 1;
                }
                else {
                    new_m_reg->value = 0;
                }
                break;
            case sltiu:
                if (rs1_val < imm) {
                    new_m_reg->value = 1;
                }
                else {
                    new_m_reg->value = 0;
                }
                break;
            case xori:
                new_m_reg->value = rs1_val ^ imm;
                break;
            case srli:
                new_m_reg->value = rs1_val >> imm;
                break;
            case srai:
                *(int64_t *)&r64 = rs1_s_val >> imm;
                new_m_reg->value = r64;
                break;
            case ori:
                new_m_reg->value = rs1_val | imm;
                break;
            case andi:
                new_m_reg->value = rs1_val & imm;
                break;
            case invalid:
                break;
            default:
                break;
        }
    }
    new_m_reg->instruction = i;
    new_m_reg->class = class;
    new_m_reg->pc = pc;
    new_m_reg->rs1 = rs1;
    new_m_reg->rs2 = rs2;
    new_m_reg->rs1_val = rs1_val;
    new_m_reg->rs2_val = rs2_val;
    new_m_reg->rd = rd;
    new_m_reg->imm = imm;
    new_m_reg->branch = 1;
    new_m_reg->NOP = false;
    return;
}
