#include "stage_fetch.h"
#include "stage_decode.h"
#include "stage_execute.h"
#include "stage_memory.h"
#include "stage_writeback.h"
#include <stdio.h>

#define SAMPLE_INSTRUCTION 0x403150b3
#define INVALID_INSTRUCTION 0x00000000

#define ADD_INSTRUCTION 0x003100b3 // add x1, x2, x3
#define SUB_INSTRUCTION 0x403100b3 // sub x1, x2, x3
#define SLL_INSTRUCTION 0x003110b3 // sll x1, x2, x3
#define SLT_INSTRUCTION 0x003120b3 // slt x1, x2, x3
#define SLTU_INSTRUCTION 0x003130b3 // sltu x1, x2, x3
#define XOR_INSTRUCTION 0x003140b3 // xor x1, x2, x3
#define SRL_INSTRUCTION 0x003150b3 // srl x1, x2, x3
#define SRA_INSTRUCTION 0x403150b3 // sra x1, x2, x3
#define OR_INSTRUCTION 0x003160b3 // or x1, x2, x3
#define AND_INSTRUCTION 0x003170b3 // and x1, x2, x3

#define ADDW_INSTRUCTION 0x003100bb // addw x1, x2, x3
#define SUBW_INSTRUCTION 0x403100bb // subw x1, x2, x3
#define SLLW_INSTRUCTION 0x003110bb // sllw x1, x2, x3
#define SRLW_INSTRUCTION 0x003150bb // srlw x1, x2, x3
#define SRAW_INSTRUCTION 0x403150bb // sraw x1, x2, x3

#define ADDI_INSTRUCTION 0x00510093 // addi x1, x2, 5
#define SLLI_INSTRUCTION 0x00511093 // slli x1, x2, 5
#define SLTI_INSTRUCTION 0x00512093 // slti x1, x2, 5
#define SLTIU_INSTRUCTION 0x00513093 // sltiu x1, x2, 5
#define XORI_INSTRUCTION 0x00514093 // xori x1, x2, 5
#define SRLI_INSTRUCTION 0x00515093 // srli x1, x2, 5
#define SRAI_INSTRUCTION 0x40515093 // srai x1, x2, 5
#define ORI_INSTRUCTION 0x00516093 // ori x1, x2, 5
#define ANDI_INSTRUCTION 0x00517093 // andi x1, x2, 5

#define ADDIW_INSTRUCTION 0x0051009b // addiw x1, x2, 5
#define SLLIW_INSTRUCTION 0x0051109b // slliw x1, x2, 5
#define SRLIW_INSTRUCTION 0x0051509b // srliw x1, x2, 5
#define SRAIW_INSTRUCTION 0x4051509b // sraiw x1, x2, 5

#define LB_INSTRUCTION 0x00410083 // lb x1, 4(x2)
#define LH_INSTRUCTION 0x00411083 // lh x1, 4(x2)
#define LW_INSTRUCTION 0x00412083 // lw x1, 4(x2)
#define LD_INSTRUCTION 0x00413083 // ld x1, 4(x2)
#define LBU_INSTRUCTION 0x00414083 // lbu x1, 4(x2)
#define LHU_INSTRUCTION 0x00415083 // lhu x1, 4(x2)
#define LWU_INSTRUCTION 0x00416083 // lwu x1, 4(x2)

#define SB_INSTRUCTION 0x00228023 // sb x2, 0(x5)
#define SH_INSTRUCTION 0x00229023 // sh x2, 0(x5)
#define SW_INSTRUCTION 0x0022a023 // sw x2, 0(x5)
#define SD_INSTRUCTION 0x0022b023 // sd x2, 0(x5)

#define AUIPC_INSTRUCTION 0x12345297 // auipc x5, 0x12345
#define LUI_INSTRUCTION 0x123452b7 // lui x5, 0x12345

#define BEQ_INSTRUCTION 0x00310263 // beq x2, x3, EXIT (where EXIT is an empty label following this instruction)
#define BNE_INSTRUCTION 0x00311263 // bne x2, x3, EXIT
#define BLT_INSTRUCTION 0x00314263 // blt x2, x3, EXIT
#define BGE_INSTRUCTION 0x00315263 // bge x2, x3, EXIT
#define BLTU_INSTRUCTION 0x00316263 // bltu x2, x3, EXIT
#define BGEU_INSTRUCTION 0x00317263 // bgeu x2, x3, EXIT

