/*
 * tlb_interface.c
 */

#include "global_variables.h"
#include "riscv_sim_framework.h"
#include "riscv_pipeline_registers.h"
#include "riscv_pipeline_registers_vars.h"
#include "memory_translation.h"

/*
 *  Bit masks for pulling values from address
 *  [TAG | Index | Offset]
 */
#define TAG_I_MASK      0x0000000000FFFFFF
#define INDEX_I_MASK    0x000001FFFF000000
#define TAG_D_MASK      0xFFFF200000000000
#define INDEX_D_MASK    0x00001FFFFF000000
#define OFFSET_MASK     0x0000000000FFFFFF

/*
 * Function tlb_get_tag()
 *
 * Arguments: 32 bit virtual address, stage flag
 * Return:    physical TAG
 */
uint32_t tlb_get_tag(uint32_t v_address, char stage) {
	switch(stage)
	{
		case 'F':
        case 'f':
            return v_address >> 22;
			break;
		case 'M':
        case 'm':
            return v_address >> 22;
			break;
		default:
			return 0;
			break;
	}
}

/*
 * Function tlb_is_entry_valid()
 *
 * Arguments: TLB index, stage flag
 * Return:    True if valid bit is set, false if not
 */
bool tlb_is_entry_valid(int index, char stage) {
    switch(stage) {
        // In fetch, use I-TLB
        case 'F':
        case 'f':
            return I_TLB[index].valid;
            break;
        // In memory, use D-TLB
        case 'M':
        case 'm':
            return D_TLB[index].valid;
            break;
        default:
            // Do nothing
            return false;
            break;
    }
}

/*
 * Function tlb_is_entry_dirty()
 *
 * Arguments: 32 bit virtual address, stage flag
 * Return:    True if entry is dirty, false if not
 */
bool tlb_is_entry_dirty(int index, char stage) {
    switch(stage) {
        // In fetch, use I-TLB
        case 'F':
        case 'f':
            return I_TLB[index].dirty;
            break;
        // In memory, use D-TLB
        case 'M':
        case 'm':
            return D_TLB[index].dirty;
            break;
        default:
            // Do nothing
            return false;
            break;
    }
}

/*
 * Function tlb_add_entry()
 *
 * Arguments: 32 bit virtual address, stage flag
 * Return:    True if success, false if not
 */
bool tlb_add_entry(uint32_t v_address, char stage) {
	int i_tlb_index = 0;
	int d_tlb_index = 0;
	switch(stage)
	{
		case 'F':
		case 'f':
		{
			struct TLB_entry i;
			i.vpn_index = (v_address >> 12) & 0x1FF;
			i.vpn_tag = (v_address >> 21) & 0x3FF;
			i.page_offset = v_address & 0xFFF;
			I_TLB[i_tlb_index++ % 8] = i;
			return true;
		}
		case 'M':
		case 'm':
		{
			struct TLB_entry i;
			i.vpn_index = (v_address >> 12) & 0x3FF;
			i.vpn_tag = (v_address >> 21) & 0x1FF;
			i.page_offset = v_address & 0xFFF;
			D_TLB[d_tlb_index++ % 8] = i;
			return true;
		}
		default:
			return false;	
	}
	return false;
}

/*
 * Function tlb_find_entry()
 *
 * Arguments: 32 bit virtual address, stage flag
 * Return:    True if success, false if not
 */
bool tlb_find_entry(uint32_t v_address, char stage) {
	static int i_tlb_index = 0;
	static int d_tlb_index = 0;
	switch(stage)
	{
		case 'F':
		case 'f':
		{
            uint32_t pageNumber = (v_address >> 12) & 0x3FF;
            for(i_tlb_index; i_tlb_index < 8; i_tlb_index++) {
                if(I_TLB[i_tlb_index].vpn_index == pageNumber )
                return true;
            }
			return false;
		}
		case 'M':
		case 'm':
		{
            uint32_t pageNumber = (v_address >> 12) & 0x3FF;
            for(d_tlb_index; d_tlb_index < 8; d_tlb_index++) {
                if(D_TLB[d_tlb_index].vpn_index == pageNumber )
                return true;
            }
			return false;
		}
		default:
			return false;	
	}
	return false;
}
