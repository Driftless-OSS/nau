/*
 * NAU Kernel
 * Path: platform/qvirt/serial.c
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
 * Implements the serial UART functions for QEMU VIRT
 */

#define UART0_BASE 0x09000000

void arch_serial_putc(char c) {
	*(volatile uint32_t *)(UART0_BASE) = c;	
}

uint8_t arch_check_display(void)
{
	/* we DO have a display which is ramfb.. but im lazy to do that right now */
	return 0;
}

void arch_init_display(void)
{

}
