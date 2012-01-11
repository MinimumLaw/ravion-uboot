/*
 * Toradex Colibri PXA320 support file
 *
 * Copyright (C) 2009 Marek Vasut <marek.vasut@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/pxa-regs.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

#define	CPLD_REG_CS_CTRL			0x17800000
#define	CPLD_REG_CS_CTRL_EXT_nCS0_EN		(1 << 0)
#define	CPLD_REG_CS_CTRL_EXT_nCS1_EN		(1 << 1)
#define	CPLD_REG_CS_CTRL_EXT_nCS2_EN		(1 << 2)
#define	CPLD_REG_CS_CTRL_EXT_nCS0_EC_EN		(1 << 5)
#define	CPLD_REG_CS_CTRL_EXT_nCS1_EC_EN		(1 << 6)
#define	CPLD_REG_CS_CTRL_EXT_nCS2_EC_EN		(1 << 7)
#define	CPLD_REG_CS_CTRL_EXT_nCS0_DIS		(1 << 8)
#define	CPLD_REG_CS_CTRL_EXT_nCS1_DIS		(1 << 9)
#define	CPLD_REG_CS_CTRL_EXT_nCS2_DIS		(1 << 10)
#define	CPLD_REG_CS_CTRL_EXT_nCS0_EC_DIS	(1 << 11)
#define	CPLD_REG_CS_CTRL_EXT_nCS1_EC_DIS	(1 << 12)
#define	CPLD_REG_CS_CTRL_EXT_nCS2_EC_DIS	(1 << 13)

#define	CPLD_REG_CS_MEM				0x17800004
#define	CPLD_REG_CS_MEM_CF_EN			(1 << 0)
#define	CPLD_REG_CS_MEM_RDnWR_EN		(1 << 1)
#define	CPLD_REG_CS_MEM_nOE_EN			(1 << 2)
#define	CPLD_REG_CS_MEM_CF_DIS			(1 << 8)
#define	CPLD_REG_CS_MEM_RDnWR_DIS		(1 << 9)
#define	CPLD_REG_CS_MEM_nOE_DIS			(1 << 10)

/*
 * Miscelaneous platform dependent initialisations
 */

int board_init (void)
{
	/* arch number for linux kernel */
	gd->bd->bi_arch_number = MACH_TYPE_COLIBRI320;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0xa0000100;

	/* Ethernet chip configuration */
	GPIO3		= 0x00000801;
	MSC1		= 0x7ff8023c;
	CSADRCFG2	= 0x0032c80b;

	/* Chipselect 3 configuration */
	GPIO4		= 0x00000801;
	CSADRCFG3	= 0x0032c809;

	/* Configuration for Compact Flash */
	CSADRCFG_P	= 0x0038080c;
	CSMSADRCFG	= 0x00000002;

	/* Configuration for the CPLD */
#if 0	/* Disabled due to bug in CPLD FW v1.6 */
	writew (CPLD_REG_CS_CTRL_EXT_nCS0_EN |
		CPLD_REG_CS_CTRL_EXT_nCS1_EN |
		CPLD_REG_CS_CTRL_EXT_nCS2_EN,
		CPLD_REG_CS_CTRL);
#endif
	writew (CPLD_REG_CS_MEM_CF_EN |
		CPLD_REG_CS_MEM_RDnWR_EN |
		CPLD_REG_CS_MEM_nOE_EN,
		CPLD_REG_CS_MEM);

	/* CPLD programming interface */
	GPIO1 = 0xd887;
	GPIO1_2 = 0xd880;
	GPIO83 = 0xd880;
	GPIO85 = 0xd880;
	GPIO86 = 0xd880;

	GPCR0 = 0x2;
	GPDR0 |= 0x2;

	GPCR2 = 0x00680000;
	GPDR2 |= 0x00480000;
	GPDR2 &= ~0x00200000;

	/* MMC */
	GPIO18 = 0x804;
	GPIO19 = 0x804;
	GPIO20 = 0x804;
	GPIO21 = 0x804;
	GPIO22 = 0x804;
	GPIO23 = 0x804;

	return 0;
}

int board_late_init(void)
{
	setenv("stdout", "serial");
	setenv("stderr", "serial");
	return 0;
}

int dram_init (void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
	return 0;
}

#ifdef	CONFIG_CMD_USB
int usb_board_init(void)
{
	UHCHR = (UHCHR | UHCHR_PCPL | UHCHR_PSPL) &
		~(UHCHR_SSEP0 | UHCHR_SSEP1 | UHCHR_SSEP2 | UHCHR_SSE);

	UHCHR |= UHCHR_FSBIR;

	while (UHCHR & UHCHR_FSBIR);

	UHCHR &= ~UHCHR_SSE;
	UHCHIE = (UHCHIE_UPRIE | UHCHIE_RWIE);

	UHCRHDA &= ~(0x200);
	UHCRHDA |= 0x100;

	/* Set port power control mask bits, only 3 ports. */
	UHCRHDB |= (0x7<<17);

	/* enable port 2 */
	UP2OCR |= UP2OCR_HXOE | UP2OCR_HXS | UP2OCR_DMPDE | UP2OCR_DPPDE;

//	CKENA &= ~CKENA_2_USBHOST;

	return 0;
}

void usb_board_init_fail(void)
{
	return;
}

void usb_board_stop(void)
{
	UHCHR |= UHCHR_FHR;
	udelay(11);
	UHCHR &= ~UHCHR_FHR;

	UHCCOMS |= 1;
	udelay(10);

	CKENA &= ~CKENA_2_USBHOST;

	return;
}
#endif
