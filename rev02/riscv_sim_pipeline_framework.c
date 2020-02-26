/*
 *
 * Instructor-supplied code for CMPE 110, Fall 2018.
 *
 * You may not modify this file.  We're going to supply our own version of it for grading,
 * so any changes that you make will be wiped away when we test your code.
 *
 * Your Makefile should compile this code along with your own code to build the simulator.
 * Note that the main() function is included in this file.  As detailed in the assignment,
 * you're writing a function that executes a single instruction.
 *
 * You're welcome to look at the code to see what we did and how.  
 *
 * (c) 2018 Ethan L. Miller
 * Redistribution not permitted without explicit permission from the copyright holder.
 *
 */

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef  HAS_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif
#include "riscv_sim_pipeline_framework.h"

#define		MEMORY_MAX_SIZE		(32 * 1024 * 1024)		/* 32 MB maximum memory size */
#define		MEMORY_PAGE_SIZE	(4096)					/* Memory size must be a multiple of page size */
typedef     unsigned long long ull;

/* Instruction execution function provided by students */
extern void execute_single_instruction(const uint64_t pc, uint64_t *new_pc);

static uint8_t *	riscv_mem;
static uint64_t		riscv_mem_size = 0;
static uint64_t     program_counter = 0;

/******************************************************************************************
 *
 * memory_initialize
 *
 * Sets up memory for the simulator.
 *
 * Parameter: size_in_bytes
 *            Must be a multiple of MEMORY_PAGE_SIZE and no larger than MEMORY_MAX_SIZE
 *
 *****************************************************************************************/
void memory_initialize (uint64_t size_in_bytes)
{
	if (size_in_bytes > MEMORY_MAX_SIZE || size_in_bytes % MEMORY_PAGE_SIZE != 0) {
		exit (1);
	}
	riscv_mem = malloc (size_in_bytes);
	if (riscv_mem == NULL) {
		exit (1);
	}
    memset (riscv_mem, 0, size_in_bytes);
	riscv_mem_size = size_in_bytes;
}

static inline
void
set_pc_internal (uint64_t pc)
{
    program_counter = pc;
}

void
set_pc (uint64_t pc)
{
    set_pc_internal (pc);
}


static inline
uint64_t
get_pc_internal (void)
{
    return program_counter;
}

uint64_t
get_pc (void)
{
    return get_pc_internal ();
}

/******************************************************************************************
 *
 * memory_load
 * memory_dump
 *
 * Functions to load values into memory or dump values out of memory.
 *
 *****************************************************************************************/
static
inline
bool memory_load (const void * region, uint64_t base, uint64_t size)
{
	if (base + size > riscv_mem_size) {
		return false;
	}
	memcpy (riscv_mem + base, region, size);
	return (true);
}

static
inline
bool memory_dump (void * region, uint64_t base, uint64_t size)
{
	if (base + size > riscv_mem_size) {
		return (false);
	}
	memcpy (region, riscv_mem + base, size);
    return true;
}

/******************************************************************************************
 *
 * memory_read
 *
 * Read a single value from memory.  "Improper" reads, such as those that read unaligned
 * or those that read outside the defined memory region, will return unpredictable values.
 *
 * Parameters:
 *      address         : offset of the start of the read
 *      value           : value returned by the read
 *      size_in_bytes   : size of the read (1, 2, 4, or 8 bytes)
 *
 * Returns:
 *      bool            : true if read is finished, false if not
 *
 *****************************************************************************************/
static uint32_t     memory_cycle_reads = 0;
static uint32_t     memory_cycle_writes = 0;

bool memory_read (uint64_t address, void *value, uint64_t size_in_bytes)
{
    if (size_in_bytes > 8 || __builtin_popcountll (size_in_bytes) != 1 ||
        address + size_in_bytes > riscv_mem_size || address % size_in_bytes != 0) {
        *(uint8_t *)value = 0;
        return true;
    }
    if (memory_cycle_reads + memory_cycle_writes >= 2) {
        *(uint8_t *)value = 0;
        return true;
    }
    memory_cycle_reads += 1;
    memory_dump (value, address, size_in_bytes);
    return true;
}


bool memory_write (uint64_t address, uint64_t value, uint64_t size_in_bytes)
{
    if (size_in_bytes > 8 || __builtin_popcountll (size_in_bytes) != 1 ||
        address + size_in_bytes > riscv_mem_size || address % size_in_bytes != 0) {
        return true;
    }
    if (memory_cycle_reads >= 2 || memory_cycle_writes >= 1) {
        return true;
    }
    memory_cycle_writes += 1;
    /* this only works on little-endian systems */
    memory_load (&value, address, size_in_bytes);
    return true;
}

