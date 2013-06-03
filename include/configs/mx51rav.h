/*
 * Copyright (C) 2007, Guennadi Liakhovetski <lg@denx.de>
 *
 * (C) Copyright 2009 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the MX51EVK Board
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

#ifndef __CONFIG_H
#define __CONFIG_H

/* For SPI CS - copy from asm/imx-common/gpio.h */
#define IMX_GPIO_NR(port, index)		((((port)-1)*32)+((index)&31))

 /* High Level Configuration Options */

#define CONFIG_MX51	/* in a mx51 */

#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_SYS_TEXT_BASE	0x97800000

#include <asm/arch/imx-regs.h>

#define CONFIG_CMDLINE_TAG			/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_SERIAL_TAG
#define CONFIG_REVISION_TAG

#define CONFIG_OF_LIBFDT

#define CONFIG_MACH_TYPE	MACH_TYPE_MX51_BABBAGE
/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(10 * 1024 * 1024)

#define CONFIG_BOARD_LATE_INIT

/*
 * Hardware drivers
 */
#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE	UART1_BASE
#define CONFIG_MXC_GPIO

/*
 * SPI Interface
  */
#define CONFIG_CMD_SPI
#define CONFIG_CMD_SF

#ifdef CONFIG_CMD_SPI
#define CONFIG_MXC_SPI

/* SPI FLASH */
#ifdef CONFIG_CMD_SF

#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_ATMEL
#define CONFIG_SPI_FLASH_ATMEL_FIX_NON_POWER2_SECTOR
#define CONFIG_SF_DEFAULT_BUS		0
#define CONFIG_SF_DEFAULT_CS		( 1 | (IMX_GPIO_NR(4,25)<<8))
#define CONFIG_SF_DEFAULT_MODE		(SPI_MODE_0)
#define CONFIG_SF_DEFAULT_SPEED		25000000

#define CONFIG_ENV_SPI_BUS		0
#define CONFIG_ENV_SPI_CS		( 1 | (IMX_GPIO_NR(4,25)<<8))
#define CONFIG_ENV_SPI_MODE		(SPI_MODE_0)
#define CONFIG_ENV_SPI_MAX_HZ		25000000
#define CONFIG_FSL_ENV_IN_SF
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_SYS_NO_FLASH

#else /* !CONFIG_CMD_SF */
#define CONFIG_ENV_IS_NOWHERE
#endif /* CONFIG_CMD_SF */

/* SPI PMIC */
#define CONFIG_POWER
#define CONFIG_POWER_SPI
#define CONFIG_POWER_FSL
#define CONFIG_FSL_PMIC_BUS		0
#define CONFIG_FSL_PMIC_CS		( 0 | (IMX_GPIO_NR(4,24)<<8))
#define CONFIG_FSL_PMIC_CLK		25000000
#define CONFIG_FSL_PMIC_MODE		(SPI_MODE_0 | SPI_CS_HIGH)
#define CONFIG_FSL_PMIC_BITLEN		32
#define CONFIG_RTC_MC13XXX
#endif /* CONFIG_CMD_SPI */

/*
 * NAND config
 * */
#define CONFIG_CMD_NAND
#define CONFIG_NAND_MXC
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		NFC_BASE_ADDR_AXI
#define CONFIG_MXC_NAND_REGS_BASE	NFC_BASE_ADDR_AXI
#define CONFIG_MXC_NAND_IP_REGS_BASE	NFC_BASE_ADDR
#define CONFIG_SYS_NAND_LARGEPAGE
#define CONFIG_MXC_NAND_HWECC
#define CONFIG_SYS_NAND_USE_FLASH_BBT

#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define MTDIDS_DEFAULT			"nand0=mxc_nand-0"
#define MTDPARTS_DEFAULT		"mtdparts=mxc_nand-0:" \
					"50M(resque),"\
					"450M(restore)," \
					"-(reserved)"

#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS

#define CONFIG_UBI_MTD			"ubi.mtd=1"
#define CONFIG_ROOT_DEVICE		"ubi0:rootfs"
#define CONFIG_ROOT_FSTYPE		"ubifs"


/*
 * MMC Configs
 * */
#define CONFIG_FSL_ESDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR	0
#define CONFIG_SYS_FSL_ESDHC_NUM	1

#define CONFIG_MMC

#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT2
#define CONFIG_DOS_PARTITION

/*
 * Eth Configs
 */