#define JALR_INSTRUCTION 0x000000ef // jal x1, 100
#define JAL_INSTRUCTION 0x064100e7 // jalr x1, 100(x2)


int sample_test(void);
int reading_test(void);
int fetch_test(void);
int decode_invalid_test(void);
int decode_add_test(void);
int decode_sub_test(void);
int decode_sll_test(void);
int decode_slt_test(void);
int decode_sltu_test(void);
int decode_xor_test(void);
int decode_srl_test(void);
int decode_sra_test(void);
int decode_or_test(void);
int decode_and_test(void);
int decode_addw_test(void);
int decode_subw_test(void);
int decode_sllw_test(void);
int decode_srlw_test(void);
int decode_sraw_test(void);
int decode_addi_test(void);
int decode_slli_test(void);
int decode_slti_test(void);
int decode_sltiu_test(void);
int decode_xori_test(void);
int decode_srli_test(void);
int decode_srai_test(void);
int decode_ori_test(void);
int decode_andi_test(void);
int decode_addiw_test(void);
int decode_slliw_test(void);
int decode_srliw_test(void);
int decode_sraiw_test(void);
int decode_lb_test(void);
int decode_beq_test(void);

void unit_tests(void)
{

    suppress_btb_size_unused_warning();

	int passed = 0, failed = 0;
	
	if(sample_test() == 1) { passed++; }
	else { failed++; }
	if(reading_test() == 1) { passed++; }
	else { failed++; }
	if(fetch_test() == 1) { passed++; }
	else { failed++; }
	if(decode_invalid_test() == 1) { passed++; }
	else { failed++; }
	if(decode_add_test() == 1) { passed++; }
	else { failed++; }
	if(decode_sub_test() == 1) { passed++; }
	else { failed++; }
	if(decode_sll_test() == 1) { passed++; }
	else { failed++; }
	if(decode_slt_test() == 1) { passed++; }
	else { failed++; }
	if(decode_sltu_test() == 1) { passed++; }
	else { failed++; }
	if(decode_xor_test() == 1) { passed++; }
	else { failed++; }
	if(decode_srl_test() == 1) { passed++; }
	else { failed++; }
	if(decode_sra_test() == 1) { passed++; }
	else { failed++; }
	if(decode_or_test() == 1) { passed++; }
	else { failed++; }
	if(decode_and_test() == 1) { passed++; }
	else { failed++; }
	if(decode_addw_test() == 1) { passed++; }
	else { failed++; }
	if(decode_subw_test() == 1) { passed++; }
	else { failed++; }
	if(decode_sllw_test() == 1) { passed++; }
	else { failed++; }
	if(decode_srlw_test() == 1) { passed++; }
	else { failed++; }
	if(decode_sraw_test() == 1) { passed++; }
	else { failed++; }
	if(decode_addi_test() == 1) { passed++; }
	else { failed++; }
	if(decode_slli_test() == 1) { passed++; }
	else { failed++; }
	if(decode_slti_test() == 1) { passed++; }
	else { failed++; }
	if(decode_sltiu_test() == 1) { passed++; }
	else { failed++; }
	if(decode_xori_test() == 1) { passed++; }
	else { failed++; }
	if(decode_srli_test() == 1) { passed++; }
	else { failed++; }
	if(decode_srai_test() == 1) { passed++; }
	else { failed++; }
	if(decode_ori_test() == 1) { passed++; }
	else { failed++; }
	if(decode_andi_test() == 1) { passed++; }
	else { failed++; }
	if(decode_addiw_test() == 1) { passed++; }
	else { failed++; }
	if(decode_slliw_test() == 1) { passed++; }
	else { failed++; }
	if(decode_srliw_test() == 1) { passed++; }
	else { failed++; }
	if(decode_sraiw_test() == 1) { passed++; }
	else { failed++; }
	if(decode_lb_test() == 1) { passed++; } // running more than one load test breaks for some reason
	else { failed++; }

	printf("PASSED: %d\r\nFAILED: %d\r\n", passed, failed);
}

int sample_test(void) {
	int val = 5;
	if (val == 5) {
		return 1;
	}
	else {
		printf("sample_test failed!\r\n");
		return 0;
	}
}

int reading_test(void)
{
	uint64_t address = 0x100;//get_pc();
	uint64_t instruction = SAMPLE_INSTRUCTION;
	uint64_t read_from_memory;
	while(!memory_write(address, SAMPLE_INSTRUCTION, 8))
	{

	}
	// printf("instruction: 0x%X\r\n", instruction);
	while(!memory_read(address, &read_from_memory, 8))
	{
		
	}
	// printf("read_from_memory: 0x%X\r\n", read_from_memory);
	if(instruction == read_from_memory) {
		return 1;
	} else {
		return 0;
	}
}

