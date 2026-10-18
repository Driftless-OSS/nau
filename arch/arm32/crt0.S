.global _start
.type _start, %function
_start:
	mrc p15, 0, r3, c0, c0, 5
	ands r3, r3, #3
	bne park
	
	mov r4, r2 @ DTB ptr

	ldr sp, =STACK_TOP

	ldr r0, =arm32_vectors
	mcr p15, 0, r0, c12, c0, 0
	isb

	ldr r0, =__bss_start
	ldr r1, =__bss_end
	mov r2, #0
1:	cmp r0, r1
	bhs 2f
	str r2, [r0], #4
	b 1b
2:
	@ DTB arg
	mov r0, r4
	bl nau_start
3:	b 3b

park: wfi
b park

.size _start, . - _start
