/*
 * (C) Copyright 2010
 * Ilko Iliev <iliev@ronetix.at>
 * Asen Dimov <dimov@ronetix.at>
 * Ronetix GmbH <www.ronetix.at>
 *
 * (C) Copyright 2007-2008
 * Stelian Pop <stelian.pop@leadtechdesign.com>
 * Lead Tech Design <www.leadtechdesign.com>
 *
 * Configuation settings for the PM9G45 board.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_ARM926EJS	1	/* This is an ARM926EJS Core */
#define CONFIG_PM9G45		1	/* It's an Ronetix PM9G45 */
#define CONFIG_AT91SAM9G45	1	/* It's an Atmel AT91SAM9G45 SoC */

/* ARM asynchronous clock */
#define CONFIG_SYS_AT91_MAIN_CLOCK	12000000 /* from 12 MHz crystal */
#define CONFIG_SYS_HZ			1000

#define CONFIG_ARCH_CPU_INIT

#define CONFIG_CMDLINE_TAG	1	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS 1
#define CONFIG_INITRD_TAG	1

#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_SKIP_RELOCATE_UBOOT

/*
 * Hardware drivers
 */
#define CONFIG_AT91_GPIO	1
#define CONFIG_ATMEL_USART	1
#define CONFIG_USART3		1	/* USART 3 is DBGU */

/*
 * 1-wire
 */
#define CONFIG_DS2401
#define CONFIG_DS2401_PIN	AT91_PIO_PORTA, 31

#define CONFIG_SYS_USE_NANDFLASH	1

/* LED */
#define CONFIG_AT91_LED
#define	CONFIG_RED_LED		AT91_PIO_PORTD, 31 /* this is the user1 led */
#define	CONFIG_GREEN_LED	AT91_PIO_PORTD, 0 /* this is the user2 led */

#define CONFIG_BOOTDELAY	3

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE	1
#define CONFIG_BOOTP_BOOTPATH		1
#define CONFIG_BOOTP_GATEWAY		1
#define CONFIG_BOOTP_HOSTNAME		1

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_IMLS

#define CONFIG_CMD_PING		1
#define CONFIG_CMD_DHCP		1
#define CONFIG_CMD_NAND		1
#define CONFIG_CMD_USB		1

#define CONFIG_CMD_JFFS2		1
#define CONFIG_JFFS2_CMDLINE		1
#define CONFIG_JFFS2_NAND		1
#define CONFIG_JFFS2_DEV		"nand0" /* NAND dev jffs2 lives on */
#define CONFIG_JFFS2_PART_OFFSET	0	/* start of jffs2 partition */
#define CONFIG_JFFS2_PART_SIZE		(256 * 1024 * 1024) /* partition */

/* SDRAM */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			0x70000000
#define PHYS_SDRAM_SIZE			0x08000000	/* 128 megs */
#define CONFIG_MAC_OUI	"02:00:00" /* Organizationally Unique Identifier*/

/* NOR flash, not available */
#define CONFIG_SYS_NO_FLASH		1
#undef CONFIG_CMD_FLASH

/* DataFlash */
#define CONFIG_ATMEL_SPI
#define CONFIG_ATMEL_DATAFLASH_SPI
#define CONFIG_HAS_DATAFLASH
#define CONFIG_SYS_SPI_WRITE_TOUT		(5 * CONFIG_SYS_HZ)
#define CONFIG_SYS_DATAFLASH_LOGIC_ADDR_CS0	0xC0000000	/* CS0 */
#define AT91_SPI_CLK				15000000
#define DATAFLASH_TCSS				(0x1a << 16)
#define DATAFLASH_TCHS				(0x1 << 24)

/* SPI Flash */
#define CONFIG_ATMEL_SPI
#define CONFIG_CMD_SF
#define CONFIG_CMD_SPI
#define CONFIG_SPI_FLASH		1
#define CONFIG_SPI_FLASH_ATMEL		1
#define CONFIG_SYS_MAX_DATAFLASH_BANKS	1

/* NAND flash */
#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_MAX_CHIPS		1
#define CONFIG_NAND_ATMEL
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x40000000
#define CONFIG_SYS_NAND_DBW_8		1
/* our ALE is AD21 */
#define CONFIG_SYS_NAND_MASK_ALE	(1 << 21)
/* our CLE is AD22 */
#define CONFIG_SYS_NAND_MASK_CLE	(1 << 22)
#define CONFIG_SYS_NAND_ENABLE_PIN	AT91_PIO_PORTC, 14
#define CONFIG_SYS_NAND_READY_PIN	AT91_PIO_PORTD, 3

