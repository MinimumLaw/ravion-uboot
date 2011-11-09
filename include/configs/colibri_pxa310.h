/*
 *
 * Configuation settings for a Toradex Colibri XScale PXA310 module on a
 * Toradex Colibri Evaluation Board.
 *
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

/*
 * If updating from U-Boot 2008.10 or 2009.03 there exists a size restriction
 * on the NAND boot image, so you MUST use a minimal configuration (e.g.
 * without MMC and UBI support) ...
 */
#undef CONFIG_MINIMAL_UBOOT

/*
 * If we are developing, we might want to start das U-Boot from RAM
 * so we MUST NOT initialize critical regs like mem-timing ...
 */
#undef CONFIG_SKIP_SDRAM_INIT
#undef CONFIG_SKIP_RELOCATE_UBOOT

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_CPU_MONAHANS	1	/* Marvell Monahans CPU */
#define CONFIG_PXA310			/* XScale PXA310 */
#define CONFIG_COLIBRI_PXA310V12 1	/* Toradex Colibri XScale PXA310 */

#ifndef CONFIG_MINIMAL_UBOOT
#define CONFIG_MMC		1
#define CONFIG_PXA_MMC		1
#endif /* CONFIG_MINIMAL_UBOOT */
#define BOARD_LATE_INIT		1

#undef CONFIG_USE_IRQ			/* we don't need IRQ/FIQ stuff */

/* we will never enable dcache, because we have to setup MMU first */
#define CONFIG_SYS_NO_DCACHE

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(3*1024*1024)
#define CONFIG_SYS_GBL_DATA_SIZE	128	/* size in bytes reserved for
						   initial data */

/*
 * Ethernet driver
 */
#define CONFIG_NET_MULTI		1
#define CONFIG_NET_RETRY_COUNT		5
#define CONFIG_TFTP_TIMEOUT		250
#define CONFIG_DRIVER_AX88796B		1
#define AX88796B_BASE			0x10000000 /* CS2 */

/*
 * select serial console configuration
 */
#define CONFIG_PXA_SERIAL
#define CONFIG_FFUART		1

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/* allow silent environment variable */
#define CONFIG_SILENT_CONSOLE

#define CONFIG_BAUDRATE		115200

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_DIAG
#define CONFIG_CMD_ELF
#undef CONFIG_CMD_FLASH
#define CONFIG_CMD_FAT
#undef CONFIG_CMD_IMLS
#ifndef CONFIG_MINIMAL_UBOOT
#define CONFIG_CMD_MMC
#endif
#define CONFIG_CMD_NAND
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_SAVEENV
#ifndef CONFIG_MINIMAL_UBOOT
#define CONFIG_CMD_UBI
#endif
#define CONFIG_CMD_USB

/* USB */
#define CONFIG_USB_OHCI_NEW	1
#define CONFIG_USB_STORAGE	1
#define CONFIG_DOS_PARTITION	1	/* MMC as well */

#define CONFIG_SYS_USB_OHCI_BOARD_INIT	1
#define CONFIG_SYS_USB_OHCI_CPU_INIT	1
#define CONFIG_SYS_USB_OHCI_REGS_BASE	0x4C000000
#define CONFIG_SYS_USB_OHCI_SLOT_NAME	"colibri_pxa310"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS 2

#define CONFIG_BOOTDELAY	5
#define CONFIG_ETHADDR		00:00:00:00:00:00
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.10.2
#define CONFIG_SERVERIP		192.168.10.1

#define DEFAULT_BOOTCOMMAND					\
	"run ubiboot; run nfsboot"

#define MMC_BOOTCMD						\
	"echo Loading RAM disk and kernel from MMC/SD card...; "\
	"mmc init && "						\
	"fatload mmc 0:1 0x81800000 rootfs-ext2.img.gz && "	\
	"fatload mmc 0:1 0x81000000 uImage;"			\
	"run ramboot"

#define NFS_BOOTCMD						\
	"setenv bootargs ${defargs} ${nfsargs} ${mtdparts}; "	\
	"echo Booting from NFS...; "				\
	"dhcp; "						\
	"bootm"

#define RAM_BOOTCMD						\
	"setenv bootargs ${defargs} ${ramargs} ${mtdparts}; "	\
	"echo Booting from RAM...; "				\
	"bootm"

#define UBI_BOOTCMD						\
	"setenv bootargs ${defargs} ${ubiargs} ${mtdparts}; "	\
	"echo Booting from NAND...; "				\
	"ubi part kernel-ubi && ubi read 0x81000000 kernel; "	\
	"bootm"

#define USB_BOOTCMD						\
	"echo Loading RAM disk and kernel from USB stick...; "	\
	"usb start && "						\
	"fatload usb 0:1 0x81800000 rootfs-ext2.img.gz && "	\
	"fatload usb 0:1 0x81000000 uImage;"			\
	"run ramboot"

