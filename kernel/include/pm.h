/*
 * NAU Kernel
 * Path: kernel/include/pm.h
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
#include <fdt.h>

/* approximately one page or 4kb */
#define KERN_BALLOC_OFFSET 0x1000

void balloc_init(uintptr_t start, uintptr_t end);
void *balloc(uint64_t size);
