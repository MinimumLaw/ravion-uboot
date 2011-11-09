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
 * 
 * Module Name	: ax88796b.c
 * Date		: 2008-09-09
 * History
 * 1.0.0 09/06/2006	: Initial release.
 * 1.1.0 09/09/2008	: Added Phy power process functions.
 *                     Ported to support new driver modal.
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <net.h>
#include <miiphy.h>
#include "ax88796b.h"

#if defined (CONFIG_S3C2440A_SMDK)
#include <s3c2440.h>
#endif

#define	SEC_TO_TICKS(seconds)	((uint64_t)(seconds) * get_tbclk())

/* packet page register access functions */

/* Note: without delays we get unexpected packet corruptions! */

static inline unsigned char get_reg (struct eth_device *dev, unsigned int regno)
{
	udelay (1); /* wait for 1ms */
	return (*((unsigned char *)(dev->iobase + regno)));
}

static inline unsigned short get_reg16 (struct eth_device *dev, unsigned int regno)
{
	udelay (1); /* wait for 1ms */
	return (*((unsigned short *)(dev->iobase + regno)));
}

static inline void put_reg (struct eth_device *dev, unsigned int regno, unsigned char val)
{
	udelay (1); /* wait for 1ms */
	*((volatile unsigned char *)(dev->iobase + regno)) = val;
}

static inline void put_reg16 (struct eth_device *dev, unsigned int regno, unsigned short val)
{
	udelay (1); /* wait for 1ms */
	*((volatile unsigned short *)(dev->iobase + regno)) = val;
}

static void ax88796b_reset (struct eth_device *dev)
{
	get_reg (dev, AX88796B_RESET);
	put_reg (dev, AX88796B_INTERRUPTSTATUS, 0xff);
	udelay (2000);		/* wait for 2ms */
}

static void mdio_sync (struct eth_device *dev)
{
	int bits;
	for (bits = 0; bits < 32; bits++) {
		put_reg (dev, AX88796B_MII_EEPROM, MDIO_DATA_WRITE1);
		put_reg (dev, AX88796B_MII_EEPROM, (MDIO_DATA_WRITE1 | MDIO_SHIFT_CLK));
	}
}

static void mdio_clear (struct eth_device *dev)
{
	int bits;
	for (bits = 0; bits < 16; bits++) {
		put_reg (dev, AX88796B_MII_EEPROM, MDIO_DATA_WRITE0);
		put_reg (dev, AX88796B_MII_EEPROM, (MDIO_DATA_WRITE0 | MDIO_SHIFT_CLK));
	}
}

#if 0
//currently unused
static int mdio_read (struct eth_device *dev, int phy_id, int loc)
{
	unsigned long cmd = (0xf6 << 10) | (phy_id << 5) | loc;
	int i, retval = 0;
	unsigned char data;

	mdio_clear (dev);
	mdio_sync (dev);

	for (i = 14; i >= 0; i--) {
		data = (cmd & (1<<i)) ? MDIO_DATA_WRITE1 : MDIO_DATA_WRITE0;
		put_reg (dev, AX88796B_MII_EEPROM, data);
		put_reg (dev, AX88796B_MII_EEPROM, (data | MDIO_SHIFT_CLK));
	}

	for (i = 19; i > 0; i--) {
		put_reg (dev, AX88796B_MII_EEPROM, MDIO_ENB_IN);
		retval = (retval << 1) | ((get_reg (dev, AX88796B_MII_EEPROM) & MDIO_DATA_READ) != 0);
		put_reg (dev, AX88796B_MII_EEPROM, (MDIO_ENB_IN | MDIO_SHIFT_CLK));
	}

	return (retval>>1) & 0xffff;
}
#endif

static void mdio_write (struct eth_device *dev, int phy_id, int loc, int value)
{
	unsigned long cmd = (0x05 << 28) | (phy_id << 23) | (loc << 18) | (1 << 17) | value;
	int i;
	unsigned char data;

	mdio_clear (dev);
	mdio_sync (dev);

	for (i = 31; i >= 0; i--) {
		data = (cmd & (1<<i)) ? MDIO_DATA_WRITE1 : MDIO_DATA_WRITE0;
		put_reg (dev, AX88796B_MII_EEPROM, data);
		put_reg (dev, AX88796B_MII_EEPROM, (data | MDIO_SHIFT_CLK));
	}

	for (i = 1; i >= 0; i--) {
		put_reg (dev, AX88796B_MII_EEPROM, MDIO_ENB_IN);
		put_reg (dev, AX88796B_MII_EEPROM, (MDIO_ENB_IN | MDIO_SHIFT_CLK));
	}
}