#define CONFIG_BOOTCOMMAND	DEFAULT_BOOTCOMMAND
#define CONFIG_NFSBOOTCOMMAND	NFS_BOOTCMD
#define CONFIG_RAMBOOTCOMMAND	RAM_BOOTCMD
#define CONFIG_EXTRA_ENV_SETTINGS \
	"altbootcmd=" CONFIG_SYS_ALT_BOOTCOMMAND "\0" \
	"bootlimit=" CONFIG_SYS_BOOTLIMIT "\0" \
	"defargs=console=ttyS0,115200n8 mem=128M\0"   \
	"flashboot=console=ttyS0,115200n8 ip=off mem=128M root=/dev/mtdblock5 rw rootfstype=yaffs2\0" \
	"mmcboot=" MMC_BOOTCMD "\0" \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	"nfsargs=ip=:::::eth0: root=/dev/nfs\0" \
	"ramargs=initrd=0xA1800000,32M ramdisk_size=32768 root=/dev/ram0 rw\0" \
	"ubiargs=ubi.mtd=6 root=ubi0:rootfs rootfstype=ubifs\0" \
	"ubiboot=" UBI_BOOTCMD "\0" \
	"usbboot=" USB_BOOTCMD "\0" \
	""
#define CONFIG_CMDLINE_TAG	1	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS 1
#define CONFIG_REVISION_TAG	1
#define CONFIG_SERIAL_TAG	1

#define CONFIG_BOOTCOUNT_LIMIT		/* support for bootcount limit */
#define CONFIG_SYS_BOOTCOUNT_ADDR	0x80000200	/* just after boot parameters
							   Note: internal SRAM not preserved! */

#define CONFIG_SYS_BOOTLIMIT		"3"
#define CONFIG_SYS_ALT_BOOTCOMMAND	"run mmcboot; run usbboot"

#define CONFIG_BZIP2
#define CONFIG_CRC32_VERIFY
#define CONFIG_TIMESTAMP

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400	/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif /* CONFIG_CMD_KGDB */

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING

#define CONFIG_SYS_LONGHELP		/* undef to save memory	*/
#ifdef CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT		"$ "	/* Monitor Command Prompt */
#else /* CONFIG_SYS_HUSH_PARSER */
#define CONFIG_SYS_PROMPT		"=> "	/* Monitor Command Prompt */
#endif /* CONFIG_SYS_HUSH_PARSER */
#define CONFIG_SYS_CBSIZE		512	/* Console I/O Buffer Size */
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE /* Boot Argument
							     Buffer Size */
#define CONFIG_SYS_DEVICE_NULLDEV	1

#define CONFIG_SYS_MEMTEST_START	0x80400000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x80800000	/* 4 ... 8 MB in DRAM */

/* default load address */
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_DRAM_BASE + 0x1000000)

#define CONFIG_SYS_HZ			1000
/* Monahans Core Frequency */
#define CONFIG_SYS_MONAHANS_RUN_MODE_OSC_RATIO		24 /* valid values: 8,
							      16, 24, 31 */
#define CONFIG_SYS_MONAHANS_TURBO_RUN_MODE_RATIO	2 /* valid values: 1,
							     2 */

					/* valid baudrates */
#define CONFIG_SYS_BAUDRATE_TABLE	{9600, 19200, 38400, 57600, 115200, \
					 230400}

#ifndef CONFIG_MINIMAL_UBOOT
#define CONFIG_SYS_MMC_BASE		0xF0000000 
#endif

/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif /* CONFIG_USE_IRQ */

/*
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1 /* we have 1 bank of DRAM */
#define PHYS_SDRAM_1		0x80000000	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	0x08000000	/* 128 MB */

#define CONFIG_SYS_DRAM_BASE		0x80000000	/* at nSDCS<0> */
#define CONFIG_SYS_DRAM_SIZE		0x08000000	/* 128 MB Ram */

#define CONFIG_SYS_SKIP_DRAM_SCRUB	/* to preserve boot count */

/* Dynamic MTD partition support */
#define CONFIG_CMD_MTDPARTS   /* Enable 'mtdparts' command line support */
#define CONFIG_MTD_PARTITIONS /* ??? */
#define CONFIG_MTD_DEVICE     /* needed for mtdparts commands */
#define MTDIDS_DEFAULT		"nand0=pxa3xx-nand"
#define MTDPARTS_DEFAULT	"mtdparts=pxa3xx-nand:"		\
				"128k(u-boot-1-part)ro,"		\
				/* "126k(u-boot-1-part)ro," */		\
				/* "2k@126k(bad-block-table)ro," */	\
				"128k(u-boot-2-part)ro,"		\
				"128k@256k(configuration-block)ro,"	\
				"128k(u-boot-3-part)ro,"		\
				"512k(u-boot-env)ro,"			\
				"3m(kernel-ubi),"			\
				"124m(rootfs-ubi),"			\
				"352m(mass-storage),"			\
				"-(replacement-blocks)ro"

/*
 * NAND Flash
 */
#define CONFIG_NAND_PXA3xx
#define CONFIG_NAND_PXA3xx_CLK	156000000UL
#define CONFIG_NEW_NAND_CODE
#define CONFIG_SYS_NAND_HW_ECC

#define CONFIG_SYS_NAND_BASE		0x0

#define CONFIG_SYS_MAX_NAND_DEVICE	1	/* Max number of NAND devices */

