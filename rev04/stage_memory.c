#include "riscv_pipeline_registers.h"
#include "riscv_pipeline_registers_vars.h"
#include "riscv_sim_framework.h"
#include "global_variables.h"
#include "memory_translation.h"
#include "tlb_interface.h"

#define OFFSET_MASK_D 0xFFF
#define INDEX_MASK_D 0x7FF000
#define TAG_MASK_D 0xFFC00000

/***************************************************************************
 *   Some debugging stuff to track what is happening to value.
 *   Comment this line out when finished debugging:
 *   #define VALUETRACE 0
 END***********************************************************************/

void stage_memory (struct stage_reg_w *new_w_reg) {
    // If stalled pass the stall through
    if (current_stage_m_register->NOP == true) {
        new_w_reg->NOP = true;
        return;
    }

    uint32_t i = current_stage_m_register->instruction;
    uint32_t class = current_stage_m_register->class;
    uint64_t pc = current_stage_m_register->pc;
    uint16_t rs1 = current_stage_m_register->rs1;
    uint16_t rs2 = current_stage_m_register->rs2;
    uint64_t rs1_val = current_stage_m_register->rs1_val;
    uint64_t rs2_val = current_stage_m_register->rs2_val;
    uint16_t rd = current_stage_m_register->rd;
    uint32_t imm = current_stage_m_register->imm;

    uint32_t virtual;
    uint32_t physical;

    #ifdef VALUETRACE    
    printf("VALUETRACE ********************************************\r\n");
    printf("Initial value definition\r\n");
    printf("current_stage_m_register->value = %d\r\n", current_stage_m_register->value);
    printf("uint64_t value = current_stage_m_register->value;\r\n");
    #endif
    uint64_t value = current_stage_m_register->value;
    #ifdef VALUETRACE    
    printf("value = %d\r\n", value);
    printf("current_stage_m_register->value = %d\r\n", 
            current_stage_m_register->value);
    printf("END TRACED BLOCK***************************************\r\n");
    #endif

    // Print the current stage and instruction
    // printf("Memory stage, instruction = 0x%x\r\n", i);
    int64_t read_from_memory;
    int x;

    // Grab RS1 or RS2 values if either is currently RD
    if (rs1 == current_stage_w_register->rd) {
        rs1_val = current_stage_w_register->value;
    }
    if (rs2 == current_stage_w_register->rd) {
        rs2_val = current_stage_w_register->value;
    }

    // Pass current value to be sent to Memory/Writeback stage register
    new_w_reg->instruction = i;
    new_w_reg->class = class;
    new_w_reg->pc = pc;
    new_w_reg->rd = rd;
    new_w_reg->value = value;
    new_w_reg->NOP = false;

    #ifdef VALUETRACE    
    printf("VALUETRACE ********************************************\r\n");
    printf("Initial set: new_w_reg->value = value;\r\n");
    printf("value = %d\r\n", value);
    printf("new_w_reg->value = %d\r\n", new_w_reg->value);
    printf("END TRACED BLOCK***************************************\r\n");
    #endif

    // Determine if a load instruction is required
    if (class == lb || class == lh || class == lw || class == ld || class == ld || class == lbu) {

        virtual = rs1_val + imm;

		if(tlb_find_entry(virtual, 'M')) {
			physical = tlb_get_tag(virtual, 'M');
		}
		else {
			physical = virtual_to_physical(virtual, (virtual & OFFSET_MASK_D));
		}
        //physical = virtual_to_physical(virtual, (virtual & OFFSET_MASK_D));

        #ifdef VALUETRACE
        printf("VALUETRACE ********************************************\r\n");
        printf("Entering load operation if() block\r\n");
        printf("END TRACED BLOCK***************************************\r\n");
        #endif

        // Iterate through data cache
        // Check for valid bit in D-cache blocks
        // Check the index in each block
        // Check the tag in each block
        // If there is a match, grab the data from the cache block
        for (x = 0; x < 2048; x++) {
            if (d_cache[x].valid_bit == true) {
                if (d_cache[x].index == virtual && INDEX_MASK_D) {
                    if (d_cache[x].tag == physical && TAG_MASK_D) {
                        new_w_reg->value = d_cache[x].data;

                        #ifdef VALUETRACE
                        printf("VALUETRACE ********************************************\r\n");
                        printf("Load: d_cache[x].valid_bit == true\r\n");
                        printf("if (d_cache[x].tag == (rs1_val+imm) && TAG_MASK_D) {}\r\n");
                        printf("value = %d\r\n", value);
                        printf("new_w_reg->value = %d\r\n", new_w_reg->value);
                        printf("END TRACED BLOCK***************************************\r\n");
                        #endif

                        return;
                    }
                }
            }
        }

        // Check memory read
        // Check memory status
        // Iterate through data cache
        // Determine load type
        // Perform load operation
        if (current_stage_w_register->memoryread == 1) {
            if (memory_status(physical, &value)) {
                for (x = 0; x < 2048; x++) {
                    if (d_cache[x].valid_bit == false) {
                        d_cache[x].valid_bit = true;
                        d_cache[x].data = value;
                        d_cache[x].offset = virtual && OFFSET_MASK_D;
                        d_cache[x].index = virtual && INDEX_MASK_D;
                        d_cache[x].tag = physical && TAG_MASK_D;
                        new_w_reg->memoryread = 0;
                        new_w_reg->value = value;
                        new_w_reg->pc = pc;
                        new_w_reg->NOP = false;

                        #ifdef VALUETRACE
                        printf("VALUETRACE********************************************\r\n");
                        printf("Load: if memoryread == 1\r\n");
                        printf("if (d_cache[x].valid_bit == false) {}\r\n");
                        printf("value = %d\r\n", value);
                        printf("new_w_reg->value = %d\r\n", new_w_reg->value);
                        printf("END TRACED BLOCK***************************************\r\n");
                        #endif

                        return;
                    }
                }
            }
            else {
                new_w_reg->NOP = true;
                return;
            }
        }
        else {
            switch(class) {
                case lb:    // LB
                    if (memory_read(physical, &value, 1)) {
                        for (x = 0; x < 2048; x++) {
                            if (d_cache[x].valid_bit == false) {
                                d_cache[x].valid_bit = true;
                                d_cache[x].data = value;
                                d_cache[x].offset = virtual && OFFSET_MASK_D;
                                d_cache[x].index = virtual && INDEX_MASK_D;
                                d_cache[x].tag = physical && TAG_MASK_D;
                                new_w_reg->value = value;
                                new_w_reg->pc = pc;
                                new_w_reg->NOP = false;

                                #ifdef VALUETRACE
                                printf("VALUETRACE********************************************\r\n");
                                printf("(LB) \r\n");
                                printf("if (d_cache[x].valid_bit == false) {}\r\n");
                                printf("value = %d\r\n", value);
                                printf("new_w_reg->value = %d\r\n", new_w_reg->value);
                                printf("END TRACED BLOCK***************************************\r\n");
                                #endif

                                return;
                            }
                        }
                    }
                    else {
                        new_w_reg->memoryread = 1;
                        new_w_reg->NOP = true;
                        return;
                    }
                case lh:    // LH
                    if (memory_read(physical, &value, 2)) {
                        for (x = 0; x < 2048; x++) {
                            if (d_cache[x].valid_bit == false) {
                                d_cache[x].valid_bit = true;
                                d_cache[x].data = value;
                                d_cache[x].offset = virtual && OFFSET_MASK_D;
                                d_cache[x].index = virtual && INDEX_MASK_D;
                                d_cache[x].tag = physical && TAG_MASK_D;
                                new_w_reg->value = value;
                                new_w_reg->pc = pc;
                                new_w_reg->NOP = false;

                                #ifdef VALUETRACE
                                printf("VALUETRACE********************************************\r\n");
                                printf("(LH) \r\n");
                                printf("if (d_cache[x].valid_bit == false) {}\r\n");
                                printf("value = %d\r\n", value);
                                printf("new_w_reg->value = %d\r\n", new_w_reg->value);
                                printf("END TRACED BLOCK***************************************\r\n");
                                #endif

                                return;
                            }
                        }
                    }
                    else {
                        new_w_reg->memoryread = 1;
                        new_w_reg->NOP = true;
                        return;
                    }
                case lw:    // LW
                    if (memory_read(physical, &value, 4)) {
                        for (x = 0; x < 2048; x++) {
                            if (d_cache[x].valid_bit == false) {
                                d_cache[x].valid_bit = true;
                                d_cache[x].data = value;
                                d_cache[x].offset = virtual && OFFSET_MASK_D;
                                d_cache[x].index = virtual && INDEX_MASK_D;
                                d_cache[x].tag = physical && TAG_MASK_D;
                                new_w_reg->value = value;
                                new_w_reg->pc = pc;
                                new_w_reg->NOP = false;

                                #ifdef VALUETRACE
                                printf("VALUETRACE********************************************\r\n");
                                printf("(LW) \r\n");
                                printf("if (d_cache[x].valid_bit == false) {}\r\n");
                                printf("value = %d\r\n", value);
                                printf("new_w_reg->value = %d\r\n", new_w_reg->value);
                                printf("END TRACED BLOCK***************************************\r\n");
                                #endif

                                return;
                            }
                        }
                    }
                    else {
                        new_w_reg->memoryread = 1;
                        new_w_reg->NOP = true;
                        return;
                    }
                case ld:    // LD
                    if (memory_read(physical, &value, 8)) {
                        for (x = 0; x < 2048; x++) {
                            if (d_cache[x].valid_bit == false) {
                                d_cache[x].valid_bit = true;
                                d_cache[x].data = value;
                                d_cache[x].offset = virtual && OFFSET_MASK_D;
                                d_cache[x].index = virtual && INDEX_MASK_D;
                                d_cache[x].tag = physical && TAG_MASK_D;
                                new_w_reg->value = value;
                                new_w_reg->pc = pc;
                                new_w_reg->NOP = false;

                                #ifdef VALUETRACE
                                printf("VALUETRACE********************************************\r\n");
                                printf("(LD) \r\n");
                                printf("if (d_cache[x].valid_bit == false) {}\r\n");
                                printf("value = %d\r\n", value);
                                printf("new_w_reg->value = %d\r\n", new_w_reg->value);
                                printf("END TRACED BLOCK***************************************\r\n");
                                #endif

                                return;
                            }
                        }
                    }
                    else {
                        // Stall
                        new_w_reg->memoryread = 1;
                        new_w_reg->NOP = true;
                        return;
                    }
                case lbu:   // LBU
                    if (memory_read(physical, &value, 1)) {
                        for (x = 0; x < 2048; x++) {
                            if (d_cache[x].valid_bit == false) {
                                d_cache[x].valid_bit = true;
                                d_cache[x].data = value;
                                d_cache[x].offset = virtual && OFFSET_MASK_D;
                                d_cache[x].index = virtual && INDEX_MASK_D;
                                d_cache[x].tag = physical && TAG_MASK_D;
                                new_w_reg->value = value;
                                new_w_reg->pc = pc;
                                new_w_reg->NOP = false;

                                #ifdef VALUETRACE
                                printf("VALUETRACE********************************************\r\n");
                                printf("(LBU)\r\n");
                                printf("if (d_cache[x].valid_bit == false) {}\r\n");
                                printf("value = %d\r\n", value);
                                printf("new_w_reg->value = %d\r\n", new_w_reg->value);
                                printf("END TRACED BLOCK***************************************\r\n");
                                #endif

                                return;
                            }
                        }
                    }
                    else {
                        new_w_reg->memoryread = 1;
                        new_w_reg->NOP = true;
                        return;
                    }
                case lhu:   // LHU
                    if (memory_read(physical, &value, 2)) {
                        for (x = 0; x < 2048; x++) {
                            if (d_cache[x].valid_bit == false) {
                                d_cache[x].valid_bit = true;
                                d_cache[x].data = value;
                                d_cache[x].offset = virtual && OFFSET_MASK_D;
                                d_cache[x].index = virtual && INDEX_MASK_D;
                                d_cache[x].tag = physical && TAG_MASK_D;
                                new_w_reg->value = value;
                                new_w_reg->pc = pc;
                                new_w_reg->NOP = false;

                                #ifdef VALUETRACE
                                printf("VALUETRACE********************************************\r\n");
                                printf("(LHU) \r\n");
                                printf("if (d_cache[x].valid_bit == false) {}\r\n");
                                printf("value = %d\r\n", value);
                                printf("new_w_reg->value = %d\r\n", new_w_reg->value);
                                printf("END TRACED BLOCK***************************************\r\n");
                                #endif

                                return;
                            }
                        }
                    }
                    else {
                        // Stall
                        new_w_reg->memoryread = 1;
                        new_w_reg->NOP = true;
                        return;
                    }
                case lwu:   // LWU
                    if (memory_read(physical, &value, 4)) {
                        for (x = 0; x < 2048; x++) {
                            if (d_cache[x].valid_bit == false) {
                                d_cache[x].valid_bit = true;
                                d_cache[x].data = value;
                                d_cache[x].offset = virtual && OFFSET_MASK_D;
                                d_cache[x].index = virtual && INDEX_MASK_D;
                                d_cache[x].tag = physical && TAG_MASK_D;
                                new_w_reg->value = value;
                                new_w_reg->pc = pc;
                                new_w_reg->NOP = false;

                                #ifdef VALUETRACE
                                printf("VALUETRACE********************************************\r\n");
                                printf("(LWU)\r\n");
                                printf("if (d_cache[x].valid_bit == false) {}\r\n");
                                printf("value = %d\r\n", value);
                                printf("new_w_reg->value = %d\r\n", new_w_reg->value);
                                printf("END TRACED BLOCK***************************************\r\n");
                                #endif

                                return;
                            }
                        }
                    }
                    else {
                        // Stall
                        new_w_reg->memoryread = 1;
                        new_w_reg->NOP = true;
                        return;
                    }
                default:
                    break;
            }
        }
    }

    // Determine if store instruction is needed
    // Iterate through data cache
    // Check valid bit
    // Check index
    // Check tag
    // Write memory if it is a match
    else if (class == sb || class == sh || class == sw || class == sd) {
        virtual = rs1_val + imm;

		if(tlb_find_entry(virtual, 'm')) {
			physical = tlb_get_tag(virtual, 'm');
		}
		else {
			physical = virtual_to_physical(virtual, (virtual & OFFSET_MASK_D));
		}
        //physical = virtual_to_physical(virtual, (virtual & OFFSET_MASK_D));

        for (x = 0; x < 2048; x++) {
            if (d_cache[x].valid_bit == true) {
                if (d_cache[x].index == virtual && INDEX_MASK_D) {
                    if (d_cache[x].tag == physical && TAG_MASK_D) {
                        d_cache[x].data = rs2_val;
                        switch(class) {
                            case sb:    // SB
                                memory_write((rs1_val + imm), rs2_val, 1);
                                new_w_reg->NOP = true;
                                return;
                            case sh:    // SH
                                memory_write((rs1_val + imm), rs2_val, 2);
                                new_w_reg->NOP = true;
                                return;
                            case sw:    // SW
                                memory_write((rs1_val + imm), rs2_val, 4);
                                new_w_reg->NOP = true;
                                return;
                            case sd:    //SD
                                memory_write((rs1_val + imm), rs2_val, 8);
                                new_w_reg->NOP = true;
                                return;
                            default:
                                return;
                        }
                    }
                }
            }
        }

        // Check memory status and stall if needed
        if (current_stage_w_register->memoryread == 1) {
            if (memory_status(physical, &value)) {
                new_w_reg->memoryread = 0;
                new_w_reg->NOP = false;
                return;
            }
            else {
                new_w_reg->NOP = true;
                return;
            }
        }
        else {
            switch(class) {
                case sb:    // SB
                    if (!memory_write((rs1_val + imm), rs2_val, 1)) {
                        new_w_reg->memoryread = 1;
                    }
                    new_w_reg->NOP = true;
                    return;
                case sh:    // SH
                    if (!memory_write((rs1_val + imm), rs2_val, 2)) {
                        new_w_reg->memoryread = 1;
                    }
                    new_w_reg->NOP = true;
                    return;
                case sw:    // SW
                    if (!memory_write((rs1_val + imm), rs2_val, 3)) {
                        new_w_reg->memoryread = 1;
                    }
                    new_w_reg->NOP = true;
                    return;
                case sd:    // SD
                    if (!memory_write((rs1_val + imm), rs2_val, 4)) {
                        new_w_reg->memoryread = 1;
                    }
                    new_w_reg->NOP = true;
                    return;
                default:
                    return;
            }
        }
    }
    #ifdef VALUETRACE    
    printf("VALUETRACE ********************************************\r\n");
    printf("Finished if else block for load and store operations...\r\n");
    printf("value = %d\r\n", value);
    printf("new_w_reg->value = %d\r\n", new_w_reg->value);
    printf("END TRACED BLOCK***************************************\r\n");
    #endif
}
