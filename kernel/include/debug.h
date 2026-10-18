/*
 * NAU Kernel
 * Path: kernel/include/debug.h
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

#define ALIGN(a, b) (((a) + ((b)-1)) & ~((b)-1))

void dputc(char c);
void dputs(const char *s);

void dprintf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
