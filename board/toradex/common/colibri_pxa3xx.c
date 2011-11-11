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

#include <common.h>		/* do not change order of include file */
#include <asm/arch/pxa-regs.h>
#include <command.h>		/* needs to be after common.h */
#include <environment.h>
#include <i2c.h>
#include <malloc.h>
#include <nand.h>
#include <net.h>		/* for eth_initialize_mac() */

DECLARE_GLOBAL_DATA_PTR;

#define		RH_A_PSM	(1 << 8)	/* power switching mode */
#define		RH_A_NPS	(1 << 9)	/* no power switching */

#ifdef CONFIG_NAND_SPL
#define printf(fmt, args...)
#endif /* CONFIG_NAND_SPL */

#define WITHIN_RANGE(min, arg, max) ( ((max) >= (arg)) && ((min) <= (arg)) )

extern int pxa3xx_nand_probe(struct nand_chip *chip);

/* ------------------------------------------------------------------------- */

/*
 * Miscelaneous platform dependent initialisations
 */

int board_init(void)
{
	/* memory and cpu-speed are setup before relocation */
	/* so we do _nothing_ here */

	/* arch number */
#ifndef CONFIG_LIMESTONE
#ifndef CONFIG_PXA320
	gd->bd->bi_arch_number = MACH_TYPE_COLIBRI300;
#else
	gd->bd->bi_arch_number = MACH_TYPE_COLIBRI320;
#endif
#else
	gd->bd->bi_arch_number = MACH_TYPE_LIMESTONE;
#endif

	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_DRAM_BASE + 0x100;

	return 0;
}

#ifndef CONFIG_NAND_SPL
int board_late_init(void)
{
	char *addr_str, *end;
	unsigned char bi_enetaddr[6]; /* Ethernet address */
	int i;
	u32 offset = 0x40008; /* config block Ethernet MAC address offset */
	size_t size = 6;
	unsigned char mac_addr[6]	= {0, 0, 0, 0, 0, 0};
	unsigned char mac_addr00[6]	= {0, 0, 0, 0, 0, 0};
	unsigned char mac_addrff[6]	= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	char env_str[64];

	setenv("stdout", "serial");
	setenv("stderr", "serial");

	/* Get MAC address from environment */
	if ((addr_str = getenv("ethaddr")) != NULL) {
		for (i = 0; i < 6; i++) {
			bi_enetaddr[i] = addr_str ? simple_strtoul(addr_str, &end, 16) : 0;
			if (addr_str) {
				addr_str = (*end) ? end + 1 : end;
			}
		}
	}

	/* read production parameter config block */
#ifdef CONFIG_NAND_PXA3xx
	nand_read(&nand_info[0], offset, &size, (u_char *) mac_addr);
#endif

	/* handle production parameter config block */
	if ((memcmp(mac_addr, mac_addr00, 6) == 0) ||
	    (memcmp(mac_addr, mac_addrff, 6) == 0)) {
		printf("Missing Colibri config block\n");
	} else {
		/* set Ethernet MAC address from config block if not already
		   set */
		if (memcmp(mac_addr00, bi_enetaddr, 6) == 0) {
			sprintf(env_str, "%02x:%02x:%02x:%02x:%02x:%02x",
				mac_addr[0], mac_addr[1], mac_addr[2],
				mac_addr[3], mac_addr[4], mac_addr[5]);
			setenv("ethaddr", env_str);
			saveenv();
		}
	}

	return 0;
}
#endif /* !CONFIG_NAND_SPL */

/*
 * Board-specific NAND initialization.
 */
