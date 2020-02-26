#include "riscv_pipeline_registers.h"
#include "riscv_sim_pipeline_framework.h"
#include "global_variables.h"

#define OFFSET_MASK_I 0xF
#define INDEX_MASK_I 0x1FF0
#define TAG_MASK_I 0xFFFFFFFFFFFFE000
#define BTB_MASK 0xFFFFFFFF

void stage_fetch (struct stage_reg_d *new_d_reg) {
    // if the memory stage is not done reading from main memory,
    // or if the decode stage is stalling,
    // don't even start fetching.
    if (current_stage_w_register->memoryread == 1 || current_stage_x_register->stall == 2) {
        return;
    }

    // variables
    uint32_t instruction;
    int i, x;
    // iterate through the whole I-cache.
    // check every valid cache entry. 
    // if the index matches, and the tag matches,
    // fetch the instruction from the I-cache. 
    // then, check if the program counter is in the BTB.
    // if so, set the program counter to the BTB's target.
    // 
    // right now, we're caling
    // set_pc(get_pc() + 4)
    // regardless of whether or not the BTB is a hit.
    for (x = 0; x < 512; x++) {
        if (i_cache[x].valid_bit == true) {
            if (i_cache[x].index == get_pc() && INDEX_MASK_I) {
                if (i_cache[x].tag == get_pc() && TAG_MASK_I) {
                    // printf("Fetch instruction 0x%x at PC %lu\r\n", i_cache[x].data, get_pc());
                    new_d_reg->instruction = i_cache[x].data;
                    new_d_reg->pc = get_pc();
                    new_d_reg->NOP = false;
                    for (i = 0; i < 32; i++) {
                        if (((btb[i] & BTB_MASK) == get_pc()) && get_pc() != 0) {
                            set_pc(btb[i] >> 32);
                            return;
                        }
                    }

                    set_pc(get_pc() + 4);
                    return;
                }
            }
        }
    }
    // if we had to stall the fetch stage last time, 
    // check if the memory is finished reading.
    // if so, put the instruction into the first non-valid I-cache block we find.
    // Then, check the BTB.
    // if not, then continue reading.
    if (current_stage_d_register->memoryread == 1) {
        if (memory_status(get_pc(), &instruction)) {
            for (x = 0; x < 512; x++) {
                if (i_cache[x].valid_bit == false) {
                    i_cache[x].valid_bit = true;
                    i_cache[x].data = instruction;
                    i_cache[x].offset = get_pc() && OFFSET_MASK_I;
                    i_cache[x].index = get_pc() && INDEX_MASK_I;
                    i_cache[x].tag = get_pc() && TAG_MASK_I;
                    new_d_reg->memoryread = 0;
                    // printf("Fetch instruction 0x%x at PC %lu\r\n", instruction, get_pc());
                    new_d_reg->instruction = instruction;
                    new_d_reg->pc = get_pc();
                    new_d_reg->NOP = false;

                    for (i = 0; i < 32; i++) {
                        if (((btb[i] & BTB_MASK) == get_pc()) && get_pc() != 0) {
                            set_pc(btb[i] >> 32);
                            return;
                        }
                    }

                    set_pc(get_pc() + 4);
                    return;
                }   
            }
        }
        else {
            new_d_reg->NOP = true;
            return;
        }
    }
    else {
	// if this is our first time calling stage_fetch for this instruction (not stalling):
	// we will try to read it. If we succeed (reading only takes 1 cycle),
	// we write the instruction to the first invalid I-cache block we see, and make it valid.
	// then we check the BTB.
	// 
	// If we don't succeed, we stall the fetch stage.
	
        if (memory_read(get_pc(), &instruction, 4)) {
            for (x = 0; x < 512; x++) {
                if (i_cache[x].valid_bit == false) {
                    i_cache[x].valid_bit = true;
                    i_cache[x].data = instruction;
                    i_cache[x].offset = get_pc() && OFFSET_MASK_I;
                    i_cache[x].index = get_pc() && INDEX_MASK_I;
                    i_cache[x].tag = get_pc() && TAG_MASK_I;

                    // printf("Fetch instruction 0x%x at PC %lu\r\n", instruction, get_pc());
                    new_d_reg->instruction = instruction;
                    new_d_reg->pc = get_pc();
                    new_d_reg->NOP = false;
                    
                    for (i = 0; i < 32; i++) {
                        if (((btb[i] & BTB_MASK) == get_pc()) && get_pc() != 0) {
                            set_pc(btb[i] >> 32);
                            return;
                        }
                    }

                    set_pc(get_pc() + 4);
                    return;
                }
            }
        }
        else {
            new_d_reg->memoryread = 1;
            new_d_reg->NOP = true;
        }
    }
}