bool memory_status (uint64_t address, uint64_t * value)
{
    return true;
}

static void memory_reset_cycle ()
{
    memory_cycle_reads = 0;
    memory_cycle_writes = 0;
}

/******************************************************************************************
 *
 * Register file routines
 *
 * These routines are used to manager the register file.  We can preallocate the register
 * file, since it's a known size (32 registers, each 64 bits long).  There's no need to
 * initialize it to zero, however, since the CPU can start up in an unknown state.
 *
 *
 *****************************************************************************************/

#define RISCV_NUM_REGISTERS         32

static uint64_t     register_file[RISCV_NUM_REGISTERS];
static uint32_t     register_cycle_reads = 0;
static uint32_t     register_cycle_writes = 0;

static inline
uint64_t register_read_one (uint64_t reg)
{
    reg %= RISCV_NUM_REGISTERS;
    return (reg > 0) ? register_file[reg] : 0ULL;
}

void register_read (uint64_t register_a, uint64_t register_b, uint64_t * value_a, uint64_t * value_b)
{
    *value_a = *value_b = 0ULL;

    if (register_cycle_reads > 1) {
        return;
    }
    *value_a = register_read_one (register_a);
    *value_b = register_read_one (register_b);

}

void register_write (uint64_t register_d, uint64_t value_d)
{
    if (register_cycle_writes > 1) {
        return;
    }
    register_d %= RISCV_NUM_REGISTERS;
    register_file[register_d] = value_d;
}

static void register_reset_cycle ()
{
    register_cycle_reads = 0;
    register_cycle_writes = 0;
}

/******************************************************************************************
 *
 * Main simulator loop
 *
 * These routines include the main simulator loop, which repeatedly calls the students'
 * execute_single_instruction() function.  The new_pc computed by the students' function
 * is copied to the PC, and then the function is called again.
 * 
 * We need to check to see whether to end the simulation; we'll use the EBREAK instruction
 * to do this.  The execute_single_instruction() doesn't need to interpret it, since the
 * simulator can do it before calling to execute an instruction.
 *
 *
 * Simulator commands:
 * load     <address> <filename>
 * dump     <address> <length> [filename]
 * writereg <register> <value>
 * readreg  <register>
 * setpc    <program_counter>
 * getpc    [/x]
 * run      <steps>
 *
 * File format defaults to direct binary.  If you want to read or write hex format,
 * append "/x" to the command with a space after it (e.g., load /x, read /x).  Addresses
 * and steps can be in decimal or hex, with hex preceded by 0x.  Filename may be omitted,
 * in which case output is to stdout (the screen).
 *
 *****************************************************************************************/

#define             RISCV_INSTR_EBREAK      0x00100073
#define             SIM_MAX_LINE            4096

static char         cmdsep[] = " \t\n\r";
static uint64_t     memory_size = 8 * 1024 * 1024;
static const char * prog_name;

/*
 * The pipeline registers are defined even if no pipelining is needed.
 * This is done so we don't get undefined function and data structure
 * errors.
 */
struct stage_reg_d cur_d_reg;
struct stage_reg_x cur_x_reg;
struct stage_reg_m cur_m_reg;
struct stage_reg_w cur_w_reg;

struct stage_reg_d * current_stage_d_register = &cur_d_reg;
struct stage_reg_x * current_stage_x_register = &cur_x_reg;
struct stage_reg_m * current_stage_m_register = &cur_m_reg;
struct stage_reg_w * current_stage_w_register = &cur_w_reg;


#ifndef SIM_NO_PIPELINE
void
simulator_execute_instructions (uint64_t n_steps)
{
    uint32_t    inst;
    struct stage_reg_d  new_d_reg;
    struct stage_reg_x  new_x_reg;
    struct stage_reg_m  new_m_reg;
    struct stage_reg_w  new_w_reg;

    for (uint64_t i = 0; i < n_steps; ++i) {
        memory_dump (&inst, get_pc_internal(), sizeof (inst));
        if (inst == RISCV_INSTR_EBREAK) {
            break;
        }
        memory_reset_cycle ();
        register_reset_cycle ();
        stage_writeback ();
        stage_memory (&new_w_reg);
        stage_execute (&new_m_reg);
        stage_decode (&new_x_reg);
        stage_fetch (&new_d_reg);
        memcpy (&cur_d_reg, &new_d_reg, sizeof (cur_d_reg));
        memcpy (&cur_x_reg, &new_x_reg, sizeof (cur_x_reg));
        memcpy (&cur_m_reg, &new_m_reg, sizeof (cur_m_reg));
        memcpy (&cur_w_reg, &new_w_reg, sizeof (cur_w_reg));
    }
}
#else

