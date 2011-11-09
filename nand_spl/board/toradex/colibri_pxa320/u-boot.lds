/*
 * (C) Copyright 2007-2011
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
		/* IPL loads code from NAND with a 8 byte (2 word) offset */
		LONG(0xEA001FFE);
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
