/*
 * NAU Kernel
 * Path: kernel/init.c
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
#include <layout.h>
#include <fdt.h>
#include <pm.h>
#include <tests.h>

static uint8_t g_is_video;

// small helper
static int dtb_valid(const void *p) {
	return p && __builtin_bswap32(((const uint32_t *)p)[0]) == 0xd00dfeed;
}

static inline void nau_run_suite(void) {
#ifdef ENABLE_TEST_SUITE
	balloc_test_run();
#endif
}

void nau_start(void *dtb) {
	dputs("\nVelocitas/NAU Version 1.0.0\n");

	/* kernel size print */
	dprintf("Kernel: %p - %p (%zu KB)\n", __kernel_start, __kernel_end,
			(size_t)(__kernel_end - __kernel_start) / 1024);

#ifdef DTB_FALLBACK
	if (!dtb_valid(dtb)) {
		dtb = (void *)DTB_FALLBACK;
	}
#endif

	/* parse the FDT if there is one and find out about memory */
	if (dtb_valid(dtb)) {
		uint32_t size = __builtin_bswap32(((uint32_t *)dtb)[1]);
		dprintf("DTB: %p size=%u\n", dtb, (unsigned int)size);
	} else {
		dprintf("DTB: none\n");
	}

	struct mem_region mem[4];
	int n = dtb_valid(dtb) ? fdt_memory(dtb, mem, 4) : 0;

#ifdef FALLBACK_RAM_BASE
	if (n == 0) {
		mem[0].base = FALLBACK_RAM_BASE;
		mem[0].size = FALLBACK_RAM_SIZE;
		n = 1;
	}
#endif

	for (int i = 0; i < n; i++) {
		dprintf("RAM: 0x%llx - 0x%llx (%llu MB)\n",
			(unsigned long long)mem[i].base,
			(unsigned long long)(mem[i].base + mem[i].size),
			(unsigned long long)(mem[i].size >> 20));
	}

	/* initialize memory systems */
	balloc_init((uintptr_t)__kernel_end, (uintptr_t)mem[0].base+mem[0].size);

	/* check if we have a display */
	g_is_video = arch_check_display();
	if (!g_is_video) {
		/* not available */
		dprintf("init: video not found\n");
	} else {
		/* we have it so initialize it */
		dprintf("init: initializing video\n");
		arch_init_display();
	}
	
	nau_run_suite();
}
