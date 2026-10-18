.section .text
.global arm32_vectors

@ 32b alignment
.balign 32
arm32_vectors:
	b stub_reset              @ 0x00
	b stub_undef              @ 0x04
	b stub_svc                @ 0x08
	b stub_pabt               @ 0x0c
	b stub_dabt               @ 0x10
	b stub_reserved           @ 0x14
	b stub_irq                @ 0x18
	b stub_fiq                @ 0x1c

.macro stub name, idx
\name:
	ldr sp, =exc_stack_top    @ banked sp of this mode is garbage so use our own
	mov r0, #\idx
	mov r1, lr
	mrs r2, spsr
	bl arm32_exception
	b .
.endm

	stub stub_reset,    0
	stub stub_undef,    1
	stub stub_svc,      2
	stub stub_pabt,     3
	stub stub_dabt,     4
	stub stub_reserved, 5
	stub stub_irq,      6
	stub stub_fiq,      7
	.ltorg

.section .bss
.balign 8
exc_stack:
	.space 4096
exc_stack_top:
