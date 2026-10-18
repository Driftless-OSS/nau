/*
 * NAU Kernel
 * Path: kernel/tests/balloc_test.c
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
#include <fdt.h>
#include <pm.h>

void balloc_test_run(void)
{
	dprintf("** balloc tests running\n");

	int size = 4096;
	void *ptr = balloc(size);
	
	dprintf("test: allocating %d bytes at %p\n", size, ptr);

	if (!ptr) {
		dprintf("test: failed to allocate\n");
		/* panic? */
		return;
	}

	dprintf("test: ran successfully **\n");
}
