/*
 * NAU Kernel
 * Path: kernel/debug.c
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
#include <stdarg.h>
#include <stddef.h>
#include <arch.h>
#include <debug.h>

/*
 * Implements debugging functions like dputs.
 */

#define FLAG_ZERO 0x1
#define FLAG_LEFT 0x2

void dputc(char c) {
	arch_serial_putc(c);

	if (c == '\n') {
		arch_serial_putc('\r');
	}	
}

void dputs(const char *s) {
	while (*s) {
		dputc(*s);
		s++;
	}
}

static unsigned int udivmod(uint64_t *n, unsigned int base) {
	uint64_t q = 0, r = 0, v = *n;

	for (int i = 63; i >= 0; i--) {
		r = (r << 1) | ((v >> i) & 1);
		if (r >= base) {
			r -= base;
			q |= (uint64_t)1 << i;
		}
	}

	*n = q;
	return (unsigned int)r;
}

static void repeat(char c, int n) {
	while (n-- > 0) {
		dputc(c);
	}
}

static void print_num(uint64_t v, unsigned int base, int upper, int neg,
		      int width, int flags) {
	const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
	char buf[24];
	int n = 0;

	if (v == 0) {
		buf[n++] = '0';
	}
	while (v) {
		buf[n++] = digits[udivmod(&v, base)];
	}

	int pad = (width > n + neg) ? width - (n + neg) : 0;

	if (!(flags & (FLAG_LEFT | FLAG_ZERO))) {
		repeat(' ', pad);
	}
	if (neg) {
		dputc('-');
	}
	if ((flags & FLAG_ZERO) && !(flags & FLAG_LEFT)) {
		repeat('0', pad);
	}
	while (n > 0) {
		dputc(buf[--n]);
	}
	if (flags & FLAG_LEFT) {
		repeat(' ', pad);
	}
}

static void print_str(const char *s, int width, int flags) {
	int len = 0;

	if (!s) {
		s = "(null)";
	}
	while (s[len]) {
		len++;
	}

	int pad = (width > len) ? width - len : 0;

	if (!(flags & FLAG_LEFT)) {
		repeat(' ', pad);
	}
	dputs(s);
	if (flags & FLAG_LEFT) {
		repeat(' ', pad);
	}
}

void dprintf(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	for (; *fmt; fmt++) {
		if (*fmt != '%') {
			dputc(*fmt);
			continue;
		}
		fmt++;

		/* flags */
		int flags = 0;
		for (;; fmt++) {
			if (*fmt == '0')      flags |= FLAG_ZERO;
			else if (*fmt == '-') flags |= FLAG_LEFT;
			else break;
		}

		/* width */
		int width = 0;
		while (*fmt >= '0' && *fmt <= '9') {
			width = width * 10 + (*fmt - '0');
			fmt++;
		}

		/* length: 0 = int, 1 = long, 2 = long long, 3 = size_t */
		int len = 0;
		if (*fmt == 'l') {
			len = 1;
			fmt++;
			if (*fmt == 'l') {
				len = 2;
				fmt++;
			}
		} else if (*fmt == 'z') {
			len = 3;
			fmt++;
		}

		char spec = *fmt;
		if (!spec) {
			break;
		}

		switch (spec) {
		case 'c':
			dputc((char)va_arg(ap, int));
			break;

		case 's':
			print_str(va_arg(ap, const char *), width, flags);
			break;

		case 'd':
		case 'i': {
			int64_t v;
			if (len == 2)      v = va_arg(ap, long long);
			else if (len == 1) v = va_arg(ap, long);
			else if (len == 3) v = (int64_t)va_arg(ap, size_t);
			else               v = va_arg(ap, int);

			int neg = v < 0;
			print_num(neg ? -(uint64_t)v : (uint64_t)v, 10, 0, neg, width, flags);
			break;
		}

		case 'u':
		case 'x':
		case 'X': {
			uint64_t v;
			if (len == 2)      v = va_arg(ap, unsigned long long);
			else if (len == 1) v = va_arg(ap, unsigned long);
			else if (len == 3) v = va_arg(ap, size_t);
			else               v = va_arg(ap, unsigned int);

			print_num(v, spec == 'u' ? 10 : 16, spec == 'X', 0, width, flags);
			break;
		}

		case 'p':
			dputs("0x");
			print_num((uint64_t)(uintptr_t)va_arg(ap, void *), 16, 0, 0,
				  (int)(sizeof(void *) * 2), FLAG_ZERO);
			break;

		case '%':
			dputc('%');
			break;

		default:
			dputc('%');
			dputc(spec);
			break;
		}
	}

	va_end(ap);
}
