/*
 * NAU Kernel
 * Path: arch/arm64/exception.c
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
 * Implements aarch64 exceptions and stuff
 */

static const char *const grp[] = {
	"Current EL SP0", "Current EL SPx", "Lower EL A64", "Lower EL A32"
};
static const char *const typ[] = { "Sync", "IRQ", "FIQ", "SError" };

static const char *ec_name(unsigned int ec) {
	switch (ec) {
	case 0x00: return "Unknown / undefined instruction";
	case 0x01: return "WFI/WFE trapped";
	case 0x07: return "FP/SIMD access trapped";
	case 0x15: return "SVC (AArch64)";
	case 0x20: return "Instruction abort, lower EL";
	case 0x21: return "Instruction abort, same EL";
	case 0x22: return "PC alignment fault";
	case 0x24: return "Data abort, lower EL";
	case 0x25: return "Data abort, same EL";
	case 0x26: return "SP alignment fault";
	case 0x2f: return "SError";
	case 0x3c: return "BRK";
	default:   return "Other";
	}
}

__attribute__((noreturn))
void arm64_exception(unsigned int vec) {
	uint64_t esr, elr, far, spsr;

	__asm__ volatile("mrs %0, esr_el1"  : "=r"(esr));
	__asm__ volatile("mrs %0, elr_el1"  : "=r"(elr));
	__asm__ volatile("mrs %0, far_el1"  : "=r"(far));
	__asm__ volatile("mrs %0, spsr_el1" : "=r"(spsr));

	unsigned int ec = (esr >> 26) & 0x3f;

	dprintf("\n*** EXCEPTION: %s %s\n", grp[vec >> 2], typ[vec & 3]);
	dprintf("ESR  = %llx  EC = %x (%s)\n", (unsigned long long)esr, ec, ec_name(ec));
	dprintf("ELR  = %llx\n", (unsigned long long)elr);
	dprintf("FAR  = %llx  (valid for aborts/alignment only)\n", (unsigned long long)far);
	dprintf("SPSR = %llx\n", (unsigned long long)spsr);

	for (;;) {
		__asm__ volatile("wfe");
	}
}
