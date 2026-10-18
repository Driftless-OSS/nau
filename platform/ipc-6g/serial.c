/*
 * NAU Kernel
 * Path: platform/ipc-6g/serial.c
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

/*
 * Implements serial, but the qemu ipod uses semihosting
 */

void arch_serial_init(void) {
}

void arch_serial_putc(char c) {
	// SYS_WRITEC
    register uint32_t r0 __asm__("r0") = 3;
    register const char *r1 __asm__("r1") = &c;
    __asm__ volatile("svc 0x123456" : "+r"(r0) : "r"(r1) : "memory");
}
