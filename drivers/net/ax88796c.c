/*
 * ASIX AX88796C Ethernet
 * (C) Copyright 2010
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
 * History:
 *
 *   1.1.0  04/26/2010    Add watchdog function.
 *
 *   1.0.0  03/15/2010    Initial release.
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <net.h>
#include <asm/errno.h>
#include <miiphy.h>
#include "ax88796c.h"

#if defined (CONFIG_S3C2440A_SMDK)
#include <s3c2440.h>
#endif

/* Note: without delays we get unexpected packet corruptions! */

#if (CONFIG_16BIT_MODE)
static inline unsigned short get_reg16 (struct eth_device *dev, 
		unsigned int regno)
{
	udelay(1); /* wait for 1ms */
	return (*((unsigned short *)(dev->iobase + regno)));
}

static inline void put_reg16 (struct eth_device *dev, unsigned int regno, 
		unsigned short val)
{
	udelay(1); /* wait for 1ms */
	*((volatile unsigned short *)(dev->iobase + regno)) = val;
}
#else
static inline unsigned short get_reg16 (struct eth_device *dev, 
		unsigned int regno)
{
	unsigned short data;
	data = *((unsigned char *)dev->iobase + regno);
	data |= (*((unsigned char *)dev->iobase + regno + 1) << 8);
	return data;
}

static inline void put_reg16 (struct eth_device *dev, unsigned int regno, 
		unsigned short val)
{
	*((unsigned char *) dev->iobase + regno) = (val & 0x00FF);
	*((unsigned char *) dev->iobase + regno + 1) = (val >> 8);
}
#endif

static int ax88796c_reset (struct eth_device *dev)
{
	unsigned long time_out;

	put_reg16 (dev, P0_PSR, PSR_RESET);
	put_reg16 (dev, P0_PSR, PSR_RESET_CLR);

	time_out = get_timer(0) + (CONFIG_SYS_HZ / 100);
	while (!(get_reg16 (dev, P0_PSR) & PSR_DEV_READY)) {
		if (get_timer(0) > time_out) {
			return -ENXIO;
		}
	}
	return 0;
}

static int ax88796c_mdio_write (struct eth_device *dev, int phy_id, int loc, int value)
{
	unsigned short reg;
	unsigned long time_out;

	reg = get_reg16 (dev, P0_PSR);
	put_reg16 (dev, P0_PSR, (reg & PSR_PAGE_MASK) | PAGE2);
	put_reg16 (dev, P2_MRDR, value);
	put_reg16 (dev, P2_MRCR, MRCR_RADDR(loc) | 
			MRCR_FADDR(phy_id) | MRCR_WRITE);

	time_out = get_timer(0) + (CONFIG_SYS_HZ / 100);
	while ((get_reg16 (dev, P2_MRCR) & MRCR_VALID) == 0) {
		if (get_timer(0) > time_out) {
			put_reg16 (dev, P0_PSR, reg);
			return -EIO;
		}
	}
	put_reg16 (dev, P0_PSR, reg);
	return 0;

}

unsigned short ax88796c_mdio_read (struct eth_device *dev, int phy_id, int loc)
{
	unsigned short reg;
	unsigned long time_out;
	unsigned short val;


	reg = get_reg16 (dev, P0_PSR);
	put_reg16 (dev, P0_PSR, (reg & PSR_PAGE_MASK) | PAGE2);
	put_reg16 (dev, P2_MRCR, MRCR_RADDR(loc) | 
			MRCR_FADDR(phy_id) | MRCR_READ);

	time_out = get_timer(0) + (CONFIG_SYS_HZ / 100);
	while ((get_reg16 (dev, P2_MRCR) & MRCR_VALID) == 0) {
		if (get_timer(0) > time_out) {
			put_reg16 (dev, P0_PSR, reg);
			return -EIO;
		}
	}
	val = get_reg16 (dev, P2_MRDR);

	put_reg16 (dev, P0_PSR, reg);

	return val;
}

static void ax88796c_set_enetaddr (struct eth_device *dev)
{
	int reg;

	reg = get_reg16 (dev, P0_PSR);
	put_reg16 (dev, P0_PSR, (reg & PSR_PAGE_MASK) | PAGE3);
	put_reg16 (dev, P3_MACASR0, (dev->enetaddr[4] << 8) | dev->enetaddr[5]);
	put_reg16 (dev, P3_MACASR1, (dev->enetaddr[2] << 8) | dev->enetaddr[3]);
	put_reg16 (dev, P3_MACASR2, (dev->enetaddr[0] << 8) | dev->enetaddr[1]);
	put_reg16 (dev, P0_PSR, reg);
}

