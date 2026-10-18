	.arm

@ vector table
	.section .text.vectors, "ax"
	.balign 32
	.global __vectors_start
	.global __vectors_end
	.global arm32_vectors
__vectors_start:
arm32_vectors:
	ldr	pc, v_reset		@ 0x00
	ldr	pc, v_undef		@ 0x04
	ldr	pc, v_svc		@ 0x08
	ldr	pc, v_pabt		@ 0x0c
	ldr	pc, v_dabt		@ 0x10
	nop				@ 0x14 reserved
	ldr	pc, v_irq		@ 0x18
	ldr	pc, v_fiq		@ 0x1c
v_reset: .word _start
v_undef: .word undef_entry
v_svc:   .word svc_entry
v_pabt:  .word pabt_entry
v_dabt:  .word dabt_entry
v_irq:   .word irq_entry
v_fiq:   .word fiq_entry
__vectors_end:

	.text
	.balign 4

@ Trap frame
.macro SAVE_FRAME adj
	sub	lr, lr, #\adj
	stmfd	sp!, {r0-r12, lr}
	mrs	r0, spsr
	stmfd	sp!, {r0, r1}
.endm

.macro RESTORE_FRAME
	ldmfd	sp!, {r0, r1}
	msr	spsr_cxsf, r0
	ldmfd	sp!, {r0-r12, pc}^
.endm

@ fatal path
.macro FATAL name, type, adj
\name:
	SAVE_FRAME \adj
	mov	r4, sp			@ frame pointer
	mrs	r5, cpsr
	ldr	r1, [r4]		@ saved spsr
	and	r1, r1, #0x1f
	cmp	r1, #0x10		@ user mode shares banks with sys
	moveq	r1, #0x1f
	orr	r1, r1, #0xc0		@ keep IRQ/FIQ masked
	msr	cpsr_c, r1
	mov	r2, sp
	mov	r3, lr
	msr	cpsr_c, r5
	mov	r0, r4
	mov	r1, #\type
	bl	arm_exception
	b	.
.endm

	FATAL undef_entry, 1, 4	@ lr = insn+4 (ARM state)
	FATAL pabt_entry,  3, 4	@ lr = insn+4
	FATAL dabt_entry,  4, 8	@ lr = insn+8

irq_entry:
	SAVE_FRAME 4
	mov	r0, sp
	bl	arm_irq_handler
	RESTORE_FRAME

fiq_entry:
	SAVE_FRAME 4
	mov	r0, sp
	bl	arm_fiq_handler
	RESTORE_FRAME

svc_entry:
	SAVE_FRAME 0
	ldr	r1, [lr, #-4]		@ the svc instruction
	bic	r1, r1, #0xff000000	@ r1 = svc number
	mov	r0, sp
	bl	arm_svc_handler
	str	r0, [sp, #8]		@ return value -> saved r0
	RESTORE_FRAME

@ called only once
	.global arm_exc_init
arm_exc_init:
	mrc	p15, 0, r0, c1, c0, 0	@ V=0: vectors at 0x00000000
	bic	r0, r0, #(1 << 13)
	mcr	p15, 0, r0, c1, c0, 0

	ldr	r0, =__vectors_start	@ copy table to 0x0
	ldr	r1, =__vectors_end
	mov	r2, #0
1:	cmp	r0, r1
	ldrlo	r3, [r0], #4
	strlo	r3, [r2], #4
	blo	1b

	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4	@ drain write buffer
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate I cache

	mrs	r0, cpsr		@ remember current mode (SVC)
	msr	cpsr_c, #0xd1		@ FIQ, I+F masked
	ldr	sp, =fiq_stack_top
	msr	cpsr_c, #0xd2		@ IRQ
	ldr	sp, =irq_stack_top
	msr	cpsr_c, #0xd7		@ ABT
	ldr	sp, =abt_stack_top
	msr	cpsr_c, #0xdb		@ UND
	ldr	sp, =und_stack_top
	msr	cpsr_c, r0		@ back
	bx	lr
	.ltorg

	.section .bss
	.balign 8
abt_stack:	.space 2048
abt_stack_top:
und_stack:	.space 2048
und_stack_top:
irq_stack:	.space 2048
irq_stack_top:
fiq_stack:	.space 2048
fiq_stack_top:
