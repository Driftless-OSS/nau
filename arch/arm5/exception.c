/*
 * NAU Kernel
 * Path: arch/arm5/exception.c
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

/*
 * Implements armv5 exceptions
 */

#include <stdint.h>
#include <arch.h>

void arch_serial_putc(char c);

struct trap_frame {
    uint32_t spsr;
    uint32_t pad;
    uint32_t r[13];
    uint32_t pc;
};

enum { EXC_RESET, EXC_UNDEF, EXC_SVC, EXC_PABT, EXC_DABT, EXC_RSVD, EXC_IRQ, EXC_FIQ };

static const char *const exc_names[] = {
    "RESET", "UNDEFINED INSTRUCTION", "SVC", "PREFETCH ABORT",
    "DATA ABORT", "RESERVED", "IRQ", "FIQ",
};

static void puts_(const char *s) { while (*s) arch_serial_putc(*s++); }

static void put_hex(uint32_t v)
{
    puts_("0x");
    for (int i = 28; i >= 0; i -= 4)
        arch_serial_putc("0123456789abcdef"[(v >> i) & 0xf]);
}

static void put_kv(const char *k, uint32_t v)
{
    puts_(k);
    put_hex(v);
    puts_("\n");
}

static const char *mode_name(uint32_t cpsr)
{
    switch (cpsr & 0x1f) {
    case 0x10: return "USR";
    case 0x11: return "FIQ";
    case 0x12: return "IRQ";
    case 0x13: return "SVC";
    case 0x17: return "ABT";
    case 0x1b: return "UND";
    case 0x1f: return "SYS";
    default:   return "???";
    }
}

__attribute__((noreturn))
void arm_exception(struct trap_frame *f, uint32_t type, uint32_t sp, uint32_t lr)
{
    puts_("\n*** EXCEPTION: ");
    puts_(type < 8 ? exc_names[type] : "?");
    puts_(" ***\n");

    put_kv("pc   = ", f->pc);
    put_kv("lr   = ", lr);
    put_kv("sp   = ", sp);
    put_kv("spsr = ", f->spsr);
    puts_("mode = ");
    puts_(mode_name(f->spsr));
    puts_((f->spsr & 0x20) ? " (Thumb)\n" : " (ARM)\n");

    for (int i = 0; i < 13; i++) {
        arch_serial_putc('r');
        if (i >= 10) arch_serial_putc('1');
        arch_serial_putc('0' + (i % 10));
        puts_(" = ");
        put_hex(f->r[i]);
        puts_("\n");
    }

    if (type == EXC_DABT) {
        uint32_t fsr, dfar;
        __asm__ volatile("mrc p15, 0, %0, c5, c0, 0" : "=r"(fsr));
        __asm__ volatile("mrc p15, 0, %0, c6, c0, 0" : "=r"(dfar));
        put_kv("fsr  = ", fsr);
        put_kv("far  = ", dfar);
    }

    for (;;);
}

/* kernel should override */
__attribute__((weak)) void arm_irq_handler(struct trap_frame *f) { (void)f; }
__attribute__((weak)) void arm_fiq_handler(struct trap_frame *f) { (void)f; }
__attribute__((weak)) uint32_t arm_svc_handler(struct trap_frame *f, uint32_t num)
{
    (void)f; (void)num;
    return (uint32_t)-1;
}