#endif

/* Ethernet */
#define CONFIG_MACB			1
#define CONFIG_RMII			1
#define CONFIG_NET_MULTI		1
#define CONFIG_NET_RETRY_COUNT		20
#if defined(CONFIG_BB9263)
#define CONFIG_RESET_PHY_R
#endif
#define CONFIG_MACB_SEARCH_PHY
#define CONFIG_CMD_MII

/* USB */
#define CONFIG_USB_ATMEL
#define CONFIG_USB_OHCI_NEW		1
#define CONFIG_DOS_PARTITION		1
#define CONFIG_SYS_USB_OHCI_CPU_INIT	1
#define CONFIG_SYS_USB_OHCI_REGS_BASE	0x00700000 /* _UHP_OHCI_BASE */
#define CONFIG_SYS_USB_OHCI_SLOT_NAME	"at91sam9g45"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	2
#define CONFIG_USB_STORAGE		1

/* LCD */
#define CONFIG_LCD			1
#define LCD_BPP				LCD_COLOR8
#undef LCD_TEST_PATTERN
#define CONFIG_SYS_WHITE_ON_BLACK	1
#define CONFIG_ATMEL_LCD		1
#if defined(CONFIG_BB9G45_v1_0)
#define CONFIG_ATMEL_LCD_BGR565		1
#else
#define CONFIG_ATMEL_LCD_RGB565		1
#endif
#if defined(CONFIG_ANDROID)
#undef CONFIG_LCD_LOGO
#undef CONFIG_LCD_INFO
#else
#define CONFIG_LCD_INFO
#define CONFIG_LCD_LOGO
#define CONFIG_LCD_INFO_BELOW_LOGO
#endif /* end else CONDFIG_ANDOIRD*/
#define CONFIG_SYS_CONSOLE_IS_IN_ENV	1

/* board specific(not enough SRAM) */
#define CONFIG_AT91SAM9G45_LCD_BASE	PHYS_SDRAM + 0xE00000

#define CONFIG_SYS_LOAD_ADDR		PHYS_SDRAM + 0x2000000 /* load addr */

#define CONFIG_SYS_MEMTEST_START	PHYS_SDRAM
#define CONFIG_SYS_MEMTEST_END		CONFIG_AT91SAM9G45_LCD_BASE

/* bootstrap + u-boot + env + linux in nandflash */
#define CONFIG_OVERWRITE_ETHADDR_ONCE
	/* Organizationally Unique Identifier + 3 octets "random" numbers */
#define CONFIG_ETHADDR		"02:00:00:de:ad:01"
#define CONFIG_ENV_IS_IN_NAND		1
#define CONFIG_ENV_OFFSET		0x60000
#define CONFIG_ENV_OFFSET_REDUND	0x80000
#define CONFIG_ENV_SIZE			0x20000		/* 1 sector = 128 kB */
#define CONFIG_BOOTCOMMAND	"nand read 0x72000000 0x200000 0x200000; bootm"
#if defined(CONFIG_BB9G45) && !defined(CONFIG_ANDROID)
#define BOOTARGS_CONSOLE	"fbcon=rotate:0 console=tty0 " \
				"console=ttyS0,115200 "
#elif defined(CONFIG_BB9263)
#define BOOTARGS_CONSOLE	"fbcon=rotate:3 console=tty0 " \
				"console=ttyS0,115200 "
#endif
#if defined(CONFIG_ANDROID)
#define BOOTARGS_CONSOLE	"console=ttyS0,115200 "
#define BOOTARGS_NAND		"root=/dev/mtdblock1 " \
				"mtdparts=atmel_nand:5m(Bootstrap)ro,95m" \
				"(ramdisk),64m(userdata),60m(cache),-(test) " \
				"rw rootfstype=jffs2 init=/init"
#else
#define BOOTARGS_NAND		"root=/dev/mtdblock4 " \
				"mtdparts=atmel_nand:128k(bootstrap)ro," \
				"256k(uboot)ro,1664k(env)," \
				"2M(linux)ro,-(root) rw " \
				"rootfstype=jffs2"
#endif
#define CONFIG_BOOTARGS		BOOTARGS_CONSOLE BOOTARGS_NAND
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{115200 , 19200, 38400, 57600, 9600 }

