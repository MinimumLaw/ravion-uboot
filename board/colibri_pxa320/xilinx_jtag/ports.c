/*
 * (C) Copyright 2003
 * Stefan Roese, esd gmbh germany, stefan.roese@esd-electronics.com
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

/*******************************************************/
/* file: ports.c                                       */
/* abstract:  This file contains the routines to       */
/*            output values on the JTAG ports, to read */
/*            the TDO bit, and to read a byte of data  */
/*            from the prom                            */
/*                                                     */
/*******************************************************/

#include <common.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/arch/pxa-regs.h>

#include "ports.h"

static unsigned long output = 0;
static int filepos = 0;
static int oldstate = 0;
static int newstate = 0;
static int readptr = 0;

extern const unsigned char *xsvfdata;


/* if in debugging mode, then just set the variables */
void setPort(short p,short val)
{
/* GPxR2:
 * 0x00400000 TDI
 * 0x00200000 TDO
 * 0x00080000 TCK
 * GPxR0:
 * 0x00000002 TMS
 * */

	switch(p) {
		case TMS:
			if (val)	GPSR0 = 0x00000002;
			else		GPCR0 = 0x00000002;
			break;
		case TDI:
			if (val)	GPSR2 = 0x00400000;
			else		GPCR2 = 0x00400000;
			break;
		case TCK:
			if (val)	GPSR2 = 0x00080000;
			else		GPCR2 = 0x00080000;
			break;
		default:
			break;
	}
}


/* toggle tck LH */
void pulseClock(void)
{
	setPort(TCK,0);  /* set the TCK port to low  */
	setPort(TCK,1);  /* set the TCK port to high */
}


/* read in a byte of data from the prom */
void readByte(unsigned char *data)
{
	/* pretend reading using a file */
	*data = xsvfdata[readptr++];
	newstate = filepos++ >> 10;
	if (newstate != oldstate) {
		printf("%4d kB\r\r\r\r", newstate);
		oldstate = newstate;
	}
}

/* read the TDO bit from port */
unsigned char readTDOBit(void)
{
	return !!(GPLR2 & 0x00200000);
}


/* Wait at least the specified number of microsec.                           */
/* Use a timer if possible; otherwise estimate the number of instructions    */
/* necessary to be run based on the microcontroller speed.  For this example */
/* we pulse the TCK port a number of times based on the processor speed.     */
void waitTime(long microsec)
{
	udelay(microsec); /* esd */
}