static
void
simulator_execute_instructions (uint64_t n_steps)
{
    uint64_t    new_pc;
    uint32_t    inst;

    for (uint64_t i = 0; i < n_steps; ++i) {
        memory_dump (&inst, get_pc_internal (), sizeof (inst));
        if (inst == RISCV_INSTR_EBREAK) {
            break;
        }
        memory_reset_cycle ();
        register_reset_cycle ();
        execute_single_instruction (get_pc_internal (), &new_pc);
        set_pc_internal (new_pc);
    }
}

#endif

static
bool
verify_base (const char * s, int base)
{
    /* Skip past 0x if it's present */
    if (base == 16 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
    }

    while (*s != '\0') {
        if (base == 8 && ! (*s >= '0' && *s <= '7')) {
            return false;
        } else if (base == 16 && ! isxdigit (*s)) {
            return false;
        }
        s++;
    }
    return true;
}

static
bool
load_data_from_file (const char *filename, bool is_hex, uint64_t addr)
{
    static char     buf[4096];
    static char     bufcpy[4100];
    static uint8_t  membuf[2048];
    char *          ctx;
    char *          tok;
    static char *   sep = " \t\n";
    uint64_t        offset;
    int             b;
    uint64_t        n;
    FILE *          fp;
    int             lineno = 0;

    if (filename == NULL) {
        fp = stdin;
    } else {
        if ((fp = fopen (filename, "r")) == NULL) {
            fprintf (stderr, "load: failed to open %s!\n", filename);
            return false;
        }
    }
    fprintf (stderr, "Loading %s at 0x%016llx using %s\n", filename, (ull)addr, is_hex ? "hex" : "binary");
    if (is_hex) {
        while (fgets (buf, sizeof (buf) - 1, fp) != NULL) {
            lineno += 1;
            strncpy (bufcpy, buf, sizeof (buf));
            tok = strtok_r (buf, sep, &ctx);
            if (tok == NULL) {
                /* No address on line, so skip the line */
                continue;
            }
            if (! verify_base (tok, 8)) {
                fprintf (stderr, "load: bad offset in %s(%d): %s", filename, lineno, bufcpy);
                fclose (fp);
                return false;
            }
            for (int i = 0; tok[i] != '\0'; ++i) {
                if (tok[i] < '0' || tok[i] > '7') {

                }
            }
            offset = strtol (tok, NULL, 8);
            for (n = 0, tok = strtok_r (NULL, sep, &ctx); tok != NULL; tok = strtok_r (NULL, sep, &ctx), n++) {
                b = strtol (tok, NULL, 16);
                if (b < 0 || b > 255 || !verify_base (tok, 16)) {
                    fprintf (stderr, "load: bad byte in %s(%d): %s", filename, lineno, bufcpy);
                    fclose (fp);
                    return false;
                }
                membuf[n] = b;
            }
            if (n > 0) {
                memory_load (membuf, addr + offset, n);
            }
        }
    } else if (fp == stdin) {
        fprintf (stderr, "load: binary load from stdin not supported!\n");
        return false;
    } else {
        fprintf (stderr, "load: binary load from file not yet implemented!\n");
        fclose (fp);
        return (false);
    }
    if (fp != stdin) {
        fclose (fp);
    }
    return true;
}

