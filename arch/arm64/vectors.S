.section .text
.global arm64_vectors

.macro ventry idx
	.balign 128
	mov x0, #\idx
	b exc_common
.endm

// must be 2kb aligned
.balign 2048
arm64_vectors:
	ventry 0
	ventry 1
	ventry 2
	ventry 3
	ventry 4
	ventry 5
	ventry 6
	ventry 7
	ventry 8
	ventry 9
	ventry 10
	ventry 11
	ventry 12
	ventry 13
	ventry 14
	ventry 15

exc_common:
	// x0 holds index
	bl arm64_exception
	b .