int board_nand_init(struct nand_chip *chip)
{
	/* Setting up DFC GPIOs. */

	/* ND_IO<15:0> alternate function 1 */
	DF_IO0 = 0x00000001;
	DF_IO1 = 0x00000001;
	DF_IO2 = 0x00000001;
	DF_IO3 = 0x00000001;
	DF_IO4 = 0x00000001;
	DF_IO5 = 0x00000001;
	DF_IO6 = 0x00000001;
	DF_IO7 = 0x00000001;
	DF_IO8 = 0x00000001;
	DF_IO9 = 0x00000001;
	DF_IO10 = 0x00000001;
	DF_IO11 = 0x00000001;
	DF_IO12 = 0x00000001;
	DF_IO13 = 0x00000001;
	DF_IO14 = 0x00000001;
	DF_IO15 = 0x00000001;
	/* ND_nCS0 and ND_nCS1 alternate function 1 */
	DF_nCS0 = 0x00000001;
	DF_nCS1 = 0x00000001;
	/* ND_nWE and ND_nRE alternate function 1, data on pad sent during
	   D1-D3 low power mode, slow 10 mA drive */
	DF_nWE = 0x00001901;
	DF_nRE = 0x00001901;
	/* ND_CLE alternate function 0 */
	DF_CLE_nOE = 0x00001900;
	/* ND_ALE alternate function 1, data on pad sent during
	   D1-D3 low power mode, slow 10 mA drive */
#if defined(CONFIG_COLIBRI_PXA300V11) || defined(CONFIG_COLIBRI_PXA310V12)
	/* PXA300 and PXA310 */
	DF_ALE_nWE = 0x00001901;
#else /* CONFIG_COLIBRI_PXA300V11 || CONFIG_COLIBRI_PXA310V12 */
	/* PXA320 */
	DF_ALE_nWE1 = 0x00001901;
	DF_ALE_nWE2 = 0x00001901;
#endif /* CONFIG_COLIBRI_PXA300V11 || CONFIG_COLIBRI_PXA310V12 */
	/* ND_RnB alternate function 0, data on pad sent during
	   D1-D3 low power mode, slow 10 mA drive */
	DF_INT_RnB = 0x00001900;

#ifdef CONFIG_NAND_PXA3xx
	return pxa3xx_nand_probe(chip);
#else
	return 0;
#endif
}

