CC = gcc
HEADERS = riscv_pipeline_registers_vars.h riscv_pipeline_registers.h riscv_sim_framework.h global_variables.h memory_translation.h

sim: riscv_sim_framework.o
	$(CC) -o sim $< stage_fetch.o stage_decode.o stage_execute.o stage_memory.o stage_writeback.o unit_tests.o memory_translation.o tlb_interface.o
riscv_sim_framework.o: riscv_sim_framework.c $(HEADERS) stage_fetch.o stage_decode.o stage_execute.o stage_memory.o stage_writeback.o unit_tests.o memory_translation.o tlb_interface.o
	$(CC) -c $(WARNINGS) $<
stage_fetch.o: stage_fetch.c $(HEADERS)
	$(CC) -c $(WARNINGS) $<
stage_decode.o: stage_decode.c $(HEADERS)
	$(CC) -c $(WARNINGS) $<
stage_execute.o: stage_execute.c $(HEADERS)
	$(CC) -c $(WARNINGS) $<
stage_memory.o: stage_memory.c $(HEADERS)
	$(CC) -c $(WARNINGS) $<
stage_writeback.o: stage_writeback.c $(HEADERS)
	$(CC) -c $(WARNINGS) $<
unit_tests.o: unit_tests.c $(HEADERS)
	$(CC) -c $(WARNINGS) $<
memory_translation.o: memory_translation.c $(HEADERS)
	$(CC) -c $(WARNINGS) $<
tlb_interface.o: tlb_interface.c $(HEADERS)
	$(CC) -c $(WARNINGS) $<
clean:
	rm -f sim *.o
