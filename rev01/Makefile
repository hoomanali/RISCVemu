CC = gcc
WARNINGS = -Wall
# To turn off warnings, put the line down below into the WARNINGS variable above:
#-Wno-unused-variable
FILES = Makefile README riscv_sim_execute_instruction.c riscv_sim_execute_instruction.h riscv_sim_framework.c riscv_sim_framework.h

sim : riscv_sim_framework.o riscv_sim_execute_instruction.o
	$(CC) -o sim riscv_sim_framework.o riscv_sim_execute_instruction.o

riscv_sim_framework.o : riscv_sim_framework.c
	$(CC) -c $(WARNINGS) riscv_sim_framework.c

riscv_sim_execute_instruction.o : riscv_sim_execute_instruction.c
	$(CC) -c $(WARNINGS) riscv_sim_execute_instruction.c

clean :
	rm -f sim riscv_sim_framework.o riscv_sim_execute_instruction.o

git_update:
	git add $(FILES)