#ifdef CONFIG_NAND_PXA3xx
#ifndef CONFIG_NAND_SPL
int do_nandupdate(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u_char *addr = 0;
	size_t count = 0;
	size_t len = 0;
	loff_t offset = 0;
	nand_erase_options_t opts;
	int ret = 0;
	char *s;
	size_t size = 0, size3 = 0;
	u_char *vaddr;
	u_char *verifybuf;

	/* pre-set addr and len */
	s = getenv("loadaddr");
	if (s != NULL)
		addr = (uchar *)simple_strtoul(s, NULL, 16);
	s = getenv("filesize");
	if (s != NULL)
		len = simple_strtoul(s, NULL, 16);

	/* parse arguments */
	switch (argc) {
	case 1:
		break;
	case 2:
		addr = (uchar *)simple_strtoul(argv[1], NULL, 16);
		break;
	case 3:
		addr = (uchar *)simple_strtoul(argv[1], NULL, 16);
		len = simple_strtoul(argv[2], NULL, 16);
		break;
	default:
		goto usage;
	}
	if (addr == 0) {
		printf("Invalid image address!\n");
		return 1;
	}

	if (!WITHIN_RANGE(CONFIG_SYS_NAND_U_BOOT_SIZE_MIN, len, CONFIG_SYS_NAND_U_BOOT_SIZE_MAX)) {
		printf("Wrong Image length! Yours is %dKb. "
		       "Should be within [%d - %d]Kb range\n",
		       len/1024, CONFIG_SYS_NAND_U_BOOT_SIZE_MIN/1024,
		       CONFIG_SYS_NAND_U_BOOT_SIZE_MAX/1024);
		return 1;
	}

	/* erase NAND area */
	memset(&opts, 0, sizeof(opts));
	opts.offset	= 0;
	opts.length	= 0x40000; /* erase 256Kb -- u-boot#1, bbt, u-boot#2 */
	opts.jffs2	= 0;
	opts.quiet	= 1;
	ret = nand_erase_opts(&nand_info[0], &opts);
	if (ret) {
		printf("Failed erasing NAND at offset 0x%08lx!\n", opts.offset);
		return 1;
	}

	memset(&opts, 0, sizeof(opts));
	opts.offset	= 0x60000;
	opts.length	= 0x20000; /* erase 128Kb -- u-boot#3 */
	opts.jffs2	= 0;
	opts.quiet	= 1;
	ret = nand_erase_opts(&nand_info[0], &opts);
	if (ret) {
		printf("Failed erasing NAND at offset 0x%08lx!\n", opts.offset);
		return 1;
	}

	/* write new u-boot image to NAND */
	/* u-boot#1 */
	offset = 0;
	size = 0x1f800;
	ret = nand_write(&nand_info[0], offset, &size, addr);
	if (ret) {
		printf("Failed writing NAND at offset 0x%08lx!\n", (long unsigned int)offset);
		return 1;
	}

	/* u-boot#2 */
	offset = 0x20000;
	size = len - 0x1f800;
	size = ((size + CONFIG_SYS_NAND_PAGE_SIZE - 1) / CONFIG_SYS_NAND_PAGE_SIZE) *
		CONFIG_SYS_NAND_PAGE_SIZE; /* align to 512b/2K page boundary */

	if (size > CONFIG_SYS_NAND_U_BOOT_SIZE2) {
		/* image is big, and will be copyied into u-boot#3 as well */
		size3 = size - CONFIG_SYS_NAND_U_BOOT_SIZE2;
		size = CONFIG_SYS_NAND_U_BOOT_SIZE2;
	}

	ret = nand_write(&nand_info[0], offset, &size, addr + 0x1f800);
	if (ret) {
		printf("Failed writing NAND at offset 0x%08lx!\n", (long unsigned int)offset);
		return 1;
	}

	if (size3) {
		/* copy to u-boot#3 */
		offset = 0x60000;
		ret = nand_write(&nand_info[0], offset, &size3,
				 addr + 0x1f800 + CONFIG_SYS_NAND_U_BOOT_SIZE2);
		if (ret) {
			printf("Failed writing NAND at offset 0x%08lx!\n",
			       (long unsigned int)offset);
		return 1;
		}
	}

	/* verify image in NAND */
	verifybuf = malloc(len);
	if (!verifybuf) {
		printf("No memory for verify buffer\n");
		return 1;
	}
	offset = 0;
	size = 0x1f800;
	ret = nand_read(&nand_info[0], offset, &size, verifybuf);
	if (ret) {
		printf("Failed reading NAND at offset 0x%08lx!\n", (long unsigned int)offset);
		if (verifybuf)
			free(verifybuf);
		return 1;
	}
	offset = 0x20000;
	if (!size3)
		size = len - 0x1f800;
	else
		size = CONFIG_SYS_NAND_U_BOOT_SIZE2;

	ret = nand_read(&nand_info[0], offset, &size, verifybuf + 0x1f800);
	if (ret) {
		printf("Failed reading NAND at offset 0x%08lx!\n", (long unsigned int)offset);
		if (verifybuf)
			free(verifybuf);
		return 1;
	}

	if (size3) {
		offset = 0x60000;
		ret = nand_read(&nand_info[0], offset, &size3,
				verifybuf + 0x1f800 + CONFIG_SYS_NAND_U_BOOT_SIZE2);
		if (ret) {
			printf("Failed reading NAND at offset 0x%08lx!\n",
			       (long unsigned int)offset);
		if (verifybuf)
			free(verifybuf);
		return 1;
		}
	}

	count = len;
	vaddr = verifybuf;
	while (count-- > 0) {
		u_char byte1 = *(u_char *)addr;
		u_char byte2 = *(u_char *)vaddr;
		if (byte1 != byte2) {
			printf("Failed image compare at 0x%08lx!\n", (long unsigned int)addr);
			if (verifybuf)
				free(verifybuf);
			return 1;
		}
		addr++;
		vaddr++;
	}
	printf("Successfully verified 0x%08lx bytes\n", (long unsigned int)len);
	if (verifybuf)
		free(verifybuf);

	return ret;

usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

U_BOOT_CMD(
	nupdate,    3,    0,    do_nandupdate,
	"nupdate - update U-Boot image in NAND from memory\n",
	"[addr] [len]\n"
	"    - updates U-Boot image in NAND from an image previously loaded"
	" into memory\n"
);
#endif /* !CONFIG_NAND_SPL */
#endif /* CONFIG_NAND_PXA3xx */

int dram_init(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return 0;
}

#ifdef CONFIG_USB_OHCI_NEW
#ifndef CONFIG_NAND_SPL
int usb_board_init(void)
{
#ifdef CONFIG_LIMESTONE
	/* Activate PowerEn GPIO3_2 (High active) */
	GPDR0 |= (1 << 3);
	GPSR0 |= (1 << 3);
	/* Check OverCurrent GPIO2_2 (Low active) */
	if (!(GPLR0 & (1 << 3))) {
		GPCR0 |= (1 << 3);	/* Shut power off immediately */
		puts("USB over current!\n");
	}
#endif /* CONFIG_LIMESTONE */

	/* Set the Power Control Polarity Low and Power Sense
	   Polarity Low to active low. */
	UHCHR = (UHCHR | UHCHR_PCPL | UHCHR_PSPL | UHCHR_SSEP2) &
		~(UHCHR_SSEP0 | UHCHR_SSEP1 | UHCHR_SSE);

	UP2OCR = UP2OCR_HXOE | UP2OCR_HXS;	/* use port 2 as host */
	UP2OCR |= UP2OCR_DPPDE | UP2OCR_DMPDE;	/* enable D +/- pull-down */

	UHCHR |= UHCHR_FSBIR;

	while (UHCHR & UHCHR_FSBIR);

	UHCHIE = (UHCHIE_UPRIE | UHCHIE_RWIE);

	UHCRHDA &= ~(RH_A_NPS);
	UHCRHDA |= RH_A_PSM;

	/* Set port power control mask bits, only 3 ports. */
	UHCRHDB |= (0x7<<17);

	return 0;
}

int usb_board_init_fail(void)
{
	return 0;
}

int usb_board_stop(void)
{
	UHCHR |= UHCHR_FHR;
	udelay(11);
	UHCHR &= ~UHCHR_FHR;

	UHCCOMS |= 1;
	udelay(10);

	CKENA &= ~(CKENA_2_USBHOST | CKENA_20_UDC);

#ifdef CONFIG_LIMESTONE
	/* Deactivate PowerEn GPIO3_2 (High active) */
	GPCR0 |= (1 << 3);
	GPDR0 &= ~(1 << 3);
#endif /* CONFIG_LIMESTONE */

	puts("Called USB STOP\n");

	return 0;
}
#endif /* !CONFIG_NAND_SPL */
#endif /* CONFIG_USB_OHCI_NEW */

#ifndef CONFIG_NAND_SPL

#ifdef CONFIG_SERIAL_TAG
/*****************************************************************************
Board serial number used to calculate ethernet MAC addres and used directly
in Linux Kernel. MAC seen like:
 00:14:2D:XX:YY;ZZ
 \______/ \______/
    \        \_______ Serial number from board label in HEX format
     \_______________ Toradex MAC series
And stored in Serial boot tag like:
serialnr->high = 0x0000ZZYY
serialnr->low  = 0xXX2D1400
If no serial# enverooment variable present, use only Toradex part of MAC.
*****************************************************************************/
void get_board_serial(struct tag_serialnr *serialnr)
{
        char *serial = getenv("serial#");
        if (serial) {
            unsigned long tmp;

            tmp = simple_strtoul(serial,NULL,10);

            serialnr->low  = 0x002D1400 | ( ( tmp & 0xFF0000) << 8 );
            serialnr->high = ( ( tmp & 0xFF ) << 8 ) | ( ( tmp & 0xFF00 ) >> 8 );
        } else {
            serialnr->high = 0x00000000;
            serialnr->low  = 0x002D1400;
        }
}
#endif /* CONFIG_SERIAL_TAG */

#ifdef CONFIG_REVISION_TAG
/*****************************************************************************
Board revision number used by Toradex kernel patch
If no rev# enverooment variable present, use revision 0.0
*****************************************************************************/
unsigned int get_board_rev(void)
{
        char *revision = getenv("rev#");
        if (revision) {
            unsigned int tmp;

            tmp = (unsigned int)simple_strtoul(revision,NULL,16);
            return tmp;
        } else
            return 0;
}
#endif /* CONFIG_REVISION_TAG */

#endif /* !CONFIG_NAND_SPL */