static void ax88796b_get_enetaddr (struct eth_device *dev)
{
	int i;

	put_reg (dev, AX88796B_CMD, AX88796B_REMOTEDMARD);
	put_reg (dev, AX88796B_DATACONFIGURATION, 0x01);
	put_reg (dev, AX88796B_REMOTESTARTADDRESS0, 0x00);
	put_reg (dev, AX88796B_REMOTESTARTADDRESS1, 0x00);
	put_reg (dev, AX88796B_REMOTEBYTECOUNT0, 12);
	put_reg (dev, AX88796B_REMOTEBYTECOUNT1, 0x00);
	put_reg (dev, AX88796B_CMD, AX88796B_REMOTEDMARD);

	for (i = 0; i < 6; i++) {
		dev->enetaddr[i] = (unsigned char) get_reg16 (dev, AX88796B_DMA_DATA);
	}
	while ((!get_reg (dev, AX88796B_INTERRUPTSTATUS) & 0x40));

	put_reg (dev, AX88796B_REMOTEBYTECOUNT0, 0x00);
	put_reg (dev, AX88796B_REMOTEBYTECOUNT1, 0x00);
	put_reg (dev, AX88796B_CMD, AX88796B_PAGE0);
}

static void ax88796b_halt (struct eth_device *dev)
{
	put_reg (dev, AX88796B_CMD, AX88796B_STOP);
}

static int ax88796b_init (struct eth_device *dev, bd_t * bd)
{
#if defined (CONFIG_S3C2440A_SMDK)
	/* 16-bit mode */
	BWSCON = ( BWSCON & ~(0xf<<4)) | (0xd << 4);
	BANKCON1 = ( 3<<11)|(0x7<<8)|(0x1<<6)|(0x3<<4)|(0x3<<2);
#endif

	put_reg (dev, AX88796B_CMD, AX88796B_PAGE0STOP);
	put_reg (dev, AX88796B_DATACONFIGURATION, 0x01);
	put_reg (dev, AX88796B_REMOTEBYTECOUNT0, 0x00);
	put_reg (dev, AX88796B_REMOTEBYTECOUNT1, 0x00);
	put_reg (dev, AX88796B_RECEIVECONFIGURATION, 0x00);
	put_reg (dev, AX88796B_TRANSMITPAGE, AX88796B_TPSTART);
	put_reg (dev, AX88796B_TRANSMITCONFIGURATION, 0x02);
	put_reg (dev, AX88796B_STARTPG, AX88796B_PSTART);
	put_reg (dev, AX88796B_BOUNDARY, AX88796B_PSTART);
	put_reg (dev, AX88796B_STOPPG, AX88796B_PSTOP);
	put_reg (dev, AX88796B_INTERRUPTSTATUS, 0xff);
	put_reg (dev, AX88796B_INTERRUPTMASK, 0x11);
	put_reg (dev, AX88796B_CMD, AX88796B_PAGE1STOP);
	put_reg (dev, AX88796B_PHYSICALADDRESS0, dev->enetaddr[0]);
	put_reg (dev, AX88796B_PHYSICALADDRESS1, dev->enetaddr[1]);
	put_reg (dev, AX88796B_PHYSICALADDRESS2, dev->enetaddr[2]);
	put_reg (dev, AX88796B_PHYSICALADDRESS3, dev->enetaddr[3]);
	put_reg (dev, AX88796B_PHYSICALADDRESS4, dev->enetaddr[4]);
	put_reg (dev, AX88796B_PHYSICALADDRESS5, dev->enetaddr[5]);
	put_reg (dev, AX88796B_MULTIADDRESS0, 0x00);
	put_reg (dev, AX88796B_MULTIADDRESS1, 0x00);
	put_reg (dev, AX88796B_MULTIADDRESS2, 0x00);
	put_reg (dev, AX88796B_MULTIADDRESS3, 0x00);
	put_reg (dev, AX88796B_MULTIADDRESS4, 0x00);
	put_reg (dev, AX88796B_MULTIADDRESS5, 0x00);
	put_reg (dev, AX88796B_MULTIADDRESS6, 0x00);
	put_reg (dev, AX88796B_MULTIADDRESS7, 0x00);
	put_reg (dev, AX88796B_CURRENT, AX88796B_PSTART);
	put_reg (dev, AX88796B_CMD, AX88796B_PAGE0);
	put_reg (dev, AX88796B_TRANSMITCONFIGURATION, 0xe0);	/*58; */

	return 0;
}

