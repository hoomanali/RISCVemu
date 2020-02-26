#include "global_variables.h"
#include "riscv_sim_framework.h"
#include "riscv_pipeline_registers.h"
#include "riscv_pipeline_registers_vars.h"
/*
 * function virtual_to_physical:
 *
 * Arguments: Virtual Page Number, Offset
 * Results: The physical address.
 *
 */
uint64_t virtual_to_physical(uint32_t vpn, uint16_t offset);
