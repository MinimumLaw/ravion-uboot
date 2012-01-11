/*
 * PXA3xx CPU Info
 *
 * Copyright (C) 2009 Marek Vasut <marek.vasut@gmail.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/************************************************************************/
/* ** HEADER FILES							*/
/************************************************************************/

#include <config.h>
#include <common.h>
#include <version.h>
#include <stdarg.h>
#include <linux/types.h>
#include <stdio_dev.h>
#include <lcd.h>
#include <asm/arch/pxa-regs.h>

#ifdef CONFIG_DISPLAY_CPUINFO

#ifdef CONFIG_CPU_MONAHANS
/* Improve the code, it's disgusting :-( */
int print_cpuinfo(void)
{
	uint32_t cpuid;
	asm volatile("mrc p15, 0, %0, c0, c0, 0" : "=r"(cpuid));

	puts("CPU: ");
	switch (cpuid & 0xf0) {
	case 0x80:
		puts("Marvell PXA30x");
		break;
	case 0x90:
		puts("Marvell PXA31x");
		break;
	case 0x20:
		puts("Marvell PXA32x");
		break;
	default:
		puts("Unknown");
		break;
	}
	puts(" ");

	switch (cpuid & 0xf) {
	case 0x01:
		puts("A1");
		break;
	case 0x02:
		puts("A2");
		break;
	case 0x03:
		puts("B0");
		break;
	case 0x05:
		puts("B1");
		break;
	case 0x06:
		puts("B2");
		break;
	case 0x07:
		puts("C0");
		break;
	default:
		puts("??");
		break;
	}
	puts("\n");

	puts("Clock: ");

	puts("Core: ");
	switch (ACSR & 0x1f) {
	case 0x08:
		puts("104");
		break;
	case 0x0c:
		puts("156");
		break;
	case 0x10:
		puts("208");
		break;
	case 0x18:
		puts("312");
		break;
	case 0x1f:
		puts("403");
		break;

	default:
		puts("??");
		break;
	}
	puts("MHz ; ");

	puts("Turbo: ");
	switch ((ACSR >> 8) & 0x07) {
	case 0x1:
		puts("x1");
		break;
	case 0x2:
		puts("x2");
		break;
	default:
		puts("??");
		break;
	}
	puts("\n");

	puts("       [ SMC: ");
	switch ((ACSR >> 23) & 0x07) {
	case 0x0:
		puts("78");
		break;
	case 0x2:
		puts("104");
		break;
	case 0x5:
		puts("208");
		break;
	default:
		puts("??");
		break;
	}
	puts("MHz ; ");

	puts("SRAM: ");
	switch ((ACSR >> 18) & 0x03) {
	case 0x0:
		puts("104");
		break;
	case 0x1:
		puts("156");
		break;
	case 0x2:
		puts("208");
		break;
	case 0x3:
		puts("312");
		break;
	default:
		puts("??");
		break;
	}
	puts("MHz ; ");

	puts("BUS: ");
	switch ((ACSR >> 14) & 0x03) {
	case 0x0:
		puts("104");
		break;
	case 0x1:
		puts("156");
		break;
	case 0x2:
		puts("208");
		break;
	case 0x3:
		puts("312");
		break;
	default:
		puts("??");
		break;
	}
	puts("MHz ; ");

	puts("DMC: ");
	switch ((ACSR >> 12) & 0x03) {
	case 0x0:
		puts("26");
		break;
	case 0x1:
		puts("312");
		break;
	case 0x3:
		puts("260");
		break;
	default:
		puts("??");
		break;
	}
	puts("MHz ]\n");

	return 0;
}
#else
int print_cpuinfo(void)
{
	return 0;
}
#endif
#endif
