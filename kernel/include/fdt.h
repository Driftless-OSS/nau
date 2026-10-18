/*
 * NAU Kernel
 * Path: kernel/include/fdt.h
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

#pragma once

#include <stdint.h>

struct mem_region {
	uint64_t base;
	uint64_t size;
};

int fdt_memory(const void *dtb, struct mem_region *out, int max);
