/*
 * External routines for use in CMPE 110, Fall 2018
 *
 * (c) 2018 Ethan L. Miller
 * Redistribution not permitted without explicit permission from the copyright holder.
 *
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

extern bool memory_read (uint64_t address, void *value, uint64_t size_in_bytes);
extern bool memory_write (uint64_t address, uint64_t value, uint64_t size_in_bytes);
extern bool memory_status (uint64_t address, uint64_t *value);

extern void register_read (uint64_t register_a, uint64_t register_b, uint64_t * value_a, uint64_t * value_b);
extern void register_write (uint64_t register_d, uint64_t value_d);

/*
 * This is the function that students have to implement for Assignment 1.
 */
extern void execute_single_instruction (uint64_t pc, uint64_t * new_pc);
