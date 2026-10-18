/*
 * NAU Kernel
 * Path: kernel/fdt.c
 *
 * Copyright (c) 2026 Driftless Software. All rights reserved.
 * Property of Driftless Software.
 *
 * NOTICE: This software is governed by a license agreement. 
 * Redistribution, modification, or use of this file, in whole or in part,
 * is strictly restricted to the terms specified in the 'LICENSE' file 
 * located at the root directory of this project repository.
 *
 * Author: Lily Walker (lwalker)
 */

/*
 * Flattened device tree parser which is extremely simple as of now.. quite cheezy
 */

#include <stdint.h>
#include <stddef.h>
#include <fdt.h>

#define FDT_BEGIN_NODE 1
#define FDT_END_NODE   2
#define FDT_PROP       3
#define FDT_NOP        4

static uint32_t be32(const uint32_t *p) {
	return __builtin_bswap32(*p);
}

static size_t slen(const char *s) {
	size_t n = 0;
	while (s[n]) n++;
	return n;
}

static int seq(const char *a, const char *b) {
	while (*a && *a == *b) { a++; b++; }
	return *a == *b;
}

static int sprefix(const char *s, const char *prefix) {
	while (*prefix) {
		if (*s++ != *prefix++) return 0;
	}
	return 1;
}

static uint64_t cells(const uint32_t *p, uint32_t n) {
	uint64_t v = 0;
	while (n--) {
		v = (v << 32) | be32(p++);
	}
	return v;
}

int fdt_memory(const void *dtb, struct mem_region *out, int max) {
	const uint8_t *base = dtb;
	const uint32_t *hdr = dtb;

	if (be32(&hdr[0]) != 0xd00dfeed) {
		return 0;
	}

	const uint32_t *p = (const uint32_t *)(base + be32(&hdr[2]));
	const char *strs  = (const char *)(base + be32(&hdr[3]));

	uint32_t ac = 2, sc = 1;
	int depth = 0, in_mem = 0, count = 0;

	for (;;) {
		uint32_t tok = be32(p++);

		if (tok == FDT_BEGIN_NODE) {
			const char *name = (const char *)p;
			p += (slen(name) + 4) / 4;
			depth++;
			in_mem = (depth == 2) && sprefix(name, "memory");
		} else if (tok == FDT_END_NODE) {
			depth--;
			in_mem = 0;
		} else if (tok == FDT_PROP) {
			uint32_t len = be32(&p[0]);
			const char *pname = strs + be32(&p[1]);
			const uint32_t *data = &p[2];
			p += 2 + (len + 3) / 4;

			if (depth == 1 && seq(pname, "#address-cells")) {
				ac = be32(data);
			} else if (depth == 1 && seq(pname, "#size-cells")) {
				sc = be32(data);
			} else if (in_mem && seq(pname, "reg")) {
				uint32_t stride = (ac + sc) * 4;
				for (uint32_t off = 0; off + stride <= len && count < max; off += stride) {
					const uint32_t *e = (const uint32_t *)((const uint8_t *)data + off);
					out[count].base = cells(e, ac);
					out[count].size = cells(e + ac, sc);
					count++;
				}
			}
		} else if (tok != FDT_NOP) {
			break;
		}
	}

	return count;
}
