/*
 *
 * Configuation settings for a Toradex Colibri XScale PXA320 module on a
 * Toradex Colibri Evaluation Board.
 *
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
#define CONFIG_PXA320			/* XScale PXA320 */
#define CONFIG_COLIBRI_PXA320V20 1	/* Toradex Colibri XScale PXA320 */

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
 * Ethernet drivers
 */
#define CONFIG_NET_MULTI		1
#define CONFIG_NET_RETRY_COUNT		5
#define CONFIG_TFTP_TIMEOUT		250
#if defined(CONFIG_COLIBRI_PXA320V10)
/* Colibri XScale PXA320 V1.0 uses DM9000E */
#define CONFIG_DM9000_BASE		0x10000000 /* CS2 */
#define CONFIG_DM9000_NO_SROM		1
#define CONFIG_DRIVER_DM9000		1
#define DM9000_DATA			(CONFIG_DM9000_BASE + 0x4)
#define DM9000_IO			CONFIG_DM9000_BASE
#elif !defined(CONFIG_COLIBRI_PXA320V20) /* CONFIG_COLIBRI_PXA320V10 */
/* Colibri XScale PXA320 V1.2b */
#define CONFIG_DRIVER_AX88796B		1
#define AX88796B_BASE			0x10000000 /* CS2 */
#else /* CONFIG_COLIBRI_PXA320V20 */
/* Colibri XScale PXA320 V2.0a */
#define CONFIG_DRIVER_AX88796C		1
#define CONFIG_16BIT_MODE		1 /* 16 bit */
#define CONFIG_AX88796B_PIN_COMPATIBLE	1
#define AX88796C_BASE			0x10000000 /* CS2 */
#define AXFLG_NO_EEPROM
#endif /* CONFIG_COLIBRI_PXA320V20 */

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
//#define CONFIG_CMD_EXT2
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
#define CONFIG_SYS_USB_OHCI_SLOT_NAME	"colibri_pxa320"
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
#define CONFIG_SYS_MONAHANS_RUN_MODE_OSC_RATIO		31 /* valid values: 8,
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
				"864m(mass-storage),"			\
				"-(replacement-blocks)ro"

/*
 * NAND Flash
 */
#define CONFIG_NAND_PXA3xx
#define CONFIG_NAND_PXA3xx_CLK  104000000UL
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

//#define CONFIG_ENV_IS_NOWHERE
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
#ifdef CONFIG_COLIBRI_PXA320V10
#define CONFIG_SYS_NAND_PAGE_SIZE	512 /* NAND chip page size */
#define CONFIG_SYS_NAND_BLOCK_SIZE	(16 << 10) /* NAND chip block size */
#define CONFIG_SYS_NAND_PAGE_COUNT	32 /* NAND chip page count */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0 /* Location of bad block marker */
#define CONFIG_SYS_NAND_4_ADDR_CYCLE	/* fourth addr used (>32MB) */

#define CONFIG_SYS_NAND_ECCSIZE	256
#define CONFIG_SYS_NAND_ECCBYTES	3
#define CONFIG_SYS_NAND_ECCSTEPS	(CONFIG_SYS_NAND_PAGE_SIZE / \
					 CONFIG_SYS_NAND_ECCSIZE)
#define CONFIG_SYS_NAND_OOBSIZE	16
#define CONFIG_SYS_NAND_ECCTOTAL	(CONFIG_SYS_NAND_ECCBYTES * \
					 CONFIG_SYS_NAND_ECCSTEPS)
#define CONFIG_SYS_NAND_ECCPOS		{8, 9, 10, 11, 12, 13}
#else /* CONFIG_COLIBRI_PXA320V10 */
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
#endif /* CONFIG_COLIBRI_PXA320V10 */

#define CONFIG_SYS_64BIT_VSPRINTF	/* needed for nand_util.c */

#define CONFIG_EXTRA_ENV_SETTINGS       \
        "__prepared=by Alex A. Mihaylov AKA MinimumLaw, 2011\0" \
        "__produced=by NTC of Schemotecnics NTK PIT\0" \
        "__requsted=by OAO Radioavionica, Saint-Petersburg, Russia\0" \
        "serial#=1234567\0" \
        "mtdids=nand0=pxa3xx-nand\0" \
        "mtdparts="MTDPARTS_DEFAULT"\0" \
        "IPADDR=192.168.5.101\0" \
        "NETMSK=255.255.255.0\0" \
        "SERVER=192.168.5.222\0" \
        "GATEWAY=192.168.5.254\0" \
        "HOSTNAME=colibri\0" \
        "NFS_PATH=/cimc/exportfs/colibri\0" \
        "KRN_RAM=uImage.pxa320\0" \
        "RAM_LD_ADDR=0xA0008000\0" \
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
        "ldr_net=setenv ipaddr ${IPADDR}; setenv serverip ${SERVER}; setenv gatewayip ${GATEWAY}; setenv hostname ${HOSTNAME}; setenv netmask ${NETMASK}\0" \
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

#endif	/* __CONFIG_H */