#define CONFIG_MII

#define CONFIG_FEC_MXC
#define IMX_FEC_BASE	FEC_BASE_ADDR
#define CONFIG_FEC_MXC_PHYADDR	0x1F

#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET

/* USB Configs */
#define CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX5
#define CONFIG_USB_STORAGE
#define CONFIG_MXC_USB_PORT	1
#define CONFIG_MXC_USB_PORTSC	PORT_PTS_ULPI
#define CONFIG_MXC_USB_FLAGS	MXC_EHCI_POWER_PINS_ENABLED

/* Framebuffer and LCD */
#define CONFIG_PREBOOT
#define CONFIG_VIDEO
#define CONFIG_VIDEO_IPUV3
#define CONFIG_CFB_CONSOLE
#define CONFIG_VGA_AS_SINGLE_DEVICE
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
#define CONFIG_VIDEO_BMP_RLE8
#define CONFIG_SPLASH_SCREEN
#define CONFIG_BMP_16BPP
#define CONFIG_VIDEO_LOGO
#define CONFIG_IPUV3_CLK	133000000

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200

/***********************************************************
 * Command definition
 ***********************************************************/

#include <config_cmd_default.h>
#define CONFIG_CMD_BOOTZ
#undef CONFIG_CMD_IMLS

#define CONFIG_CMD_DATE

#define	CONFIG_TIMESTAMP
#define	CONFIG_BOOTDELAY		5	/* autoboot delay */
#define CONFIG_AUTOBOOT_KEYED		1
#define CONFIG_AUTOBOOT_PROMPT  \
        "Enter boot password in %d seconds to stop autoboot process\n", bootdelay
#undef CONFIG_AUTOBOOT_DELAY_STR
#define CONFIG_AUTOBOOT_STOP_STR "ravion"

#define	CONFIG_ETHPRIME		"FEC0"
#define	CONFIG_ETHADDR		00:11:22:33:44:55

#define CONFIG_LOADADDR		0x90800000	/* loadaddr env var */

