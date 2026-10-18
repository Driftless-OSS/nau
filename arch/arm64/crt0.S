.global _start
_start:
	// preserve DTB pointer for later..
	mov x19, x0

	ldr x30, =stack_top
	mov sp, x30

	ldr x0, =arm64_vectors
	msr vbar_el1, x0
	isb

	ldr x0, =__bss_start
	ldr x1, =__bss_end
1:	cmp x0, x1
	b.hs 2f
	str xzr, [x0], #8
	b 1b
2:
	// dtb arg
	mov x0, x19
	bl nau_start
	b .
