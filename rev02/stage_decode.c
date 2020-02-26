#include "stage_decode.h"
#define OPCODE_MASK 0b1111111

void stage_decode(struct stage_reg_x *new_x_reg)
{
	// code guard
	if(current_stage_d_register == NULL) { return; }
	// implement stalling here.
	if(current_stage_d_register->NOP == true) { return;} 
    suppress_btb_size_unused_warning();
	// variable declaration
	uint32_t i = current_stage_d_register->instruction;	// always access instruction
	uint16_t opcode = i & OPCODE_MASK;			// we're decoding from the opcode.
	uint16_t rs1=0, rs2=0, rd=0, funct3=0, funct7=0; // only use when necessary.
	uint32_t imm=0; 					// imm can be 20 bits or 12 bits.
	uint64_t rs1_val=0, rs2_val=0;			// read from register in decode stage. 
								// set default values to prevent bugs.
	if(i == 0) { return; } // protection from bad instructions.
	#ifdef DEBUG
	printf("Decode stage, instruction = 0x%x\r\n", i);
	#endif

	switch(opcode)
	{
		case 0x13:
			// immediate arithmetic
			rd =    (i >> 7) & 0b11111; 	// 5 bits
			funct3= (i >> 12) & 0b111; 	// 3 bits
			rs1 =   (i >> 15) & 0b11111; 	// 5 bits;
			imm =   (i >> 20) & 0b11111;   // lowest order 5 bits;
			funct7 = (i >> 25);
			// sign extension
			if((imm >> 11) & 1 ) {
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
					if((imm >> 5) == 0x20)
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
			
			// at the end: register_read(rs1, rs1, &rs1_val, &rs1_val);
			rs2 = 0; // unused variables;
			break;
		case 0x03:
			// load instructions
			rd = 	(i >> 7) & 0b11111; // 5 bits
			funct3= (i >> 12) & 0b111; // 3 bits
			rs1 = 	(i >> 15) & 0b11111; // 5 bits;
			
			// at the end: register_read(rs1, rs1, &rs1_val, &rs1_val);
			imm = (i >> 20); // 12 bits;
			if((imm >> 11) & 1 ) {
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
			rs2 = 0, funct7 = 0; // unused variables;
			break;
		case 0x1B:
			// wide immediate arithmetic
			rd = 	(i >> 7) & 0b11111; // 5 bits
			funct3= (i >> 12) & 0b111; // 3 bits
			rs1 = 	(i >> 15) & 0b11111; // 5 bits;
			imm = 	(i >> 20) & 0b11111; // 12 bits;
			funct7 = (i >> 25);
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
			rs2 = 0; // unused variables;
			if((imm >> 11) & 1 ) {
				imm |= 0xFFFFF800;
			}
			// at the end: register_read(rs1, rs2, &rs1_val, &rs2_val);
			break;
		case 0x67:
			// jalr function
			rd = 	(i >> 7) & 0b11111; // 5 bits
			funct3= (i >> 12) & 0b111; // 3 bits
			rs1 = 	(i >> 15) & 0b11111; // 5 bits;
			imm = 	(i >> 20); // 12 bits;
			rs2 = 0, funct7=0; // unused variables;
			new_x_reg->class = jalr;
			// at the end: register_read(rs1, rs2, &rs1_val, &rs2_val);
			if((imm >> 11) & 1 ) {
				imm |= 0xFFFFF800;
			}
			break;
		case 0x23:
			 // stores
			funct3 = 		(i >> 12) & 0b111; // 3 bits;
			uint8_t imm_4_0 = 	(i >> 7) & 0b11111; // 5 bits;
			rs1 = 			(i >> 15) & 0b11111; // 5 bits;
			rs2 = 			(i >> 20) & 0b11111; // 5 bits;
			uint8_t imm_11_5 = 	(i >> 25); // 7 bits;
			
			imm = imm_4_0 | (imm_11_5 << 4);
			rd = 0, funct7 = 0; // unused variables.
			// at the end: register_read(rs1, rs2, &rs1_val, &rs2_val);
			// sign extension of immediate.
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
			if((imm >> 11) & 1 ) {
				imm |= 0xFFFFF800;
			}
			break;
		case 0x63:
			// branch instructions
			funct3 = 		(i >> 12) & 0b111;
			uint8_t imm_11 = 	(i >> 7) & 1; // 1 bit
			uint8_t imm_4_1 = 	(i >> 8) & 0b1111; // 4 bits;
			rs1 = 			(i >> 15) & 0b11111; 
			rs2 = 			(i >> 20) & 0b11111;
			uint8_t imm_10_5 =	(i >> 25) & 0b111111;
			uint8_t imm_12 = 	(i >> 31) & 1; // 1 bit;
			// imm[0] = 0 always. so the immediate is always even.
			imm = 0 | (imm_4_1 << 1) | (imm_10_5 << 4) | (imm_11 << 10) | (imm_12 << 11);
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
			// at the end: register_read(rs1, rs2, &rs1_val, &rs2_val);
			// sign extension of immediate
			if((imm >> 11) & 1 ) {
				imm |= 0xFFFFF800;
			}
			break;
		case 0x17:
			// lui
			new_x_reg->class = lui;
			rd = (i >> 7) & 0b11111; // 5 bits;
			imm = (i >> 12); // 20 bits;
			
			// sign extension of 20-bit immediate
			if((imm >> 20) & 1 ) {
				imm |= 0xFFF00000;
			}
			rs1 = 0, rs2 = 0, funct3 = 0, funct7 = 0;
			break;
		case 0x37:
			// auipc
			new_x_reg->class = auipc;
			
			rd =  (i >> 7) & 0b11111; // 5 bits;
			imm = (i >> 12); // 20 bits;
			
			// sign extension of 20-bit immediate
			if((imm >> 20) & 1 ) {
				imm |= 0xFFF00000;
			}
			rs1 = 0, rs2 = 0, funct3 = 0, funct7 = 0;
			break;
		case 0x6F:
			// jal
			new_x_reg->class = jal;
			rd = 			(i >> 7)  & 0b11111; // 5 bits
			uint8_t imm_19_12 = 	(i >> 12) & 0b11111111; // 8 bits;
			imm_11 =		(i >> 20) & 0b1;
			uint16_t imm_10_1 = 	(i >> 21) & 0b1111111111; // 10 bits;
			uint8_t imm_20 = 	(i >> 31) & 0b1; 
			imm = 0 | (imm_20 << 19) | (imm_19_12 << 11) | (imm_11 << 10) | (imm_10_1 << 1);
			// sign extension
			if((imm >> 20) & 1 ) {
				imm |= 0xFFF00000;
			}
			rs1 = 0, rs2 = 0, funct3 = 0, funct7 = 0;
			break;	
		case 0x33:
			// arithmetic
			rd = 		(i >> 7) & 0b11111;
			funct3 = 	(i >> 12) & 0b111;
			rs1 = 		(i >> 15) & 0b11111;
			rs2 = 		(i >> 20) & 0b11111;
			funct7 = 	(i >> 25);
			// at the end: register_read(rs1, rs2, &rs1_val, &rs2_val);
			imm = 0;		
			if(funct7 != 0x1) 
			{ 
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
			// at the end: register_read(rs1, rs2, &rs1_val, &rs2_val);
			imm = 0;		
			break;
		default:
			new_x_reg->class = addi; // NOP.
			rd = 0, funct3 = 0, rs1 = 0, rs2 = 0, funct7 = 0, imm = 0;
			break;
	}
	#ifdef DEBUG1
		printf("rs1 = %d, rs2 = %d, rd = %d, imm = %d funct3 = %d, funct7 = %d, opcode: 0x%X. ", rs1, rs2, rd, imm, funct3, funct7,opcode);
		switch(new_x_reg->class)
		{
			case r_w_type:
				printf("Wide ");
			case r_type:
				printf("R-type instruction called.\r\n");
				break;
			case i_w_type:
				printf("Wide ");
			case i_type:
				printf("I-type instruction called.\r\n");
				break;
			case sb_type:
				printf("Conditional ");
			case i_j_type:
			case u_j_type:
				printf("Branch instruction called.\r\n");
				break;
			case u_type:
				printf("LUI instruction called.\r\n");
				break;
			case u_a_type:
				printf("AUIPC instruction called.\r\n");
				break;
			case i_l_type:
				printf("Load instruction called.\r\n");
				break;
			case s_type: 
				printf("Store instruction called.\r\n");
				break;
			case 'Z':
				printf("NOT AN INSTRUCTION.\r\n");
				break;
			default:
				printf("WTF. This is the type called: %c\r\n", new_x_reg->class);
				break;
		}
	#endif
	new_x_reg->instruction = i; // must pass things here.
	// reading value from rs1 & rs2;
	// these values are passed for loading purposes.	

	// calculating what values to use here.
	// forwarding what registers to write to in execute.
	new_x_reg->rs1 = rs1;
	new_x_reg->rs2 = rs2;
	// if the next two stages are currently trying to calculate the value of rd,
    // then stall until it has the correct value.
	uint16_t x_rd = current_stage_x_register->rd;
	uint16_t m_rd = current_stage_m_register->rd;
	
	register_read(rs1, rs2, &rs1_val, &rs2_val);
	if(x_rd == rs1)
	{
		new_x_reg->rs1_val = 0;
		new_x_reg->STALL = true;
	}
	if(x_rd == rs2)
	{
		new_x_reg->rs2_val = 0;
		new_x_reg->STALL = true;
	
	}
	if(m_rd == rs1) {
		new_x_reg->rs1 = 0;
		new_x_reg->STALL = true;
	
	}
	if(m_rd == rs2){
		new_x_reg->rs2_val = 0;
		new_x_reg->STALL = true;
	} else {
		new_x_reg->STALL = false;
		new_x_reg->rs1_val = rs1_val;
		new_x_reg->rs2_val = rs2_val;
	}
	// pass along these values... 
	new_x_reg->new_pc = current_stage_d_register->new_pc; // keep track of what pc
                                                          // this instruction is 
                                                          // located at.
	// implement execute stalling here.

}