static unsigned char nic_to_pc (struct eth_device *dev)
{
	unsigned char rec_head_status;
	unsigned char next_packet_pointer;
	unsigned short rxlen = 0;
	unsigned int i, tmp;
	unsigned char current_point;
	unsigned char *addr;
	struct ax88796b_pkt_hdr	hdr;
	/*
	 * The first 4B is packet status,page of next packet
	 * and packet length(2B).So we receive the fist 4B.
	 */
	put_reg (dev, AX88796B_REMOTESTARTADDRESS1, get_reg (dev, AX88796B_BOUNDARY));
	put_reg (dev, AX88796B_REMOTESTARTADDRESS0, 0x00);
	put_reg (dev, AX88796B_REMOTEBYTECOUNT1, 0x00);
	put_reg (dev, AX88796B_REMOTEBYTECOUNT0, 0x04);

	put_reg (dev, AX88796B_CMD, AX88796B_REMOTEDMARD);
//printf("receive: ");
	for(i=0; i < 2; i++)
{
		*((u16 *)&hdr + i)= get_reg16 (dev, AX88796B_DMA_DATA);
//printf("0x%04X ", *((u16 *)&hdr + i));
}
//printf("\n");
	rec_head_status = hdr.status;
	next_packet_pointer = hdr.next;
	rxlen = hdr.count - 4;
	put_reg (dev, AX88796B_CMD, AX88796B_PAGE0);

	if (rxlen > PKTSIZE_ALIGN + PKTALIGN)
		printf ("packet too big!\n");

	/*Receive the packet */
	put_reg (dev, AX88796B_REMOTESTARTADDRESS0, 0x04);
	put_reg (dev, AX88796B_REMOTESTARTADDRESS1, get_reg (dev, AX88796B_BOUNDARY));

	put_reg (dev, AX88796B_REMOTEBYTECOUNT0, (rxlen & 0xff));
	put_reg (dev, AX88796B_REMOTEBYTECOUNT1, ((rxlen >> 8) & 0xff));


	put_reg (dev, AX88796B_CMD, AX88796B_REMOTEDMARD);

	tmp = rxlen >> 1;
	addr = (unsigned char *) NetRxPackets[0];
	for(i=0; i < tmp; i++) {
		*((u16 *)addr + i) = get_reg16 (dev, AX88796B_DMA_DATA);
//printf("%02x %02x ", *((u8 *)addr + 2*i), *((u8 *)addr + 2*i + 1));
	}
	if (rxlen & 0x01)
{
		addr[rxlen-1] = get_reg (dev, AX88796B_DMA_DATA);
//printf("%02x ", addr[rxlen-1]);
}
//printf("\n");
	/* Pass the packet up to the protocol layers. */
	NetReceive (NetRxPackets[0], rxlen);

	while (!(get_reg (dev, AX88796B_INTERRUPTSTATUS)) & 0x40);	/* wait for the op. */

	/*
	 * To test whether the packets are all received,get the
	 * location of current point
	 */
	put_reg (dev, AX88796B_CMD, AX88796B_PAGE1);
	current_point = get_reg (dev, AX88796B_CURRENT);
	put_reg (dev, AX88796B_CMD, AX88796B_PAGE0);
	put_reg (dev, AX88796B_BOUNDARY, next_packet_pointer);
	return current_point;
}

/* Get a data block via Ethernet */
static int ax88796b_recv (struct eth_device *dev)
{
	struct ax88796b_private *ax_local = (struct ax88796b_private *)dev->priv;
	unsigned char temp, current_point, status;

	put_reg (dev, AX88796B_CMD, AX88796B_PAGE0);

	/* Check for link status */
	status = get_reg (dev, AX88796B_STATUS);
	if (!(status & 1)) {
		if (!ax_local->wait_link) {
			/* power down phy */
			mdio_write (dev, 0x10, PHY_BMCR, PHY_BMCR_POWD);
			udelay (1000);		/* wait for 1ms */
			/* power up phy */
			mdio_write (dev, 0x10, PHY_BMCR, PHY_BMCR_AUTON);
			udelay (60000);		/* wait for 60ms */
			/* restart phy autoneg */
			mdio_write (dev, 0x10, PHY_BMCR, (PHY_BMCR_AUTON | PHY_BMCR_RST_NEG));

			ax_local->wait_link = 1;
			ax_local->end_time = SEC_TO_TICKS (3) + get_ticks();
		} else {
			if (get_ticks() > ax_local->end_time) {
				/* set wait_link = 0 to retry phy power process */
				ax_local->wait_link = 0;
			}
		}
	} else {
		ax_local->wait_link = 0;
	}

	while (1) {
		temp = get_reg (dev, AX88796B_INTERRUPTSTATUS);

		if (temp & ENISR_OVER) {
			/*overflow */
			put_reg (dev, AX88796B_CMD, AX88796B_PAGE0STOP);
			udelay (2000);
			put_reg (dev, AX88796B_REMOTEBYTECOUNT0, 0);
			put_reg (dev, AX88796B_REMOTEBYTECOUNT1, 0);
			put_reg (dev, AX88796B_TRANSMITCONFIGURATION, 2);

			do {
				current_point = nic_to_pc (dev);
			} while (get_reg (dev, AX88796B_BOUNDARY) != current_point);

			put_reg (dev, AX88796B_TRANSMITCONFIGURATION, 0xe0);
		}

		if (temp & ENISR_RX) {
			/*packet received */
			do {
				put_reg (dev, AX88796B_INTERRUPTSTATUS, 0x01);
				current_point = nic_to_pc (dev);
			} while (get_reg (dev, AX88796B_BOUNDARY) != current_point);
		}

		if (!(temp & ENISR_RX))
			return 0;
		/* done and exit. */
	}
}

