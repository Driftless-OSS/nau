/*
 * NAU Kernel
 * Path: platform/sgexy/serial.c
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
 * Implements the serial UART functions
 */

#define UART0_BASE   0x13800000u
#define ULCON        (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UCON         (*(volatile uint32_t *)(UART0_BASE + 0x04))
#define UFCON        (*(volatile uint32_t *)(UART0_BASE + 0x08))
#define UTRSTAT      (*(volatile uint32_t *)(UART0_BASE + 0x10))
#define UFSTAT       (*(volatile uint32_t *)(UART0_BASE + 0x18))
#define UTXH         (*(volatile uint32_t *)(UART0_BASE + 0x20))

void arch_serial_init(void) {
    ULCON = 0x3;
    UCON = 0x5;
    UFCON = 0x0;
}

void arch_serial_putc(char c) {
    while (!(UTRSTAT & (1u << 1)));
    UTXH = (uint8_t)c;
}

uint8_t arch_check_display(void)
{
	/* this also does have a display - lcd, but will do it later */
	return 0;
}

void arch_init_display(void)
{

}
