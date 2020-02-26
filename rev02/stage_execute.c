#include "stage_execute.h"

void stage_execute(struct stage_reg_m *new_m_reg)
{
	// protection from bad instructions.
	if(current_stage_x_register == NULL){ return; }
	
	// protection from flushed instructions
	if(current_stage_x_register->NOP == true) { return; }
	
	suppress_btb_size_unused_warning();
	
	// protection against bad instructions.
	uint32_t i = current_stage_x_register->instruction;
	if(i == 0) { return; } 	
	
	static int flush_flag = 0;
	uint64_t address = 0; // address to read/write to during the memory stage. 
	char type = current_stage_x_register->class;
	static uint64_t rs1_val, rs2_val;
	uint64_t result;
	uint16_t funct7, funct3;
	
	// implement stalling until the data is forwarded.
	/*if(current_stage_x_register->STALL == true) {
		bool start_executing = false;
		uint16_t rs1 = current_stage_x_register->rs1;
		uint16_t rs2 = current_stage_x_register->rs2;
		uint16_t previous_x_rd = current_stage_m_register->rd;
		uint16_t m_rd = current_stage_w_register->rd;
		static bool rs1_flag = 0; // tells if rs1 has a value.
		static bool rs2_flag = 0; // tells if rs2 has a value.
		switch(type) {
			case addi:
			case u_type:
			case u_a_type:
			case i_type:
				if(!rs1_flag)
				{
					if(rs1 == m_rd) 
					{
						rs1_val = current_stage_w_register->mx_forwarding_variable;
						rs1_flag = 1;
						
					} else if(rs1 == previous_x_rd) {
						rs1_val = current_stage_m_register->xd_forwarding_variable;
						rs1_flag = 1;
					}
				}
				rs2_flag = 1;
				rs2_val = 0; // rs2 isn't used in these functions.
				break;
			default:
				if(!rs1_flag)
				{
					if(rs1 == m_rd) 
					{
						rs1_val = current_stage_w_register->mx_forwarding_variable;
						rs1_flag = 1;
					
					} else if(rs1 == previous_x_rd) {
						rs1_val = current_stage_m_register->xd_forwarding_variable;
						rs1_flag = 1;
					}
				}
				if(!rs2_flag)
				{
					if(rs2 == m_rd)
					{
						rs2_val = current_stage_w_register->mx_forwarding_variable;
						rs2_flag = 1;
					} else if(rs2 == previous_x_rd) {
						rs2_val = current_stage_m_register->xd_forwarding_variable;
						rs2_flag = 1;
					}
				}
			break;
		}
		if(rs1_flag && rs2_flag)
		{
			start_executing = true;
			rs1_flag = 0;
			rs2_flag = 0;
		}
		#ifdef DEBUG
		printf("instruction stalling. values we have: ");
		if(rs1_flag) { printf("rs1_val: %ld", rs1_val);}
		switch(type) {
			case i_type:
			case i_l_type:
			case i_w_type:
			case i_j_type:
			case u_j_type:
				break;	
			default:
				if(rs2_flag) { printf("rs2_val: %ld", rs2_val);}
				break;
		}
		printf("\r\n"); 
		#endif
		if(!start_executing) { return;}
			// if the value isn't ready yet, stall the decode instruction until ready.
		if(flush_flag) { new_m_reg->NOP = true; flush_flag--; return;}

	} else {
		rs1_val = current_stage_x_register->rs1_val;
		rs2_val = current_stage_x_register->rs2_val;
	}
	if(flush_flag) { new_m_reg->NOP = true; flush_flag--; return;}
	*/
	// debugging. to turn this off, go to Makefile, remove DEBUG from DEFINES variable.
	#ifdef DEBUG
	printf("Execute stage, instruction = 0x%x\r\n", i);
	#endif

	switch(type)
	{
		case lui: // lui
			// lui does nothing in execute or memory, only writeback.
			// it writes imm to rd.
			new_m_reg->value = current_stage_x_register->imm;
			break;
		case auipc: // auipc	
			address = current_stage_x_register->new_pc + current_stage_x_register->imm;
			new_m_reg->value = current_stage_x_register->imm;
			break;
		case jal: // jal
			address = current_stage_x_register->new_pc + current_stage_x_register->imm;
			new_m_reg->value = current_stage_x_register->new_pc + 4;
			// implement branch prediction here.
			break;
		case sb:
		case sh:
		case sw:
		case sd:
			address = current_stage_x_register->new_pc + current_stage_x_register->imm;
			new_m_reg->value = rs1_val;
			break;
		case add:
			result = rs1_val + rs2_val;
			new_m_reg->value = result;
			break;
		case sub:
			result = rs1_val - rs2_val;
			new_m_reg->value = result;
			break;
		case sll:
			result = rs1_val << rs2_val;
			new_m_reg->value = result;
			break;
		case slt:
			{
			int64_t rs1_s_val, rs2_s_val;
			rs1_s_val = *(int64_t *)&rs1_val;
	     		rs2_s_val = *(int64_t *)&rs2_val;
	       		result = rs1_s_val < rs2_s_val;
			new_m_reg->value = result;
			}
			break;
		case sltu:
			result = rs1_val < rs2_val;
			new_m_reg->value = result;
			break;	
		case or1:
			result = rs1_val ^ rs2_val;
			new_m_reg->value = result;
			break;
		case and1:
			result = rs1_val & rs2_val;
			new_m_reg->value = result;
			break;
		case mul:
			{
			int64_t rs1_s_val, rs2_s_val;
			rs1_s_val = *(int64_t *)&rs1_val;
	       		rs2_s_val = *(int64_t *)&rs2_val;
			__int128_t r = rs1_s_val * rs2_s_val;
			r <<= 64;
			r >>= 64;
	       		*(__int128_t *)&result = r;	
			new_m_reg->value = result;
			}
			break;
		case mulh: 
			{
			int64_t rs1_s_val = *(int64_t *)&rs1_val;
	    		int64_t rs2_s_val = *(int64_t *)&rs2_val;
	    		__uint128_t r = rs1_s_val * rs2_s_val;
	    		result = (uint64_t) (r >> 64);	
			new_m_reg->value = result;
			}
			break;
			
		case mulhsu:
			{
			int64_t rs1_s_val = *(int64_t *)&rs1_val;
			result = (uint64_t) (rs1_s_val * rs2_val);
			new_m_reg->value = result;
			}
			break;
		case mulhu:
			result = rs1_val * rs2_val;
			new_m_reg->value = result;
			break;
		case div:
			{
			int64_t rs1_s_val = *(int64_t *)&rs1_val;
			int64_t rs2_s_val = *(int64_t *)&rs2_val;
			if(rs2_s_val == 0) {
				result = -1; 
			} else if (rs1_val == -1 && rs2_val == -1) {
				result = -1;
			} else {
				result = (uint64_t) (rs1_s_val / rs2_s_val);
			}
			new_m_reg->value = result;
			}
			break;	
		case divu:
			if(rs2_val == 0) {
				result = -1;
			} else {
				result = rs1_val / rs2_val;
			}	
			new_m_reg->value = result;
			break;
		case rem:
			{
			int64_t rs1_s_val = *(int64_t *)&rs1_val;
			int64_t rs2_s_val = *(int64_t *)&rs2_val;
			if(rs2_s_val == 0) { 
				result = rs1_val; 
			}
			else if (rs1_val == -1 && rs2_val == -1) {
				result = 0;
			} else {
				result = rs1_s_val % rs2_s_val;
			}
			new_m_reg->value = result;
			}
			break;
		case remu:
			if(rs2_val == 0) {
				result = rs1_val;
			} else {
				result = rs1_val % rs2_val;
			}
			new_m_reg->value = result;
			break;
		case addw:
			{
			int32_t rs1_s32_val, rs2_s32_val;
			rs1_s32_val = *(int32_t *)&rs1_val;
			rs2_s32_val = *(int32_t *)&rs2_val;
			result = rs1_s32_val + rs2_s32_val;
			new_m_reg->value = result;
			}
			break;
		case subw:
			{
			int32_t rs1_s32_val, rs2_s32_val;
			rs1_s32_val = *(int32_t *)&rs1_val;
			rs2_s32_val = *(int32_t *)&rs2_val;
			result = rs1_s32_val - rs2_s32_val;
			new_m_reg->value = result;
			}
			break;
		case sllw:
			{
			int32_t rs1_s32_val, rs2_s32_val;
			rs1_s32_val = *(int32_t *)&rs1_val;
			rs2_s32_val = *(int32_t *)&rs2_val;
			result = rs1_s32_val << rs2_s32_val;
			new_m_reg->value = result;
			}
			break;
		case srlw:
			{
			int32_t rs1_s32_val, rs2_s32_val;
			rs1_s32_val = *(int32_t *)&rs1_val;
			rs2_s32_val = *(int32_t *)&rs2_val;
			result = rs1_s32_val >> rs2_s32_val;
			new_m_reg->value = result;
			}
			break;
		case srai:
			{
			int32_t rs1_s32_val, rs2_s32_val;
			rs1_s32_val = *(int32_t *)&rs1_val;
			rs2_s32_val = *(int32_t *)&rs2_val;
			result = rs1_s32_val >> rs2_s32_val;
			if(result & (0x80000000 >> rs2_val))
			{
				result &= (0xFFFFFFFF << (32 - rs2_val));
			}
			new_m_reg->value = result;
			}
			break;
		/*
  			case i_type:
			funct3 = current_stage_x_register->funct3;
			int64_t imm = current_stage_x_register->imm;
			switch(funct3)
			{
				case 0x00: // addi
					result = rs1_val + imm;
					break;
				case 0x01: // slli
					result = rs1_val << imm;
					break;
				case 0x02: // slti
					{
					int64_t rs1_s_val = *(int64_t *)&rs1_val;
					result = rs1_s_val < imm;
					}
					break;
				case 0x03: // sltiu
					result = rs1_val < imm;
					break;
				case 0x04: // xori
					result = rs1_val ^ imm;
					break;
				case 0x05: // srli and srai
					// only access funct7 if funct3 is 0x05 to save on execution time
					if(current_stage_x_register->funct7 == 0)
					{
						result = rs1_val >> imm; // srli
					} else {
						// srai
						int64_t rs1_s_val = *(int64_t *)&rs1_val;
						*(int64_t *)&result = rs1_s_val >> imm;
					}
				case 0x06: // ori
					result = rs1_val | imm;
					break;
				case 0x07: // andi
					result = rs1_val & imm;
					break;
				default:
					result = 0; // prevents bugs
					break;
			}
			new_m_reg->value = result;
			break;
		case i_w_type: // wide immediate arithmetic
			funct3 = current_stage_x_register->funct3;
			imm = current_stage_x_register->imm;
			int32_t rs1_s32_val;
			switch(funct3) {
				case 0x00: // addiw
					rs1_s32_val = *(int32_t *)&rs1_val;
					result = rs1_s32_val + imm;
					break;
				case 0x01: // slliw
					rs1_s32_val = *(int32_t *)&rs1_val;
					result = rs1_s32_val << imm;
				case 0x05: // srliw and sraiw
					if(current_stage_x_register->funct7 == 0x00) { 
						// srliw
						result = rs1_val >> imm;
					
					} else { // sraiw
						rs1_s32_val = *(int32_t *)&rs1_val;
						result = rs1_s32_val >> imm;
					}
				default:
					break;
				
			}
			new_m_reg->value = result;
			break;
		case i_l_type: // load instructions
			address = current_stage_x_register->new_pc + current_stage_x_register->imm; 	
			break;
		case i_j_type: // jalr function
			new_m_reg->value = current_stage_x_register->new_pc + 4; // this value will be written to rd, to return to after the function is called.
			new_m_reg->address = rs1_val + current_stage_x_register->imm;
			break;
		case sb_type: // branch instructions.
			funct3 = current_stage_x_register->funct3;
			rs2_val = current_stage_x_register->rs2_val;
			imm = current_stage_x_register->imm;
			uint64_t pc = current_stage_x_register->new_pc; // pc which the instruction is located at.
			int64_t rs1_s_val, rs2_s_val;
			// implement branch prediction here. if it fails, flush the other instructions. how are we going to do that?
			switch(funct3)
			{
				case 0x00:  // beq
					if(rs1_val == rs2_val) {
						address = pc + imm;
					} else {
						address = pc + 4;
					}
					break;
				case 0x01: // bne
					if(rs1_val != rs2_val) {
						address = pc + imm;
					} else {
						address = pc + 4;
					}
					break;
				case 0x04: // blt
					rs1_s_val = *(int64_t *) &rs1_val;
	    				rs2_s_val = *(int64_t *) &rs2_val;
            
					if(rs1_s_val < rs2_s_val) {
						address = pc + imm;
					} else {
						address = pc + 4;
					}
					
					break;
				case 0x05: // bge
					rs1_s_val = *(int64_t *) &rs1_val;
	    				rs2_s_val = *(int64_t *) &rs2_val;
           				if(rs1_s_val >= rs2_s_val) {
						address = pc + imm;
					} else {
						address = pc + 4;
					} 
					break;
				case 0x06: // bltu
					if(rs1_val < rs2_val) {
						address = pc + imm;
					} else {
						address = pc + 4;
					}
					break;
				case 0x07: // bgeu
					if(rs1_val >= rs2_val) {
						address = pc + imm;
					} else {
						address = pc + 4;
					}
					break;
				default:
					break;
			}
					break;
		*/
		default:
			break;
	}
	// branch prediction
	bool in_btb;
	switch(type)
	{
		case jal:
		case jalr:
		case beq:
		case blt:
		case bge:
		case bltu:
		case bgeu:
			in_btb = false;
			// If it's in the BTB, and the branch taken is not the correct branch, flush the instructions.
			for(int i = 0; i < btb_size; i++)
			{
				if((btb[i] & 0xFFFFFFFF) == current_stage_x_register->new_pc)
				{
					in_btb = true;
					if((btb[i] >> 32) != address)
					{
						// flush mispredicted instructions (somehow) 
						// register_flush(&current_stage_d_register);
						flush_flag = 2;
						set_pc(address);
					}
				}	
			}
			// if it's not in the BTB, put it in 
			if(!in_btb)
			{
				btb[btb_size++ % BHT_SIZE] = (address << 32) | (current_stage_x_register->new_pc & 0xFFFFFFFF);
			}
			break;
		default:
			break;
	}
	assert(address >= 0);
	new_m_reg->instruction = i;
	new_m_reg->class = type;
	new_m_reg->address = address;
	// implement data forwarding here
	new_m_reg->xd_forwarding_variable = new_m_reg->value;
	// implement memory stalling here.
	new_m_reg->NOP = false;
	// reset rs1 and rs2;
	rs1_val = 0;
	rs2_val = 0;
}