/* Send a data block via Ethernet. */
static int ax88796b_send (struct eth_device *dev, volatile void *packet, int length)
{
	unsigned int i;

	while (get_reg (dev, AX88796B_CMD) == AX88796B_TRANSMIT);

	put_reg (dev, AX88796B_REMOTESTARTADDRESS0, 0);
	put_reg (dev, AX88796B_REMOTESTARTADDRESS1, AX88796B_TPSTART);
	put_reg (dev, AX88796B_REMOTEBYTECOUNT0, (length & 0xff));
	put_reg (dev, AX88796B_REMOTEBYTECOUNT1, ((length >> 8) & 0xff));

	put_reg (dev, AX88796B_CMD, AX88796B_REMOTEDMAWR);
//printf("send: ");
	for (i = 0; i < length; i += 2) {
//printf("0x%04X ", *((volatile unsigned short *)(packet + i)));
		put_reg16 (dev, AX88796B_DMA_DATA, *((volatile unsigned short *)(packet + i)));
	}
//printf("\n");
	while (!(get_reg (dev, AX88796B_INTERRUPTSTATUS)) & 0x40);

	put_reg (dev, AX88796B_INTERRUPTSTATUS, 0x40);
	put_reg (dev, AX88796B_TRANSMITPAGE, AX88796B_TPSTART);
	if (length < 60)
		put_reg (dev, AX88796B_TRANSMITBYTECOUNT0, 60);
	else
		put_reg (dev, AX88796B_TRANSMITBYTECOUNT0, (length & 0xff));
	put_reg (dev, AX88796B_TRANSMITBYTECOUNT1, ((length >> 8 & 0xff)));
	put_reg (dev, AX88796B_CMD, AX88796B_TRANSMIT);

	return 0;
}

/* Set MAC address in chip */
static int ax88796b_write_hwaddr(struct eth_device *dev)
{
	/* Switch to page 1 */
	put_reg (dev, AX88796B_CMD, AX88796B_PAGE1STOP);

	/* Fill device MAC address registers */
	put_reg (dev, AX88796B_PHYSICALADDRESS0, dev->enetaddr[0]);
	put_reg (dev, AX88796B_PHYSICALADDRESS1, dev->enetaddr[1]);
	put_reg (dev, AX88796B_PHYSICALADDRESS2, dev->enetaddr[2]);
	put_reg (dev, AX88796B_PHYSICALADDRESS3, dev->enetaddr[3]);
	put_reg (dev, AX88796B_PHYSICALADDRESS4, dev->enetaddr[4]);
	put_reg (dev, AX88796B_PHYSICALADDRESS5, dev->enetaddr[5]);

        return 0;
}

/*
===========================================================================
<<<<<<                 Exported SubProgram Bodies              >>>>>>
===========================================================================
*/
int ax88796b_initialize (bd_t *bis)
{
	struct eth_device *dev;
	struct ax88796b_private *ax_local;

	dev = (struct eth_device *)malloc (sizeof *dev);
	if (NULL == dev)
		return 0;

	ax_local = (struct ax88796b_private *)malloc (sizeof *ax_local);
	if (NULL == ax_local)
		return 0;

	memset (dev, 0, sizeof *dev);
	memset (ax_local, 0, sizeof *ax_local);

	sprintf (dev->name, "ax88796b");
	dev->iobase = AX88796B_BASE;
	dev->priv = ax_local;
	dev->init = ax88796b_init;
	dev->halt = ax88796b_halt;
	dev->send = ax88796b_send;
	dev->recv = ax88796b_recv;
	dev->write_hwaddr = ax88796b_write_hwaddr;

	ax88796b_reset (dev);

	ax88796b_get_enetaddr (dev);

	eth_register (dev);

	return 1;
}