static
bool
dump_data_to_file (const char *filename, bool is_hex, uint64_t addr, uint64_t length)
{
    static uint8_t  buf[4096];
    uint64_t        offset;
    FILE *          fp;
    const uint64_t  bpl = 16;
    uint64_t        i, actual_bytes;

    if (filename == NULL) {
        fp = stdout;
    } else {
        if ((fp = fopen (filename, "w")) == NULL) {
            fprintf (stderr, "dump: failed to open %s!\n", filename);
            return false;
        }
    }
    fprintf (stderr, "Dumping %s to %s at 0x%016llx for %llu bytes\n", is_hex ? "hex" : "binary",
             filename == NULL ? "stdout" : filename, (ull)addr,
             (ull)length);
    if (is_hex) {
        for (offset = 0; offset < length; offset += bpl) {
            actual_bytes = offset - length < bpl ? offset - length : bpl;
            memory_dump (buf, addr + offset, actual_bytes);
            fprintf (fp, "%012llo", (unsigned long long)(addr + offset));
            for (i = 0; i < actual_bytes; ++i) {
                fprintf (fp, " %02x", buf[i]);
            }
            fputc ('\n', fp);
        }
        fflush (stdout);
    } else if (fp == stdout) {
        fprintf (stderr, "dump: binary output to stdout not supported!\n");
        return false;
    } else {
        fprintf (stderr, "dump: output to binary not yet implemented!\n");
        fclose (fp);
        return false;
    }

    if (fp != stdout) {
        fclose (fp);
    }
    return true;
}

static
bool
check_for_hex (const char *sep, char **ctx, char **token)
{
    *token = strtok_r (NULL, sep, ctx);
    if (*token != NULL && !strcasecmp (*token, "/x")) {
        *token = strtok_r (NULL, sep, ctx);
        return true;
    }
    return false;
}

/*
 * Need to rewrite this using flex and bison.  That'll happen soon....
 */
static
bool
execute_line (const char * l)
{
    char    linebuf[SIM_MAX_LINE];
    char *  cmd;
    char *  token;
    char *  ctx;
    bool    is_hex;
    uint64_t    prog_start;
    uint64_t    n_steps;
    uint64_t    address, length, reg_num, value;

    strncpy (linebuf, l, SIM_MAX_LINE-1);
    linebuf[SIM_MAX_LINE-1] = '\0';         /* NULL-terminate just in case */
    cmd = strtok_r (linebuf, cmdsep, &ctx);
    /* The for loop allows us to use break like in a case statement */
    for (int i = 0; i < 1; ++i) {
        /* No command given */
        if (cmd == NULL) {
             break;
        }        
        if (!strcasecmp ("load", cmd)) {

            is_hex = check_for_hex (cmdsep, &ctx, &token);
            if (token == NULL || ! verify_base (token, 16)) {
                fprintf (stderr, "Usage: load [/x] <address> [filename]\n");
                break;
            }
            address = strtol (token, NULL, 0);
            if (address > memory_size) {
                fprintf (stderr, "Address out of range: 0x%16llx\n", (ull)address);
                break;
            }
            token = strtok_r (NULL, cmdsep, &ctx);
            if (! load_data_from_file (token, is_hex, address)) {
                fprintf (stderr, "load: failed to load all data from %s\n", token == NULL ? "<stdin>" : token);
                break;
            }
        } else if (!strcasecmp ("dump", cmd)) {
            is_hex = check_for_hex (cmdsep, &ctx, &token);
            if (token == NULL) {
                fprintf (stderr, "Usage: dump [/x] <address> <length> [filename]\n");
                break;
            }
            address = strtol (token, NULL, 0);
            token = strtok_r (NULL, cmdsep, &ctx);
            if (token == NULL) {
                fprintf (stderr, "Usage: dump [/x] <address> <length> [filename]\n");
                break;
            }
            length = strtol (token, NULL, 0);
            if (address + length > memory_size || length > memory_size) {
                fprintf (stderr, "dump: address or length out of range: address 0x%16llx length %lld\n", (ull)address, (ull)length);
                break;
            }
            token = strtok_r (NULL, cmdsep, &ctx);
            if (! dump_data_to_file (token, is_hex, address, length)) {
                fprintf (stderr, "dump: failed to dump data from %s\n", token == NULL ? "<stdout>" : token);
                break;
            }

        } else if (!strcasecmp ("readreg", cmd)) {
            is_hex = check_for_hex (cmdsep, &ctx, &token);
            if (token == NULL) {
                fprintf (stderr, "Usage: readreg [/x] <reg_num>\n");
            }
            reg_num = strtol (token, NULL, 0);
            if (reg_num >= RISCV_NUM_REGISTERS) {
                fprintf (stderr, "readreg: bad register %llu\n", (ull)reg_num);
                break;
            }
            if (is_hex) {
                printf ("R%llu = 0x%016llx\n", (ull)reg_num, (ull)register_read_one (reg_num));
            } else {
                printf ("R%llu = %llu\n", (ull)reg_num, (ull)register_read_one (reg_num));
            }
        } else if (!strcasecmp ("writereg", cmd)) {
            token = strtok_r (NULL, cmdsep, &ctx);
            if (token == NULL) {
                fprintf (stderr, "Usage: writereg <reg_num> <value>\n");
                break;
            }
            reg_num = strtoull (token, NULL, 0);
            if (reg_num >= RISCV_NUM_REGISTERS) {
                fprintf (stderr, "writereg: bad register %llu\n", (ull)reg_num);
                break;
            }
            token = strtok_r (NULL, cmdsep, &ctx);
            if (token == NULL) {
                fprintf (stderr, "writereg: no value\n");
                break;
            }
            value = strtol (token, NULL, 0);
            register_file[reg_num] = value;
        } else if (!strcasecmp ("run", cmd)) {
            token = strtok_r (NULL, cmdsep, &ctx);
            if (token == NULL) {
                fprintf (stderr, "Usage: run <number of steps>\n");
                break;
            }
            n_steps = strtol (token, NULL, 0);
            if (n_steps < 1 || n_steps > 100000000) {
                fprintf (stderr, "run: steps must be between 1-100000000, not %llu\n", (ull)n_steps);
                break;
            }
            simulator_execute_instructions (n_steps);
        } else if (!strcasecmp ("setpc", cmd)) {
            token = strtok_r (NULL, cmdsep, &ctx);
            if (token == NULL) {
                fprintf (stderr, "Usage: setpc <program counter>\n");
                break;
            }
            prog_start = strtoul (token, NULL, 0);
            if (prog_start > memory_size) {
                fprintf (stderr, "setpc: program counter (%llx) must be within memory (%llx)\n",
                         (ull)prog_start, (ull)memory_size);
            }
            if (prog_start % 4 != 0) {
                fprintf (stderr, "setpc: program counter (%llx) must be a multiple of 4\n",
                    (ull)prog_start);
                break;
            }
            set_pc (prog_start);
        } else if (!strcasecmp ("getpc", cmd)) {
            printf ("PC: 0x%llx\n", (ull)get_pc ());
        } else if (!strcasecmp ("exit", cmd)) {
            fflush (stdout);
            return false;
        } else {
            fprintf (stderr, "Unrecognized command: %s\n", cmd);
            break;
        }
    }
    fflush (stdout);
    return true;
}