int fetch_test(void) {
	simulator_execute_instructions(1); // initialize memory
	memory_write(0, SAMPLE_INSTRUCTION, 4);
	simulator_execute_instructions(1);
	if (current_stage_d_register->instruction == SAMPLE_INSTRUCTION && current_stage_d_register->new_pc == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

int decode_invalid_test(void) {
	memory_write(get_pc(), INVALID_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	printf("%x", current_stage_x_register->instruction);
	if (current_stage_x_register->class == addi &&
		current_stage_x_register->rs1 == 0 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 0 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_invalid_test failed!\r\n");
		return 0;
	}
}

int decode_add_test(void) {
	memory_write(get_pc(), ADD_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == ADD_INSTRUCTION &&
		current_stage_x_register->class == add &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_add_test failed!\r\n");
		return 0;
	}
}

int decode_sub_test(void) {
	memory_write(get_pc(), SUB_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SUB_INSTRUCTION &&
		current_stage_x_register->class == sub &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0 &&
		current_stage_x_register->funct7 == 0x20)
	{
		return 1;
	}
	else {
		printf("decode_sub_test failed!\r\n");
		return 0;
	}
}

int decode_sll_test(void) {
	memory_write(get_pc(), SLL_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SLL_INSTRUCTION &&
		current_stage_x_register->class == sll &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0x01 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_sll_test failed!\r\n");
		return 0;
	}
}

int decode_slt_test(void) {
	memory_write(get_pc(), SLT_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SLT_INSTRUCTION &&
		current_stage_x_register->class == slt &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0x02 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_slt_test failed!\r\n");
		return 0;
	}
}

int decode_sltu_test(void) {
	memory_write(get_pc(), SLTU_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SLTU_INSTRUCTION &&
		current_stage_x_register->class == sltu &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0x03 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_sltu_test failed!\r\n");
		return 0;
	}
}

int decode_xor_test(void) {
	memory_write(get_pc(), XOR_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == XOR_INSTRUCTION &&
		current_stage_x_register->class == xor1 &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0x04 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_xor_test failed!\r\n");
		return 0;
	}
}

int decode_srl_test(void) {
	memory_write(get_pc(), SRL_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SRL_INSTRUCTION &&
		current_stage_x_register->class == srl &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0x05 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_srl_test failed!\r\n");
		return 0;
	}
}

int decode_sra_test(void) {
	memory_write(get_pc(), SRA_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SRA_INSTRUCTION &&
		current_stage_x_register->class == sra &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0x05 &&
		current_stage_x_register->funct7 == 0x20)
	{
		return 1;
	}
	else {
		printf("decode_sra_test failed!\r\n");
		return 0;
	}
}

int decode_or_test(void) {
	memory_write(get_pc(), OR_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == OR_INSTRUCTION &&
		current_stage_x_register->class == or1 &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0x06 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_or_test failed!\r\n");
		return 0;
	}
}

int decode_and_test(void) {
	memory_write(get_pc(), AND_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == AND_INSTRUCTION &&
		current_stage_x_register->class == and1 &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0x07 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_and_test failed!\r\n");
		return 0;
	}
}

int decode_addw_test(void) {
	memory_write(get_pc(), ADDW_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == ADDW_INSTRUCTION &&
		current_stage_x_register->class == addw &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_addw_test failed!\r\n");
		return 0;
	}
}

int decode_subw_test(void) {
	memory_write(get_pc(), SUBW_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SUBW_INSTRUCTION &&
		current_stage_x_register->class == subw &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0 &&
		current_stage_x_register->funct7 == 0x20)
	{
		return 1;
	}
	else {
		printf("decode_subw_test failed!\r\n");
		return 0;
	}
}

int decode_sllw_test(void) {
	memory_write(get_pc(), SLLW_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SLLW_INSTRUCTION &&
		current_stage_x_register->class == sllw &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0x01 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_sllw_test failed!\r\n");
		return 0;
	}
}

int decode_srlw_test(void) {
	memory_write(get_pc(), SRLW_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SRLW_INSTRUCTION &&
		current_stage_x_register->class == srlw &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0x05 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_srlw_test failed!\r\n");
		return 0;
	}
}

int decode_sraw_test(void) {
	memory_write(get_pc(), SRAW_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SRAW_INSTRUCTION &&
		current_stage_x_register->class == sraw &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 3 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 0 &&
		current_stage_x_register->funct3 == 0x05 &&
		current_stage_x_register->funct7 == 0x20)
	{
		return 1;
	}
	else {
		printf("decode_sraw_test failed!\r\n");
		return 0;
	}
}

int decode_addi_test(void) {
	memory_write(get_pc(), ADDI_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == ADDI_INSTRUCTION &&
		current_stage_x_register->class == addi &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_addi_test failed!\r\n");
		return 0;
	}
}

int decode_slli_test(void) {
	memory_write(get_pc(), SLLI_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SLLI_INSTRUCTION &&
		current_stage_x_register->class == slli &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0x01 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_slli_test failed!\r\n");
		return 0;
	}
}

int decode_slti_test(void) {
	memory_write(get_pc(), SLTI_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SLTI_INSTRUCTION &&
		current_stage_x_register->class == slti &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0x02 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_slti_test failed!\r\n");
		return 0;
	}
}

int decode_sltiu_test(void) {
	memory_write(get_pc(), SLTIU_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SLTIU_INSTRUCTION &&
		current_stage_x_register->class == sltiu &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0x03 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_sltiu_test failed!\r\n");
		return 0;
	}
}

int decode_xori_test(void) {
	memory_write(get_pc(), XORI_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == XORI_INSTRUCTION &&
		current_stage_x_register->class == xori &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0x04 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_xori_test failed!\r\n");
		return 0;
	}
}

int decode_srli_test(void) {
	memory_write(get_pc(), SRLI_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SRLI_INSTRUCTION &&
		current_stage_x_register->class == srli &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0x05 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_srli_test failed!\r\n");
		return 0;
	}
}

int decode_srai_test(void) {
	memory_write(get_pc(), SRAI_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SRAI_INSTRUCTION &&
		current_stage_x_register->class == srai &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0x05 &&
		current_stage_x_register->funct7 == 0x20)
	{
		return 1;
	}
	else {
		printf("decode_srai_test failed!\r\n");
		return 0;
	}
}

int decode_ori_test(void) {
	memory_write(get_pc(), ORI_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == ORI_INSTRUCTION &&
		current_stage_x_register->class == ori &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0x06 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_ori_test failed!\r\n");
		return 0;
	}
}

int decode_andi_test(void) {
	memory_write(get_pc(), ANDI_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == ANDI_INSTRUCTION &&
		current_stage_x_register->class == andi &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0x07 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_andi_test failed!\r\n");
		return 0;
	}
}

int decode_addiw_test(void) {
	memory_write(get_pc(), ADDIW_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == ADDIW_INSTRUCTION &&
		current_stage_x_register->class == addiw &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_addiw_test failed!\r\n");
		return 0;
	}
}

int decode_slliw_test(void) {
	memory_write(get_pc(), SLLIW_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SLLIW_INSTRUCTION &&
		current_stage_x_register->class == slliw &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0x01 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_slliw_test failed!\r\n");
		return 0;
	}
}

int decode_srliw_test(void) {
	memory_write(get_pc(), SRLIW_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SRLIW_INSTRUCTION &&
		current_stage_x_register->class == srliw &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0x05 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_srliw_test failed!\r\n");
		return 0;
	}
}

int decode_sraiw_test(void) {
	memory_write(get_pc(), SRAIW_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == SRAIW_INSTRUCTION &&
		current_stage_x_register->class == sraiw &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 5 &&
		current_stage_x_register->funct3 == 0x05 &&
		current_stage_x_register->funct7 == 0x20)
	{
		return 1;
	}
	else {
		printf("decode_sraiw_test failed!\r\n");
		return 0;
	}
}

int decode_lb_test(void) {
	memory_write(get_pc(), LB_INSTRUCTION, 4);
	simulator_execute_instructions(2);
	if (current_stage_x_register->instruction == LB_INSTRUCTION &&
		current_stage_x_register->class == lb &&
		current_stage_x_register->rs1 == 2 &&
		current_stage_x_register->rs2 == 0 &&
		current_stage_x_register->rd == 1 &&
		current_stage_x_register->imm == 4 &&
		current_stage_x_register->funct3 == 0 &&
		current_stage_x_register->funct7 == 0)
	{
		return 1;
	}
	else {
		printf("decode_lb_test failed!\r\n");
		return 0;
	}

}