#ifndef AXFLG_NO_EEPROM
static void ax88796c_get_enetaddr (struct eth_device *dev)
{
	int reg;
	unsigned short mac_addr;

	reg = get_reg16 (dev, P0_PSR);
	/*Get MAC Address from Registers*/
	put_reg16 (dev, P0_PSR, (reg & PSR_PAGE_MASK) | PAGE3);
	mac_addr = get_reg16 (dev, P3_MACASR0);
	dev->enetaddr[5] = mac_addr & 0x00FF;
	dev->enetaddr[4] = mac_addr >> 8;
	mac_addr = get_reg16 (dev, P3_MACASR1);
	dev->enetaddr[3] = mac_addr & 0x00FF;
	dev->enetaddr[2] = mac_addr >> 8;
	mac_addr = get_reg16 (dev, P3_MACASR2);
	dev->enetaddr[1] = mac_addr & 0x00FF;
	dev->enetaddr[0] = mac_addr >> 8;


	if ((dev->enetaddr[0] & 0x00) ||
	    ((dev->enetaddr[0] == 0) && (dev->enetaddr[1] == 0) &&
	    (dev->enetaddr[2] == 0) && (dev->enetaddr[3] == 0) &&
	    (dev->enetaddr[4] == 0) && (dev->enetaddr[5] == 0))) {
		dev->enetaddr[0] = 0x08;
		dev->enetaddr[1] = 0x00;
		dev->enetaddr[2] = 0x3e;
		dev->enetaddr[3] = 0x26;
		dev->enetaddr[4] = 0x0a;
		dev->enetaddr[5] = 0x5b;
	}
	put_reg16 (dev, P0_PSR, reg);
}
#endif /* AXFLG_NO_EEPROM */

static void ax88796c_halt (struct eth_device *dev)
{
	/*STOP RX*/
	put_reg16 (dev, P0_MACCR,  get_reg16 (dev, P0_MACCR) | MACCR_RXPATH);
	put_reg16 (dev, P0_TSNR, TSNR_TXB_REINIT);
}

static int ax88796c_init (struct eth_device *dev, bd_t * bd)
{
	unsigned short reg;
	struct ax88796c_private *ax_local = 
		(struct ax88796c_private *) dev->priv;

	ax_local->seq_num = 0 ;
	ax_local->link = 0;

	reg = get_reg16 (dev, P0_PSR);
	put_reg16 (dev, P0_PSR, (reg & PSR_PAGE_MASK) | PAGE1);

	/*Set RX Packet Count Threshold*/
	put_reg16 (dev, P1_RPCTR, 0x01);

	/*Disable RX Stuffing Padding*/
	put_reg16 (dev, P1_RXBSPCR, 0);

	/*Byte Swap, Enable TX RX bridge*/
	put_reg16 (dev, P0_PSR, (reg & PSR_PAGE_MASK) | PAGE0);
	put_reg16 (dev, P0_FER, get_reg16 (dev, P0_FER) | FER_BSWAP | FER_RXEN 
		| FER_TXEN);

	/*Set MAC address*/
	ax88796c_set_enetaddr(dev);

	/*Set Unicast + Broadcast*/
	put_reg16 (dev, P0_PSR, (reg & PSR_PAGE_MASK) | PAGE2);
	put_reg16 (dev, P2_RXCR, RXCR_BROADCAST);

	/* Init PHY auto-polling */
	put_reg16 (dev, P2_POOLCR, (PHY_ID << 8) | POOLCR_POLL_EN | 
		POOLCR_POLL_BMCR);

	/* Return to page 0 */
	put_reg16 (dev, P0_PSR, (reg & PSR_PAGE_MASK));

	ax_local->w_state = chk_cable;
	ax_local->timeout = get_timer(0) + CONFIG_SYS_HZ;
	return 0;
}

