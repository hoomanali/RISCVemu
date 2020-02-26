/*
 * tlb_interface.h
 */

#include "global_variables.h"
#include "riscv_sim_framework.h"
#include "riscv_pipeline_registers.h"
#include "riscv_pipeline_registers_vars.h"

/*
 * Function tlb_get_tag()
 *
 * Arguments: 32 bit virtual address, stage flag
 * Return:    physical TAG
 */
uint32_t tlb_get_tag(uint32_t v_address, char stage);

/*
 * Function tlb_is_entry_valid()
 *
 * Arguments: 32 bit virtual address, stage flag
 * Return:    True if valid bit is set, false if not
 */
bool tlb_is_entry_valid(int index, char stage);

/*
 * Function tlb_is_entry_dirty()
 *
 * Arguments: 32 bit virtual address, stage flag
 * Return:    True if entry is dirty, false if not
 */
bool tlb_is_entry_dirty(int index, char stage);

/*
 * Function tlb_add_entry()
 *
 * Arguments: 32 bit virtual address, stage flag
 * Return:    True if success, false if not
 */
bool tlb_add_entry(uint32_t v_address, char stage);

/*
 * Function tlb_find_entry()
 *
 * Return true if found.
 */
bool tlb_find_entry(uint32_t v_address, char stage);
