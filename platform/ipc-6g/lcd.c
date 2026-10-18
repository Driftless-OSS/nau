/*
 * NAU Kernel
 * Path: platform/ipc-6g/lcd.c
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
#include <debug.h>

/*
 * Simple SL8702 LCD driver implementation
 */

#define LCD_BASE 0x38300000u
#define LCD_CMD (*(volatile uint32_t *)(LCD_BASE + 0x04))
#define LCD_STATUS (*(volatile uint32_t *)(LCD_BASE + 0x1c))
#define LCD_DATA (*(volatile uint32_t *)(LCD_BASE + 0x40))
#define LCD_CON (*(volatile uint32_t *)(LCD_BASE + 0x00))
#define LCD_FE_SIZE (*(volatile uint32_t *)(LCD_BASE + 0x74))
#define LCD_FE_GO (*(volatile uint32_t *)(LCD_BASE + 0x70))

#define LCD_W 320
#define LCD_H 240

static uint16_t fb[LCD_W*LCD_H] __attribute__((aligned(32)));

/* helpers */
static void lcd_cmd(uint8_t c) {
	while (!(LCD_STATUS & (1u<<1)));
	LCD_CMD = c;
}

static void lcd_data(uint32_t d) {
	while ((LCD_STATUS & (1u<<4)));
	LCD_DATA = d;
}

void lcd_flush(void) {
	LCD_CON = 0x80000c21;

	lcd_cmd(0x2A);
	lcd_data(0);
	lcd_data(0);
	lcd_data(0x01);
	lcd_data(0x3f);
	lcd_cmd(0x2B);
	lcd_data(0);
	lcd_data(0);
	lcd_data(0x00);
	lcd_data(0xef);
	lcd_cmd(0x2C);

	LCD_CON = 0x80100db1;

	for (int i = 0; i < LCD_W * LCD_H; i++)
		lcd_data(fb[i]);

	LCD_FE_SIZE = 0x014000f0;
	LCD_FE_GO = 1;
}

static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
	return (uint16_t)(((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3));
}

void fb_fill(uint16_t c) {
	for (int i = 0; i < LCD_W*LCD_H; i++)
		fb[i] = c;
}

void fb_pixel(int x, int y, uint16_t c) {
	if ((unsigned)x < LCD_W && (unsigned)y < LCD_H) fb[y * LCD_W + x] = c;
}

uint8_t arch_check_display(void)
{
	/* ipods always have it */
	return 1;
}

void arch_init_display(void)
{
	/* just temporary testing */
	dprintf("flushing lcd with black\n");
	fb_fill(0x0000);
	lcd_flush();

	dprintf("flushing lcd with white\n");
	fb_fill(0xFFFF);
	lcd_flush();
}