#ifndef HAS_READLINE
static
char *
readline (const char * prompt)
{
    static char linebuf[8192];

    printf ("%s", prompt);
    fflush (stdout);
    return (fgets (linebuf, sizeof (linebuf) - 1, stdin));
}

#endif

extern void unit_tests ();

static
void
usage_and_exit () {
    fprintf (stderr, "Usage: %s [-f command_file] [-u]\n", prog_name);
    fprintf (stderr, "\t-f command_file : run simulator commands from command_file\n");
    fprintf (stderr, "\t-u : run unit tests\n");
    exit (1);
}


int
main (int argc, char * const * argv)
{
    char    prompt[100];
    char    linebuf[1024];
    char *  cur_line;
    bool    interactive = true;
    int     ch;
    FILE *  cmd_fp;

    prog_name = argv[0];
    while ((ch = getopt (argc, argv, "uf:")) != -1) {
        switch (ch) {
        case 'f':
            if ((cmd_fp = fopen (optarg, "r")) != NULL) {
                interactive = false;
            } else {
                fprintf (stderr, "Couldn't open command file %s\n", optarg);
                usage_and_exit ();
            }
            break;
        case 'u':
            memory_initialize(memory_size);
	    unit_tests ();
            exit (0);
            break;
        case 'h':
        case '?':
        default:
            usage_and_exit ();
            break;
        }
    }

    memory_initialize (memory_size);
    memset (&cur_d_reg, 0, sizeof (cur_d_reg));
    memset (&cur_x_reg, 0, sizeof (cur_x_reg));
    memset (&cur_m_reg, 0, sizeof (cur_m_reg));
    memset (&cur_w_reg, 0, sizeof (cur_w_reg));

    while (1) {
        if (interactive) {
            sprintf (prompt, "RISCV (PC=0x%llx)> ", (ull)get_pc ());
            if ((cur_line = readline (prompt)) == NULL) {
                putchar ('\n');
                fflush (stdout);
                exit (0);
            }
        } else {
            cur_line = fgets (linebuf, sizeof (linebuf) - 1, cmd_fp);
            if (cur_line == NULL) {
                /* End of input */
                break;
            }
            linebuf[sizeof(linebuf) - 1] = '\0';
        }
        if (!execute_line (cur_line)) {
            break;
        }
    }
}

