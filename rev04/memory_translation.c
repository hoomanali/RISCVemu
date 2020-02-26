#include "memory_translation.h"
#include <stdio.h>
#include "riscv_pipeline_registers.h"


/*
 *Function root_PTE
 *Returns the root PTE entry necessary
 */

uint32_t root_PTE_index(uint32_t VPN)
{
	uint32_t root_PTE = get_ptbr() + (VPN >> 22);
	uint32_t root_PTE_val;
	memory_read(root_PTE, &root_PTE_val, 4);
	if(!(root_PTE_val >> 31)) // valid bit 
	{
		printf("root PTE entry not valid. exiting.\r\n");
	} 
	return (root_PTE_val & 0x7FFF);
}
/*
 * Function second_PTE_index
 * Returns the second page table entry we want.
 */
uint32_t second_PTE_index(uint32_t root_PTE, uint32_t VPN)
{
	
	uint32_t second_PTE = root_PTE + (VPN >> 12);
	uint32_t second_PTE_val;
	memory_read(second_PTE, &second_PTE_val, 4);
	if(!(second_PTE_val >> 31)) // valid bit
	{
		printf("Invalid PTE at second level PT.\r\n");
	}
	return (second_PTE_val & 0x7FFF);
}
/*
 *Function memory_translation
 *
 * Arguments: a 2nd level PTE entry.
 *
 * Returns: a pointer to a physical memory address
 */

uint64_t memory_translation(uint32_t second_PTE, uint16_t offset )
{
	return second_PTE << offset;
}
/*
 * function virtual_to_physical:
 *
 * Arguments: Virtual Page Number, Offset
 * Results: The physical address.
 *
 */
uint64_t virtual_to_physical(uint32_t VPN, uint16_t offset)
{
	uint32_t r = root_PTE_index(VPN);
	uint32_t s = second_PTE_index(r, VPN);
	uint64_t a = memory_translation(s, offset);
	return a;
}
