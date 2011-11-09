/*
 * (C) Copyright 2009-2011
 * Marcel Ziswiler, Noser Engineering, marcel.ziswiler@noser.com.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
	. = 0x00000000;

	.text      :
	{
		LONG(0xea000029); /* version, use this field to skip the header */
		LONG(0x54494d48); /* identifier (TIMH) */
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x4e414e06); /* NAN 06 = 8bit nand device on cs0 */
		LONG(0x00000002); /* 2 images */
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x54494d48); /* image: TIMH */
		LONG(0x4f424d49); /* next image: OBMI */
		LONG(0x00000000); /* flash address */
		LONG(0x5c000800); /* load address */
		LONG(0x000000b0); /* size */
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x4f424d49); /* image: OBMI */
		LONG(0xffffffff); /* next image: *none* */
		LONG(0x00000000); /* flash address */
		LONG(0x5c012f40); /* load address */
		LONG(0x00004000); /* size */
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		LONG(0x00000000);
		start.o	(.text)
		lowlevel_init.o	(.text)
		nand_boot.o	(.text)
		pxa3xx_nand.o	(.text)

		*(.text)
		*(.fixup)
	}

	_etext = .;

	.data    :
	{
		*(.rodata*)
		*(.data*)
		*(.sdata*)
		__got2_start = .;
		*(.got2)
		__got2_end = .;
	}

	_edata  =  .;

	. = ALIGN(4);
	__bss_start = .;
	.bss : { *(.bss) }
	_end = .;
}