static unsigned char nic_to_pc (struct eth_device *dev)
{
	unsigned short rcphr, rxlen, pio_len, i, burst_len;
	unsigned char *addr;
	unsigned long time_out;
	struct rx_header hdr;

	rcphr = get_reg16 (dev, P0_RCPHR);
	/*Check the correctness of packet */
	if (!((rcphr & RX_HDR_ERROR) == 0)) {
		put_reg16 (dev, P0_RXBCR1, RXBCR1_RXB_DISCARD);
		return -ENOMEM;
	}

	rxlen = (rcphr & RX_HDR_LEN);
	
	if (rxlen > PKTSIZE_ALIGN + PKTALIGN)
		printf ("packet too big!\n");
	/* 
	 * Burst Word Count (header length + packet length has
	 * to be double word alignment)
	 */
	burst_len = ((rxlen + sizeof(struct rx_header) + 3) & 0xFFFC) >> 1;
	put_reg16 (dev, P0_RXBCR1, (burst_len | RXBCR1_RXB_START));
	time_out = get_timer(0) + (CONFIG_SYS_HZ / 100);
	while ((get_reg16 (dev, P0_RXBCR2) & RXBCR2_RXB_READY) == 0) {
		if (get_timer(0) > time_out) {
			goto error_out;
		}
	}
	/*Receive RX Header*/
	hdr.hdr1.flags_len = get_reg16 (dev, P0_RTDPR);
	hdr.hdr1.seq_lenbar = get_reg16 (dev, P0_RTDPR);
	hdr.hdr2 = get_reg16 (dev, P0_RTDPR);

	/*Receive RX Data*/
	/*Packet length + RX header 2 has to be double word alignment*/
	pio_len = (((rxlen + 2 + 3) & 0xFFFC) >> 1) - 1;
	addr = (unsigned char *) NetRxPackets[0];
	for (i = 0; i < pio_len ; i++) {
		*((u16 *) addr + i) = get_reg16 (dev, P0_RTDPR + 
		DATA_PORT_ADDR);
	}
	time_out = get_timer(0) + (CONFIG_SYS_HZ / 100);
	while ((get_reg16 (dev, P0_RXBCR2) & RXBCR2_RXB_IDLE) == 0) {
		if (get_timer(0) > time_out) {
			goto error_out;
		}
	}
	/* Pass the packet up to the protocol layers. */
	NetReceive (NetRxPackets[0], rxlen);
	return 0;
error_out:
	put_reg16 (dev, P0_RXBCR2, RXBCR2_RXB_REINIT);
	return -ENOMEM;
	
}

/*
 * ----------------------------------------------------------------------------
 * Function Name: ax88796c_check_media
 * Purpose: Process media link status
 * ----------------------------------------------------------------------------
 */
static void ax88796c_check_media (struct eth_device *dev)
{
	struct ax88796c_private *ax_local = 
		(struct ax88796c_private *)dev->priv;
	u16 bmsr;

	bmsr = ax88796c_mdio_read (dev, PHY_ID, PHY_BMSR);
	bmsr = ax88796c_mdio_read (dev, PHY_ID, PHY_BMSR);

	if (!(bmsr & PHY_BMSR_LS) && ax_local->link) {
		ax_local->link = 0;
		ax_local->w_state = chk_cable;
		ax_local->timeout = get_timer(0) + CONFIG_SYS_HZ;
	} else if ((bmsr & PHY_BMSR_LS) && !ax_local->link){
		ax_local->link = 1;
	}

	return;
}

static void ax88796c_watchdog (struct eth_device *dev)
{
	struct ax88796c_private *ax_local = 
		(struct ax88796c_private *)dev->priv;
	unsigned short phy_status;

	phy_status = get_reg16 (dev, P0_PSCR);

	if (!(phy_status & PSCR_PHYCOFF)) {
	/* The ethernet cable has been plugged */

		if (ax_local->w_state == chk_cable) {
			ax_local->w_state = chk_link;
			ax_local->w_ticks = 0;
		} else {
			if (++ax_local->w_ticks == AX88796C_WATCHDOG_RESTART) {
				ax88796c_mdio_write (dev, PHY_ID, PHY_BMCR,
						PHY_BMCR_100MB | 
						PHY_BMCR_AUTON | 
						PHY_BMCR_RST_NEG);
				ax_local->w_ticks = 0;
			}
		}
	} else {
		ax_local->w_state = chk_cable;
	}
}

static int ax88796c_recv (struct eth_device *dev)
{
	struct ax88796c_private *ax_local = 
		(struct ax88796c_private *)dev->priv;
	unsigned short interrupts;

	interrupts = get_reg16 (dev, P0_ISR);

	/* Acknowledge all interrupts */
	put_reg16 (dev, P0_ISR, interrupts);

	if (interrupts & ISR_LINK) {
		ax88796c_check_media (dev);
	}

	if (interrupts & ISR_RXPCT) {
		/*packet received */
		nic_to_pc (dev);
		put_reg16 (dev, P0_ISR, ISR_RXPCT);
	}

	if (!ax_local->link && (get_timer(0) >= ax_local->timeout)) {
		ax88796c_watchdog (dev);
		ax_local->timeout = get_timer(0) + CONFIG_SYS_HZ;
	}

	return 0;
}

static void ax88796c_handle_tx_hdr (struct tx_header * txhdr, 
		unsigned short len, unsigned short seq_num)
{
	unsigned short len_bar = (~len & TX_HDR_SOP_PKTLENBAR);

	/*SOP Header*/
	txhdr->sop.flags_pktlen = len;
	txhdr->sop.seqnum_pktlenbar =  TX_HDR_SEQNUM (seq_num) | len_bar;

	/*Segment Header*/
	txhdr->seg.flags_seqnum_seglen = TX_HDR_SEG_FS | TX_HDR_SEG_LS | len;
	txhdr->seg.eo_so_seglenbar = len_bar;

	/*EOP Header*/
	txhdr->eop.seqnum_pktlen = TX_HDR_SEQNUM (seq_num) | len;
	txhdr->eop.seqnumbar_pktlenbar = TX_HDR_SEQNUM(~seq_num) | len_bar;

}

