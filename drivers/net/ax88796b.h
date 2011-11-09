/*
 * ASIX AX88796B Ethernet
 * (C) Copyright 2005-2006
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
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 */

#include <asm/types.h>
#include <config.h>


#ifdef CONFIG_DRIVER_AX88796B

struct ax88796b_pkt_hdr {
	unsigned char status; /* status */
	unsigned char next;   /* pointer to next packet. */
	unsigned short count; /* header + packet length in bytes */
};

struct ax88796b_private {
	unsigned wait_link:1;
	unsigned long end_time;
};

#define REG_SHIFT(x)					((x) << 1)

#define AX88796B_CMD					REG_SHIFT(0x00)
#define AX88796B_STARTPG				REG_SHIFT(0x01)
#define AX88796B_STOPPG					REG_SHIFT(0x02)
#define AX88796B_BOUNDARY				REG_SHIFT(0x03)
#define AX88796B_TRANSMITSTATUS			REG_SHIFT(0x04)
#define AX88796B_TRANSMITPAGE			REG_SHIFT(0x04)
#define AX88796B_TRANSMITBYTECOUNT0		REG_SHIFT(0x05)
#define AX88796B_NCR			 		REG_SHIFT(0x05)
#define AX88796B_TRANSMITBYTECOUNT1 	REG_SHIFT(0x06)
#define AX88796B_INTERRUPTSTATUS		REG_SHIFT(0x07)
#define AX88796B_CURRENT				REG_SHIFT(0x07)
#define AX88796B_REMOTESTARTADDRESS0 	REG_SHIFT(0x08)
#define AX88796B_CRDMA0  				REG_SHIFT(0x08)
#define AX88796B_REMOTESTARTADDRESS1 	REG_SHIFT(0x09)
#define AX88796B_CRDMA1 				REG_SHIFT(0x09)
#define AX88796B_REMOTEBYTECOUNT0		REG_SHIFT(0x0a)
#define AX88796B_REMOTEBYTECOUNT1		REG_SHIFT(0x0b)
#define AX88796B_RECEIVESTATUS			REG_SHIFT(0x0c)
#define AX88796B_RECEIVECONFIGURATION	REG_SHIFT(0x0c)
#define AX88796B_TRANSMITCONFIGURATION	REG_SHIFT(0x0d)
#define AX88796B_FAE_TALLY 				REG_SHIFT(0x0d)
#define AX88796B_DATACONFIGURATION		REG_SHIFT(0x0e)
#define AX88796B_CRC_TALLY 				REG_SHIFT(0x0e)
#define AX88796B_INTERRUPTMASK			REG_SHIFT(0x0f)
#define AX88796B_MISS_PKT_TALLY			REG_SHIFT(0x0f)
#define AX88796B_PHYSICALADDRESS0		REG_SHIFT(0x01)
#define AX88796B_PHYSICALADDRESS1		REG_SHIFT(0x02)
#define AX88796B_PHYSICALADDRESS2		REG_SHIFT(0x03)
#define AX88796B_PHYSICALADDRESS3		REG_SHIFT(0x04)
#define AX88796B_PHYSICALADDRESS4		REG_SHIFT(0x05)
#define AX88796B_PHYSICALADDRESS5		REG_SHIFT(0x06)
#define AX88796B_MULTIADDRESS0			REG_SHIFT(0x08)
#define AX88796B_MULTIADDRESS1			REG_SHIFT(0x09)
#define AX88796B_MULTIADDRESS2			REG_SHIFT(0x0a)
#define AX88796B_MULTIADDRESS3			REG_SHIFT(0x0b)
#define AX88796B_MULTIADDRESS4			REG_SHIFT(0x0c)
#define AX88796B_MULTIADDRESS5			REG_SHIFT(0x0d)
#define AX88796B_MULTIADDRESS6			REG_SHIFT(0x0e)
#define AX88796B_MULTIADDRESS7			REG_SHIFT(0x0f)
#define AX88796B_DMA_DATA				REG_SHIFT(0x10)
#define AX88796B_MII_EEPROM				REG_SHIFT(0x14)
#define AX88796B_STATUS					REG_SHIFT(0x17)
#define AX88796B_RESET					REG_SHIFT(0x1f)

/*  Register accessed at EN_CMD, the 8390 base addr.  */
#define AX88796B_STOP					0x01   /* Stop and reset the chip */
#define AX88796B_START					0x02   /* Start the chip, clear reset */
#define AX88796B_TRANS					0x04   /* Transmit a frame */
#define AX88796B_RREAD					0x08   /* Remote read */
#define AX88796B_RWRITE					0x10   /* Remote write  */
#define AX88796B_NODMA					0x20   /* Remote DMA */

#define AX88796B_PAGE0               	0x22
#define AX88796B_PAGE1               	0x62
#define AX88796B_PAGE2					0xA2
#define AX88796B_PAGE3					0xE2

#define AX88796B_PAGE0DMAWRITE       	0x12
#define AX88796B_PAGE2DMAWRITE       	0x92
#define AX88796B_REMOTEDMAWR         	0x12
#define AX88796B_REMOTEDMARD         	0x0A
#define AX88796B_ABORTDMAWR          	0x32
#define AX88796B_ABORTDMARD          	0x2A
#define AX88796B_PAGE0STOP           	0x21
#define AX88796B_PAGE1STOP           	0x61
#define AX88796B_TRANSMIT            	0x26
#define AX88796B_TXINPROGRESS        	0x04
#define AX88796B_SEND		    		0x1A

#define AX88796B_PSTART					0x4c
#define AX88796B_PSTOP					0x80
#define AX88796B_TPSTART				0x40

/* Bits in EN0_ISR - Interrupt status register */
#define ENISR_RX						0x01	/* Receiver, no error */
#define ENISR_TX						0x02	/* Transmitter, no error */
#define ENISR_RX_ERR					0x04	/* Receiver, with error */
#define ENISR_TX_ERR					0x08	/* Transmitter, with error */
#define ENISR_OVER						0x10	/* Receiver overwrote the ring */
#define ENISR_COUNTERS					0x20	/* Counters need emptying */
#define ENISR_RDC						0x40	/* remote dma complete */
#define ENISR_RESET						0x80	/* Reset completed */
#define ENISR_ALL						(ENISR_RX | ENISR_TX | ENISR_RX_ERR | ENISR_TX_ERR | ENISR_OVER | ENISR_COUNTERS)	/* Interrupts we will enable */

/*  MII interface support */
#define MDIO_SHIFT_CLK		0x01
#define MDIO_DATA_WRITE0	0x00
#define MDIO_DATA_WRITE1	0x08
#define MDIO_DATA_READ		0x04
#define MDIO_MASK			0x0f
#define MDIO_ENB_IN			0x02

#endif /*end of CONFIG_DRIVER_AX88796B*/
