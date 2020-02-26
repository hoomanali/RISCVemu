#include "riscv_pipeline_registers.h"
#include "riscv_pipeline_registers_vars.h"
#include "riscv_sim_framework.h"
#include "global_variables.h"

void stage_writeback (void) {
    uint32_t i = current_stage_w_register->instruction; // Received instruction
    uint64_t pc = current_stage_w_register->pc;         // Program counter of received instruction
    uint16_t rd = current_stage_w_register->rd;         // Destination register to write to
    uint64_t value = current_stage_w_register->value;   // Value to write

    // Don't write anything if a stall is initiated or writeback is killed
    if (current_stage_w_register->NOP == true || current_stage_m_register->terminateWriteback == true){
        return;
    }

    // Write value to destination register
    // printf("Writeback stage, instruction = 0x%x, writing value %d to register %d\r\n", i, value, rd);
    register_write(rd, value);
}