static int ax88796c_send (struct eth_device *dev, volatile void *packet, 
		int length)
{
	unsigned int i, word_len;
	unsigned long time_out;
	struct tx_header hdr;
	struct ax88796c_private *ax_local = 
		(struct ax88796c_private *)dev->priv;

	time_out = get_timer(0) + (CONFIG_SYS_HZ / 100);
	while ((get_reg16 (dev, P0_TSNR) & TXNR_TXB_IDLE) == 0) {
		if (get_timer(0) > time_out) {
			goto error_out;
		}
	}
	ax88796c_handle_tx_hdr (&hdr, length, ax_local->seq_num);
	
	put_reg16 (dev, P0_TSNR,  TSNR_TXB_START | TSNR_PKT_CNT(1));
	/*Send SOP, Segment Header*/
	put_reg16 (dev, P0_RTDPR, hdr.sop.flags_pktlen);
	put_reg16 (dev, P0_RTDPR, hdr.sop.seqnum_pktlenbar);
	put_reg16 (dev, P0_RTDPR, hdr.seg.flags_seqnum_seglen);
	put_reg16 (dev, P0_RTDPR, hdr.seg.eo_so_seglenbar);
	/*Send Data*/
	word_len = ((length + 3) & 0xFFFC) / 2;
	for (i = 0; i < word_len; i++) {
		put_reg16 (dev, P0_RTDPR + DATA_PORT_ADDR,  
		*((unsigned short *) (packet + i * 2)));
 	}
	/*Send EOP Header*/
	put_reg16 (dev, P0_RTDPR, hdr.eop.seqnum_pktlen);
	put_reg16 (dev, P0_RTDPR, hdr.eop.seqnumbar_pktlenbar);

	time_out = get_timer(0) + (CONFIG_SYS_HZ / 100);
	while ((get_reg16 (dev, P0_TSNR) & TXNR_TXB_IDLE) == 0) {
		if (get_timer(0) > time_out) {
			goto error_out;
		}
	}
	if(get_reg16 (dev, P0_ISR) & ISR_TXERR) {
		goto error_out;
	}
	ax_local->seq_num++;
	return 0;
error_out:
	put_reg16 (dev, P0_TSNR, TSNR_TXB_REINIT);
	ax_local->seq_num = 0;
	return -EBUSY;
}

/* Set MAC address in chip */
static int ax88796c_write_hwaddr(struct eth_device *dev)
{
	ax88796c_set_enetaddr(dev);

	return 0;
}

/*
* =========================================================
* <<<<<<           Exported SubProgram Bodies         >>>>>>
* =========================================================
*/
int ax88796c_initialize (bd_t *bis)
{
	struct eth_device *dev;
	struct ax88796c_private *ax_local;

	dev = (struct eth_device *)malloc (sizeof *dev);
	if (NULL == dev)
		return -EFAULT;

	ax_local = (struct ax88796c_private *)malloc (sizeof *ax_local);
	if (NULL == ax_local)
		return -EFAULT;

	memset (dev, 0, sizeof *dev);
	memset (ax_local, 0, sizeof *ax_local);
	
	sprintf (dev->name, "ax88796c");
	dev->iobase = AX88796C_BASE;
	dev->priv = ax_local;
	dev->init = ax88796c_init;
	dev->halt = ax88796c_halt;
	dev->send = ax88796c_send;
	dev->recv = ax88796c_recv;
	dev->write_hwaddr = ax88796c_write_hwaddr;

	if (!ax88796c_reset (dev)) {

		unsigned short reg = get_reg16 (dev, P0_PSR);

#ifndef AXFLG_NO_EEPROM
		ax88796c_get_enetaddr (dev);
#endif

		/*Setup LED mode*/
		put_reg16 (dev, P0_PSR, (reg & PSR_PAGE_MASK) | PAGE2);
		put_reg16 (dev, P2_LCR0, LCR_LED0_EN | LCR_LED0_DUPLEX | LCR_LED1_EN | 
			LCR_LED1_100MODE);
		put_reg16 (dev, P2_LCR1, (get_reg16 (dev, P2_LCR1) & LCR_LED2_MASK) | 
			LCR_LED2_EN | LCR_LED2_LINK);

		ax88796c_mdio_write (dev, PHY_ID, PHY_ANAR, MII_ADVERTISE);
		ax88796c_mdio_write (dev, PHY_ID, PHY_BMCR,
			PHY_BMCR_100MB | PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);

		eth_register (dev);
	}
	return 0;
}