/* nand timeout values */
#define CONFIG_SYS_NAND_PROG_ERASE_TO	3000
#define CONFIG_SYS_NAND_OTHER_TO	100
#define CONFIG_SYS_NAND_SENDCMD_RETRY	3
#undef NAND_ALLOW_ERASE_ALL	/* Allow erasing bad blocks - don't use */

#undef CONFIG_MTD_DEBUG
#undef CONFIG_MTD_DEBUG_VERBOSE

#ifndef CONFIG_MINIMAL_UBOOT
#define CONFIG_SYS_USE_UBI
#define CONFIG_MTD_DEVICE
#define CONFIG_CMD_JFFS2
#define CONFIG_JFFS2_NAND
#define CONFIG_JFFS2_CMDLINE
#define CONFIG_RBTREE
#endif /* CONFIG_MINIMAL_UBOOT */

#define CONFIG_SYS_NAND_MAX_CHIPS	1

#define CONFIG_SYS_NO_FLASH	1	/* no NOR flash */

#define CONFIG_ENV_IS_IN_NAND	1	/* use NAND for environment vars */
#define CONFIG_ENV_OFFSET	0x80000
#define CONFIG_ENV_RANGE	0x80000
#define CONFIG_ENV_SIZE		CONFIG_SYS_NAND_BLOCK_SIZE

/*
 * IPL (Initial Program Loader, integrated inside CPU)
 * Will load first 16 kByte from NAND (SPL) into internal SRAM and execute it from
 * there.
 *
 * SPL (Secondary Program Loader)
 * Will load special U-Boot version (NUB) from NAND and execute it. This SPL
 * has to fit into 16 kByte. It sets up the CPU and configures the SDRAM
 * controller and the NAND controller so that the special U-Boot image can be
 * loaded from NAND to SDRAM.
 *
 * NUB (NAND U-Boot)
 * This NAND U-Boot (NUB) is a special U-Boot version which can be started
 * from RAM. Therefore it mustn't (re-)configure the SDRAM controller.
 */
#define CONFIG_SYS_NAND_U_BOOT_DST	0x87008000	/* Load NUB to this
							   addr */
/* Start NUB from this addr */
#define CONFIG_SYS_NAND_U_BOOT_START	CONFIG_SYS_NAND_U_BOOT_DST

/*
 * Define the partitioning of the NAND chip (only RAM U-Boot is needed here)
 */
#define CONFIG_SYS_NAND_U_BOOT_OFFS	(16 << 10) 	/* Offset to RAM U-Boot
							   image */
#define CONFIG_SYS_NAND_U_BOOT_SIZE	(110 << 10) /* Size of RAM U-Boot 1st
						       image part (exclude
						       16K SPL) and 2K bad
						       block table@126Kb */
#define CONFIG_SYS_NAND_U_BOOT_OFFS2	(128 << 10)	/* Offset to 2nd part */
#define CONFIG_SYS_NAND_U_BOOT_SIZE2	(128 << 10)	/* Size of 2nd part */
#define CONFIG_SYS_NAND_U_BOOT_OFFS3	(384 << 10)	/* Offset to 3rd part */
#define CONFIG_SYS_NAND_U_BOOT_SIZE3	(128 << 10)	/* Size of 3rd part */

/* min u-boot size in bytes */
/* should be of size of the first image part at least */
#define CONFIG_SYS_NAND_U_BOOT_SIZE_MIN CONFIG_SYS_NAND_U_BOOT_SIZE

/* max u-boot size in bytes */
#define CONFIG_SYS_NAND_U_BOOT_SIZE_MAX (CONFIG_SYS_NAND_U_BOOT_SIZE_MIN + CONFIG_SYS_NAND_U_BOOT_SIZE2 + CONFIG_SYS_NAND_U_BOOT_SIZE3)

/*
 * Now the NAND chip has to be defined (no autodetection used!)
 */
#define CONFIG_SYS_NAND_PAGE_SIZE	2048	/* NAND chip page size */
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 << 10) /* NAND chip block size */
#define CONFIG_SYS_NAND_PAGE_COUNT	64	/* NAND chip page count */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0 /* Location of bad block marker */
#define CONFIG_SYS_NAND_4_ADDR_CYCLE		/* fourth addr used (>32MB) */

#define CONFIG_SYS_NAND_ECCSIZE		256
#define CONFIG_SYS_NAND_ECCBYTES	3
#define CONFIG_SYS_NAND_ECCSTEPS	(CONFIG_SYS_NAND_PAGE_SIZE / \
					 CONFIG_SYS_NAND_ECCSIZE)
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_ECCTOTAL	(CONFIG_SYS_NAND_ECCBYTES * \
					 CONFIG_SYS_NAND_ECCSTEPS)
#define CONFIG_SYS_NAND_ECCPOS		{40, 41, 42, 43, 44, 45, 46, 47, 48, \
					 49, 50, 51, 52, 53, 54, 55, 56, 57, \
					 58, 59, 60, 61, 62, 63}

#define CONFIG_SYS_64BIT_VSPRINTF	/* needed for nand_util.c */

#endif	/* __CONFIG_H */
