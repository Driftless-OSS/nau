.global _start
_start:
	@ DTB
	mov r4, r2
	ldr sp, =STACK_TOP
	@ single core no park yet
	ldr r0, =__bss_start
	ldr r1, =__bss_end
	mov r2, #0
1:	cmp r0, r1
	bhs 2f
	str r2, [r0], #4
	b 1b
2: bl arm_exc_init
   mov r0, r4	
   bl nau_start
3:	b 3b
