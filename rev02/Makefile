CC = gcc
DEFINES =  DEBUG
WARNINGS = -Wall $(addprefix -D, $(DEFINES))
HEADERS = riscv_pipeline_registers.h riscv_sim_pipeline_framework.h global_variables.h
FILES = $(HEADERS) riscv_sim_pipeline_framework.c stage_fetch.c stage_fetch.h stage_decode.c stage_decode.h stage_execute.c stage_execute.h stage_memory.c stage_memory.h stage_writeback.c stage_writeback.h Makefile README unit_tests.c
TESTS = add.hex

sim: riscv_sim_pipeline_framework.o
	$(CC) -o sim $< stage_fetch.o stage_decode.o stage_execute.o stage_memory.o stage_writeback.o unit_tests.o
riscv_sim_pipeline_framework.o: riscv_sim_pipeline_framework.c $(HEADERS) stage_fetch.o stage_decode.o stage_execute.o stage_memory.o stage_writeback.o unit_tests.o
	$(CC) -c $(WARNINGS) $<

stage_fetch.o: stage_fetch.c stage_fetch.h $(HEADERS)
	$(CC) -c $(WARNINGS) $<
stage_decode.o: stage_decode.c stage_decode.h $(HEADERS)
	$(CC) -c $(WARNINGS) $<
stage_execute.o: stage_execute.c stage_execute.h $(HEADERS)
	$(CC) -c $(WARNINGS) $<
stage_memory.o: stage_memory.c stage_memory.h $(HEADERS)
	$(CC) -c $(WARNINGS) $<
stage_writeback.o: stage_writeback.c stage_writeback.h $(HEADERS)
	$(CC) -c $(WARNINGS) $<
unit_tests.o: unit_tests.c $(HEADERS)
	$(CC) -c $(WARNINGS) $<
git_update:
	git add $(FILES) $(TESTS)
clean:
	rm -f sim *.o
