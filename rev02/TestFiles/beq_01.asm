.section .text
.globl  _start
_start:
        addi    x1, x0, 1
        addi    x2, x0, 1
        addi    x7, x0, 0
        beq     x1, x2, LOC
        addi    x7, x7, 1
LOC:
        addi    x7, x7, 2
        