#define CONFIG_SYS_PROMPT		"U-Boot> "
#define CONFIG_SYS_CBSIZE		256
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_LONGHELP		1
#define CONFIG_CMDLINE_EDITING		1
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		ROUND(3 * CONFIG_ENV_SIZE + 128*1024,\
					0x1000)
#define CONFIG_SYS_GBL_DATA_SIZE	128 /* 128 bytes for initial data */

#define CONFIG_STACKSIZE		(32*1024)	/* regular stack */

#ifdef CONFIG_USE_IRQ
#error CONFIG_USE_IRQ not supported
#endif

#define CONFIG_EXTRA_ENV_SETTINGS       \
        "__prepared=by Alex A. Mihaylov AKA MinimumLaw, 2011\0" \
        "__produced=by NTC of Schemotecnics NTK PIT\0" \
        "__requsted=by OAO Radioavionica, Saint-Petersburg, Russia\0" \
        "serial#=1234567\0" \
        "mtdids=nand0=atmel_nand\0" \
        "mtdparts="MTDPARTS_DEFAULT"\0" \
        "IPADDR=192.168.5.101\0" \
        "NETMSK=255.255.255.0\0" \
        "SERVER=192.168.5.222\0" \
        "GATEWAY=192.168.5.254\0" \
        "HOSTNAME=ronetix\0" \
        "NFS_PATH=/ronetix\0" \
        "KRN_RAM=uImage\0" \
        "RAM_LD_ADDR=0x72000000\0" \
        "add_basic_args=setenv bootargs\0" \
        "add_ser_cons=setenv bootargs ${bootargs} console=ttyS0,115200\0" \
        "add_ip_conf=setenv bootargs ${bootargs} ip=${IPADDR}:${SERVER}:${GATEWAY}:${NETMASK}:${HOSTNAME}:eth0:off\0" \
        "add_mtd_dev=setenv bootargs ${bootargs} " CONFIG_UBI_MTD " ${mtdparts}\0" \
        "add_mtd_root=setenv bootargs ${bootargs} root=" CONFIG_ROOT_DEVICE " ro rootfstype=" CONFIG_ROOT_FSTYPE"\0" \
        "add_sd_root=setenv bootargs ${bootargs} root=/dev/mmcblk0p1 ro rootdelay=2\0" \
        "add_cf_root=setenv bootargs ${bootargs} root=/dev/sda1 ro rootdelay=2\0" \
        "add_usb_root=setenv bootargs ${bootargs} root=/dev/sdb1 ro rootdelay=2\0" \
        "add_nfs_root=run add_ip_conf; setenv bootargs ${bootargs} root=/dev/nfs rw nfsroot=${SERVER}:${NFS_PATH}\0" \
        "boot_from_nand=ubi part ubi; ubifsmount boot; ubifsload ${RAM_LD_ADDR} ${KRN_RAM}; bootm ${RAM_LD_ADDR}\0" \
        "ldr_net=setenv ipaddr ${IPADDR}; setenv serverip ${SERVER}; setenv gatewayip ${GATEWAY}; " \
                "setenv hostname ${HOSTNAME}; setenv netmask ${NETMASK}\0" \
        "make_mtd_args=run add_basic_args; run add_mtd_dev; run add_mtd_root\0" \
        "make_nfs_args=run add_basic_args; run add_mtd_dev; run add_nfs_root\0" \
        "make_usb_args=run add_basic_args; run add_mtd_dev; run add_usb_root\0" \
        "make_cf_args=run add_basic_args; run add_mtd_dev; run add_cf_root\0" \
        "make_sd_args=run add_basic_args; run add_mtd_dev; run add_sd_root\0" \
        "make_ubi_parts=nand erase ubi; ubi part ubi;" \
            "ubi create boot 1000000; ubi create firmware 800000;" \
            "ubi create modules 4000000; ubi create rootfs\0" \
        "nfsram=run make_nfs_args; run add_ser_cons; run tftp_get_ram_kern; bootm ${RAM_LD_ADDR}\0" \
        "nfsrom=run make_nfs_args; run add_ser_cons; run boot_from_nand\0" \
        "mtdram=run make_mtd_args; run add_ser_cons; run tftp_get_ram_kern; bootm ${RAM_LD_ADDR}\0" \
        "resque=run make_mtd_args; run add_ser_cons; run boot_from_nand\0" \
        "spo=run make_cf_args; run boot_from_nand\0" \
        "tftp_get_ram_kern=run ldr_net; tftpboot ${RAM_LD_ADDR} ${SERVER}:${KRN_RAM}\0" 

#endif
