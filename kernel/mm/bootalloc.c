/*
 * NAU Kernel
 * Path: kernel/mm/bootalloc.c
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
#include <stddef.h>
#include <arch.h>
#include <debug.h>
#include <pm.h>
#include <fdt.h>

/*
 * Extremely simple allocator to solve the recursive bitmap alloc problem.. just
 * bumping to allocate
 */

uintptr_t g_boot_ptr;
uintptr_t g_boot_end;

void balloc_init(uintptr_t start, uintptr_t end)
{
	dprintf("balloc: initializing bump allocator (0x%llx, 0x%llx)\n", (unsigned long long)start, (unsigned long long)end);

	g_boot_ptr = start;
	g_boot_end = end;
}

void *balloc(uint64_t size)
{
	uintptr_t save = ALIGN(g_boot_ptr, 8);

	g_boot_ptr = (save+ALIGN(size, 8));

	return (void*)save;
}
