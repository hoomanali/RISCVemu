#include "stage_fetch.h"

void stage_fetch(struct stage_reg_d *new_d_reg)
{
	uint32_t instruction;
	memory_read(get_pc(), &instruction, 4);
	if(instruction == 0) { return; } // protection against bad instructions.
	new_d_reg->instruction = instruction;
	new_d_reg->new_pc = get_pc();
	#ifdef DEBUG
	printf("Fetch instruction 0x%x at PC %lu\r\n", 
				instruction, get_pc());
	#endif
	
	// branch prediction.
    suppress_btb_size_unused_warning();
	uint8_t low_order_bits = get_pc() & 0xF;
	uint32_t tag  		= get_pc() & 0xFFFFFFF0;
	int next_instruction = true;
	for(int i = 0; i < btb_size; i++)
	{
		if((btb[i] & 0xF) == low_order_bits) {
			if((btb[i] & 0xFFFFFFF0) == tag) {
				/// if the branch is already in the BTB, go to the target address.
				// target address is first 32 bits of btb entry.	
				#ifdef DEBUG
				printf("instruction 0x%X retrieved from BTB. Branching to 0x%lX\r\n", 
					instruction, btb[i] >> 32);
				#endif
				next_instruction = false;
				set_pc(btb[i] >> 32); // destination is in the first 32 bits.
				break;
			}
		}
	}
	if(next_instruction) {
		set_pc(get_pc() + 4); // for now
	}
	// implement decode stalling here.
	new_d_reg->NOP = false;
}

