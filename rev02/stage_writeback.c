#include "stage_writeback.h"

void stage_writeback(void)
{
	// protection against void instructions.
	if(current_stage_w_register == NULL || current_stage_w_register->NOP == true)
	{
		return;
	}
	suppress_btb_size_unused_warning();
	char type = current_stage_w_register->class;
	(void) btb_size; // Compiler warning suppression for global variable.
	if(current_stage_w_register->instruction == 0) { return; } // protection against bad instructions.
	register_write(current_stage_w_register->rd, current_stage_w_register->value);

	#ifdef DEBUG
	printf("Writeback stage, instruction = 0x%x \r\n", current_stage_w_register->instruction);
	#endif
}
