/*
 * NAU Kernel
 * Path: arch/arm32/exception.c
 *
 * Copyright (c) 2026 Driftless Software. All rights reserved.
 * Property of Driftless Software.
 *
 * NOTICE: This software is governed by a license agreement. 
 * Redistribution, modification, or use of this file, in whole or in part,
 * is strictly restricted to the terms specified in the 'LICENSE' file 
 * located at the root directory of this project repository.
 *
 * Author: Mayank Pathak (mpathak)
 */

#include <stdint.h>
#include <arch.h>
#include <debug.h>

/*
 * Implements arm32 exceptions
 */

static const char *const names[] = {
	"Reset", "Undefined instruction", "SVC", "Prefetch abort",
	"Data abort", "Reserved", "IRQ", "FIQ"
};

__attribute__((noreturn))
void arm32_exception(unsigned int vec, unsigned int lr, unsigned int spsr) {
	dprintf("\n*** EXCEPTION: %s\n", names[vec]);

	/* LR points past fault instr so offset depends on type */
	if (vec == 1) {
		dprintf("PC   = %x (undefined instruction)\n", lr - 4);
	} else if (vec == 3) {
		unsigned int ifsr, ifar;
		__asm__ volatile("mrc p15, 0, %0, c5, c0, 1" : "=r"(ifsr));
		__asm__ volatile("mrc p15, 0, %0, c6, c0, 2" : "=r"(ifar));
		dprintf("PC   = %x\nIFSR = %x  IFAR = %x\n", lr - 4, ifsr, ifar);
	} else if (vec == 4) {
		unsigned int dfsr, dfar;
		__asm__ volatile("mrc p15, 0, %0, c5, c0, 0" : "=r"(dfsr));
		__asm__ volatile("mrc p15, 0, %0, c6, c0, 0" : "=r"(dfar));
		dprintf("PC   = %x\nDFSR = %x  DFAR = %x\n", lr - 8, dfsr, dfar);
	} else {
		dprintf("LR   = %x\n", lr);
	}
	dprintf("SPSR = %x\n", spsr);

	for (;;) {
		__asm__ volatile("wfe");
	}
}
