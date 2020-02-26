#include "stage_memory.h"

void stage_memory(struct stage_reg_w *new_w_reg)
{
	if(current_stage_m_register == NULL || current_stage_m_register->NOP == true)
	{
		return;
	}
    suppress_btb_size_unused_warning();
	
	uint64_t read_from_memory, address, write_to_memory;
	uint16_t funct3;
	char type = current_stage_m_register->class;
    // protection against bad instructions.
	if(current_stage_m_register->instruction == 0) { return; } 

	switch(type)
	{
		case lb:			// sign extended for lb, lh, lw. ld doesn't need it.
			memory_read(address, &read_from_memory, 1);
			if(read_from_memory & 0x80) {
				read_from_memory &= 0xFFFFFFFFFFFFFF80;
			}
			new_w_reg->value = read_from_memory; // writes to register during writeback stage.
			break;
		case lh:
			memory_read(address, &read_from_memory, 2);
			if(read_from_memory & 0x8000) {
				read_from_memory &= 0xFFFFFFFFFFFF8000;
			}
			new_w_reg->value = read_from_memory; // writes to register during writeback stage.
			break;
		case lw: // lw
			memory_read(address, &read_from_memory, 4);
			if(read_from_memory & 0x80000000) {
				read_from_memory &= 0xFFFFFFFF80000000;
			}
			new_w_reg->value = read_from_memory; // writes to register during writeback stage.
			break;
		case ld: // ld
			memory_read(address, &read_from_memory, 8);
			new_w_reg->value = read_from_memory; // writes to register during writeback stage.
			break;
		case lbu: // lbu
			memory_read(address, &read_from_memory, 1);
			new_w_reg->value = read_from_memory; // writes to register during writeback stage.
			break;
		case lhu: // lhu
			memory_read(address, &read_from_memory, 2);
			new_w_reg->value = read_from_memory; // writes to register during writeback stage.
			break;
		case lwu: // lwu
			memory_read(address, &read_from_memory, 4);
			new_w_reg->value = read_from_memory; // writes to register during writeback stage.
			break;
		case sb: // store instructions: access memory
			write_to_memory = current_stage_m_register->value;
			memory_write(address, write_to_memory, 1);
			break;
		case sh:
			write_to_memory = current_stage_m_register->value;
			memory_write(address, write_to_memory, 2);
			break;
		case sw:
			write_to_memory = current_stage_m_register->value;
			memory_write(address, write_to_memory, 4);
			break;
		case sd:	
			write_to_memory = current_stage_m_register->value;
			memory_write(address, write_to_memory, 8);
			break;
		default:
			new_w_reg->value = current_stage_m_register->value;
			break;
	}
	new_w_reg->instruction = current_stage_m_register->instruction;
	new_w_reg->class = type; // current_stage_m_register->class;
	new_w_reg->rd = current_stage_m_register->rd;
	// implement data forwarding here
	new_w_reg->mx_forwarding_variable = new_w_reg->value;
	new_w_reg->md_forwarding_variable = new_w_reg->value;
	// implement writeback stalling here.	
	new_w_reg->NOP = false;
	#ifdef DEBUG
	printf("Memory stage, instruction = 0x%x \r\n", current_stage_m_register->instruction);
	#endif
}