#define CONFIG_EXTRA_ENV_SETTINGS \
	"__prepared=by Alex A. Mihaylov AKA MinimumLaw, 2013\0" \
	"__produced=by NTC of Schemotecnics NTK PIT\0" \
	"__requsted=by OAO Radioavionica, Saint-Petersburg, Russia\0" \
	"IPADDR=192.168.5.101\0" \
	"NETMASK=255.255.255.0\0" \
	"GATEWAYIP=192.168.5.222\0" \
	"SERVERIP=192.168.5.222\0" \
	"NFS_PATH=/cimc/exportfs/mx5test\0" \
	"mtdids="MTDIDS_DEFAULT"\0" \
	"mtdparts="MTDPARTS_DEFAULT"\0" \
	"script_name=bscript.img \0" \
	"script_addr=0x98760000 \0" \
	"blockboot=" \
	"if fatload ${bdev} ${binst}:${bpart} ${script_addr} ${script_name}; then " \
	    "source ${script_addr}; " \
	"else " \
	    "echo No found ${script_name} on FAT partition ${bdev}${binst}:${bpart}; " \
	"fi; " \
	"if ext2load ${bdev} ${binst}:${bpart} ${script_addr} ${script_name}; then " \
	    "source ${script_addr};" \
	"else " \
	    "echo No found $script_name on EXT2 partition ${bdev}${binst}:${bpart}; " \
	"fi; \0" \
	"mmcboot=" \
	"setenv bdev mmc; setenv binst 0; " \
	"setenv bpart 2; run blockboot; " \
	"setenv bpart 1; run blockboot\0" \
	"usbboot=usb start; " \
	"if usb storage; then " \
	    "echo Found USB storage device(s); " \
	    "if usb dev 0; then " \
		"echo Try USB device 0; " \
		"setenv bdev usb; setenv binst 0; " \
		"setenv bpart 2; run blockboot; " \
		"setenv bpart 1; run blockboot; " \
	    "else " \
		"echo Error setup USB device 0; " \
	    "fi; " \
	    "if usb dev 1; then " \
		"echo Try USB device 1; " \
		"setenv bdev usb; setenv binst 1; " \
		"setenv bpart 2; run blockboot; " \
		"setenv bpart 1; run blockboot; " \
	    "else " \
		"echo Error setup USB device 1; " \
	    "fi; " \
	    "if usb dev 2; then " \
		"echo Try USB device 2; " \
		"setenv bdev usb; setenv binst 2; " \
		"setenv bpart 2; run blockboot; " \
		"setenv bpart 1; run blockboot; " \
	    "else " \
		"echo Error setup USB device 2; " \
	    "fi; " \
	"else " \
	    "echo No USB storage device(s) found; " \
	"fi; \0" \
	"ubiboot=" \
	"if ubi part ${ubipart}; then " \
	    "echo Assign UBI partition ${ubipart}; " \
	    "if ubifsmount ${ubifs}; then " \
		"echo Mount UBI filesystem ${ubifs}; " \
		"if ubifsload ${script_addr} ${script_name}; then " \
		    "source ${script_addr}; " \
		"else " \
		    "echo Error load ${script_name} from filesystem ${ubifs} on " \
		    "UBI partition ${ubipart}; " \
		"fi; " \
	    "else " \
		"echo Error mount UBI filesystem ${ubifs}; " \
	    "fi; " \
	"else " \
	    "echo Error mount UBI partition ${ubipart}; " \
	"fi; \0" \
	"restboot=setenv ubipart restore; setenv ubifs ubi0:rootfs; run ubiboot\0" \
	"resqboot=setenv ubipart resque; setenv ubifs ubi0:rootfs; run ubiboot\0" \
	"prepnet=run prepenv; " \
		"setenv ipaddr ${IPADDR}; " \
		"setenv netmask ${NETMASK}; " \
		"setenv gatewayip ${GATEWAYIP}; " \
		"setenv serverip ${SERVERIP}\0" \
	"boottftp=tftpboot ${script_addr} ${serverip}:${script_name}; " \
		"source ${script_addr}\0" \
	"netboot=run prepnet; run boottftp\0" \
	"bootnfs=nfs ${script_addr} ${serverip}:${NFS_PATH}/${script_name}; " \
		"source ${script_addr}\0" \
	"nfsboot=run prepnet; run bootnfs\0" \
	"envclean=setenv bootargs\0" \
	"prepmtd=setenv bootargs \"${bootargs} " \
	"${mtdparts};mxc_dataflash:768K(u-boot)ro,256K(u-boot-env)ro,-(reserved)\"\0" \
	"prepp54sn=if setenv p54serial# ${p54serial#}; then " \
		"setenv bootargs \"${bootargs} p54spi_sn=${p54serial#}\"; fi;\0" \
	"prepcon=setenv bootargs \"${bootargs} console=ttymxc0,115200,8n1\"\0" \
	"prepenv=run envclean; run prepmtd; run prepp54sn\0" \
	"ravboot=run prepenv; "  \
		"run usbboot; "  \
		"run mmcboot; "  \
		"run restboot; " \
		"run nfsboot; "  \
		"run netboot; "  \
		"run resqboot\0" \

#define CONFIG_BOOTCOMMAND \
	"run ravboot"

#define CONFIG_ARP_TIMEOUT	200UL

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser */
#define CONFIG_SYS_PROMPT		"MX51RAV U-Boot > "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS	16	/* max number of command args */
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE /* Boot Argument Buffer Size */

#define CONFIG_SYS_MEMTEST_START       0x90000000
#define CONFIG_SYS_MEMTEST_END         0x90010000

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR

#define CONFIG_SYS_HZ		1000
#define CONFIG_CMDLINE_EDITING

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		CSD0_BASE_ADDR
#define PHYS_SDRAM_1_SIZE	(512 * 1024 * 1024)

#define CONFIG_SYS_SDRAM_BASE		(PHYS_SDRAM_1)
#define CONFIG_SYS_INIT_RAM_ADDR	(IRAM_BASE_ADDR)
#define CONFIG_SYS_INIT_RAM_SIZE	(IRAM_SIZE)

#define CONFIG_BOARD_EARLY_INIT_F

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#define CONFIG_SYS_DDR_CLKSEL	0
#define CONFIG_SYS_CLKTL_CBCDR	0x59E35100
#define CONFIG_SYS_MAIN_PWR_ON

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */ 
#define CONFIG_ENV_OFFSET      (768 * 1024)
#define CONFIG_ENV_SECT_SIZE   (256 * 1024)
#define CONFIG_ENV_SIZE        (256 * 1024)

#if 0  /* CONFIG_SYS_NO_FLASH */
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV 0
#endif /* CONFIG_SYS_NO_FLASH */

#endif
