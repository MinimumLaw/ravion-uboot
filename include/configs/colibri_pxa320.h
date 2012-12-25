/*
 * Toradex Colibri PXA320 configuration file
 *
 * Copyright (C) 2009 Marek Vasut <marek.vasut@gmail.com>
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
 * High Level Board Configuration Options
 */
#define	CONFIG_CPU_MONAHANS	1	/* Marvell Monahans CPU */
#define	CONFIG_CPU_PXA320	1	/* Marvell Monahans P CPU */
#define	CONFIG_COLIBRI_PXA320	1	/* Colibri PXA320 board */

#define	BOARD_LATE_INIT			/* Set proper Toradex MAC */
#undef	CONFIG_SKIP_RELOCATE_UBOOT
#undef	CONFIG_USE_IRQ
#undef	CONFIG_SKIP_LOWLEVEL_INIT

#define	CONFIG_DISPLAY_CPUINFO
#undef	CONFIG_DISPLAY_BOARDINFO

/*
 * Environment settings
 */
#define	CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 1024*1024)
#define	CONFIG_SYS_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */
#define	CONFIG_SYS_64BIT_VSPRINTF		/* needed for nand_util.c */
#define	CONFIG_ENV_SIZE			0x8000

#define	CONFIG_ENV_OVERWRITE		/* override default environment */

#define	CONFIG_BOOTCOMMAND		"run spo"
//#define	CONFIG_BOOTARGS			"console=tty0 console=ttyS0,115200 root=/dev/sda1 rootdelay=15"
#define	CONFIG_TIMESTAMP
#define	CONFIG_BOOTDELAY		3	/* autoboot delay */
#define CONFIG_AUTOBOOT_KEYED           1
#define CONFIG_AUTOBOOT_PROMPT  \
        "Enter boot password in %d seconds to stop autoboot process\n", bootdelay
#undef CONFIG_AUTOBOOT_DELAY_STR
#define CONFIG_AUTOBOOT_STOP_STR "ravion"

#define	CONFIG_CMDLINE_TAG		/* Commandline ATAG for Linux kernel */
#define CONFIG_SERIAL_TAG		/* Module serial number - used as eth MAC addr */
#define CONFIG_REVISION_TAG		/* Module revision number - used by Toradex Kernel */

#define	CONFIG_SETUP_MEMORY_TAGS	/* DRAN ATAG for Linux kernel */

#define	CONFIG_SYS_NO_FLASH		1

#define	CONFIG_LZMA			/* LZMA compression support */

/*
 * Serial Console Configuration
 * FFUART - the lower serial port on Colibri board
 */
#define	CONFIG_PXA_SERIAL
#define	CONFIG_FFUART			1
#define	CONFIG_BAUDRATE			115200
#define	CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * Bootloader Components Configuration
 */
#include <config_cmd_default.h>

#define	CONFIG_CMD_NET
#define	CONFIG_CMD_ENV
#undef	CONFIG_CMD_IMLS
#define	CONFIG_CMD_MMC
#define	CONFIG_CMD_NAND
#define	CONFIG_CMD_USB

/*#define CONFIG_CMD_BMP
#define	CONFIG_CMD_DISPLAY
*/
/*
 * Networking Configuration
 * AX88696L / NE2000 chip on the Colibri PXA320 board
 */
#ifdef	CONFIG_CMD_NET
#define	CONFIG_CMD_PING
#define	CONFIG_CMD_DHCP

/*-----------------------------------------------------------------------
* Ethernet Adapter AX88796B on module rev before 2.0a
*/
#define CONFIG_DRIVER_AX88796B          1
#define AX88796B_BASE                   0x10000000

/*-----------------------------------------------------------------------
* Ethernet Adapter AX88796C on module rev 2.0a and more
*/
#define CONFIG_DRIVER_AX88796C          1
#define CONFIG_16BIT_MODE               1 /* 16 bit :1 , 8 bit :0 */
#define CONFIG_AX88796B_PIN_COMPATIBLE  1
#define AX88796C_BASE                   0x10000000

/*-----------------------------------------------------------------------
* Both drivers AX88796B and AX88796C use NET_MULTI
*/
#define CONFIG_NET_MULTI                1

#define	CONFIG_BOOTP_BOOTFILESIZE
#define	CONFIG_BOOTP_BOOTPATH
#define	CONFIG_BOOTP_GATEWAY
#define	CONFIG_BOOTP_HOSTNAME

#define	CONFIG_NETMASK			255.255.255.0
#define	CONFIG_IPADDR			192.168.1.52
#define	CONFIG_SERVERIP			192.168.1.51
//#define	CONFIG_ETHADDR			00:11:22:33:44:55
#endif

/*
 * MMC Card Configuration
 */
#ifdef	CONFIG_CMD_MMC
#define CONFIG_MMC
#define	CONFIG_GENERIC_MMC
#define	CONFIG_PXA_MMC_GENERIC
//#define	CONFIG_SYS_MMC_BASE		0xF0000000
#define CONFIG_PARTITIONS
#define	CONFIG_CMD_FAT
#define	CONFIG_CMD_EXT2
#define	CONFIG_DOS_PARTITION
#endif

/*
 * KGDB
 */
#ifdef	CONFIG_CMD_KGDB
#define	CONFIG_KGDB_BAUDRATE		230400		/* speed to run kgdb serial port */
#define	CONFIG_KGDB_SER_INDEX		2		/* which serial port to use */
#endif

/*
 * HUSH Shell Configuration
 */
#define	CONFIG_SYS_HUSH_PARSER		1
#define	CONFIG_SYS_PROMPT_HUSH_PS2	"> "

#define	CONFIG_SYS_LONGHELP				/* undef to save memory	*/
#ifdef	CONFIG_SYS_HUSH_PARSER
#define	CONFIG_SYS_PROMPT		"$ "		/* Monitor Command Prompt */
#else
#define	CONFIG_SYS_PROMPT		"=> "		/* Monitor Command Prompt */
#endif
#define	CONFIG_SYS_CBSIZE		256		/* Console I/O Buffer Size */
#define	CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)	/* Print Buffer Size */
#define	CONFIG_SYS_MAXARGS		16		/* max number of command args */
#define	CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size */
#define	CONFIG_SYS_DEVICE_NULLDEV	1

/*
 * Clock Configuration
 */
#undef	CONFIG_SYS_CLKS_IN_HZ
#define	CONFIG_SYS_HZ			1000

/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define	CONFIG_STACKSIZE		(128*1024)	/* regular stack */
#ifdef	CONFIG_USE_IRQ
#define	CONFIG_STACKSIZE_IRQ		(4*1024)	/* IRQ stack */
#define	CONFIG_STACKSIZE_FIQ		(4*1024)	/* FIQ stack */
#endif

/*
 * DRAM Map
 */
#define	CONFIG_NR_DRAM_BANKS		1	   /* We have 1 bank of DRAM */
#define	PHYS_SDRAM_1			0xa0000000 /* SDRAM Bank #1 */
#define	PHYS_SDRAM_1_SIZE		0x08000000 /* 128 MB */

#define	CONFIG_SYS_DRAM_BASE		0xa0000000 /* CS0 */
#define	CONFIG_SYS_DRAM_SIZE		0x08000000 /* 128 MB DRAM */

#define	CONFIG_SYS_MEMTEST_START	0xa0000000	/* mtest works on whole DRAM */
#define	CONFIG_SYS_MEMTEST_END		0xa8000000

#define	CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_DRAM_BASE + 0x8000) /* default load address */

/*
 * NAND
 */
#ifdef	CONFIG_CMD_NAND

#define	CONFIG_NAND_PXA3XX
#define	CONFIG_NEW_NAND_CODE
#define	CONFIG_SYS_NAND0_BASE		0x0
#undef	CONFIG_SYS_NAND1_BASE

#define CONFIG_SYS_NAND_BASE_LIST	{ CONFIG_SYS_NAND0_BASE }
#define CONFIG_SYS_MAX_NAND_DEVICE	1	/* Max number of NAND devices */

#define CONFIG_SYS_NAND_PROG_ERASE_TO	3000
#define CONFIG_SYS_NAND_OTHER_TO	100
#define CONFIG_SYS_NAND_SENDCMD_RETRY	3
#undef	NAND_ALLOW_ERASE_ALL		/* Allow erasing bad blocks - don't use */

/* Environment is in NAND */
#define	CONFIG_ENV_IS_IN_NAND		1
#define CONFIG_ENV_OFFSET		0x20000
#define	CONFIG_ENV_SECT_SIZE		0x20000

#define __USE_MTD__

#ifdef __USE_MTD__
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE
#define MTDIDS_DEFAULT          	"nand0=pxa3xx_nand-0"

// Choice one: __USE_UBIFS__, __USE_JFFS2__ or __USE_YAFFS2__
#define __USE_UBIFS__
                                        
#ifdef __USE_YAFS2__
#define CONFIG_YAFFS2
#define CONFIG_MTD_NAND_ECC_YAFFS
#define MTDPARTS_DEFAULT                "mtdparts=pxa3xx_nand-0:" \
					    "128K(ipl),"\
                                    	    "1M(u-boot),"\
                                    	    "6M(kernel),"\
                                    	    "896K(reserved),"\
                                    	    "-(fs)"
#define CONFIG_ROOT_DEVICE	"/dev/mtdblock4"
#define CONFIG_ROOT_FSTYPE	"yaffs2"
#endif // __USE_YAFFS2__

#ifdef __USE_JFFS2__
#define CONFIG_CMD_JFFS2
#define CONFIG_JFFS2_NAND 1
#define CONFIG_MTD_NAND_ECC_JFFS2
#define CONFIG_JFFS2_LZO
#define MTDPARTS_DEFAULT                "mtdparts=pxa3xx_nand-0:" \
					    "128K(ipl),"\
                                    	    "1M(u-boot),"\
                                    	    "6M(kernel),"\
                                    	    "896K(reserved),"\
                                    	    "128M(rootfs)"\
                                    	    "-(unused)"
#define CONFIG_ROOT_DEVICE	"/dev/mtdblock4"
#define CONFIG_ROOT_FSTYPE	"jffs2"
#endif // __USE_JFFS2__

#ifdef __USE_UBIFS__
// library
#define CONFIG_RBTREE
#define CONFIG_LZO
// ubi subsys
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define MTDPARTS_DEFAULT                "mtdparts=pxa3xx_nand-0:" \
					    "128K(ipl)ro,"\
					    "256K(u-boot-cfg)," \
					    "640K(u-boot)ro,"\
					    "1010M(ubi),"\
					    "-(reserved)"
#define CONFIG_UBI_MTD		"ubi.mtd=3"
#define CONFIG_ROOT_DEVICE	"ubi0:rootfs"
#define CONFIG_ROOT_FSTYPE	"ubifs"
#endif // __USE_UBIFS__

#endif // __USE_MTD__

#else

#define	CONFIG_ENV_IS_NOWHERE		1

#endif

/*
 * LCD settings
 */
//#define	CONFIG_LCD
//#define	CONFIG_PXA_VGA

/*
 * OHCI USB
 */
#ifdef	CONFIG_CMD_USB
#define	CONFIG_USB_OHCI_NEW
#define	CONFIG_SYS_USB_OHCI_CPU_INIT
#define	CONFIG_SYS_USB_OHCI_BOARD_INIT
#define	CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	3
#define	CONFIG_SYS_USB_OHCI_REGS_BASE	0x4C000000
#define	CONFIG_SYS_USB_OHCI_SLOT_NAME	"colibri320"
#define	CONFIG_USB_STORAGE
#endif

#define CONFIG_EXTRA_ENV_SETTINGS       \
	"__prepared=by Alex A. Mihaylov AKA MinimumLaw, 2012\0" \
	"__produced=by NTC of Schemotecnics NTK PIT\0" \
	"__requsted=by OAO Radioavionica, Saint-Petersburg, Russia\0" \
	"serial#=1234567\0" \
	"rev#=20b\0" \
	"mtdids="MTDIDS_DEFAULT"\0" \
	"mtdparts="MTDPARTS_DEFAULT"\0" \
	"IPADDR=192.168.5.101\0" \
	"NETMSK=255.255.255.0\0" \
	"SERVER=192.168.5.222\0" \
	"GATEWAY=192.168.5.254\0" \
	"HOSTNAME=colibri\0" \
	"NFS_PATH=/colibri\0" \
	"KRN_RAM=kernel_ram.img\0" \
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
	"clean_env=nand erase u-boot-cfg\0" \
	"ldr_net=setenv ipaddr ${IPADDR}; setenv serverip ${SERVER}; setenv gatewayip ${GATEWAY}; setenv hostname ${HOSTNAME}; setenv netmask ${NETMASK}\0" \
	"tftp_get_ram_kern=run ldr_net; tftpboot ${RAM_LD_ADDR} ${SERVER}:${KRN_RAM}\0" \
	"make_mtd_args=run add_basic_args; run add_mtd_dev; run add_mtd_root\0" \
	"make_nfs_args=run add_basic_args; run add_mtd_dev; run add_nfs_root\0" \
	"make_usb_args=run add_basic_args; run add_mtd_dev; run add_usb_root\0" \
	"make_cf_args=run add_basic_args; run add_mtd_dev; run add_cf_root\0" \
	"make_sd_args=run add_basic_args; run add_mtd_dev; run add_sd_root\0" \
	"make_install_args=setenv bootargs ${mtdparts}; run add_ser_cons; run add_sd_root\0" \
	"nfsram=run make_nfs_args; run add_ser_cons; run tftp_get_ram_kern; bootm ${RAM_LD_ADDR}\0" \
	"nfsrom=run make_nfs_args; run add_ser_cons; run boot_from_nand\0" \
	"mtdram=run make_mtd_args; run add_ser_cons; run tftp_get_ram_kern; bootm ${RAM_LD_ADDR}\0" \
	"resque=run make_mtd_args; run add_ser_cons; run boot_from_nand\0" \
	"spo=run make_cf_args; run boot_from_nand\0" \
	"mksys=nand scrub ubi; run make_install_args; mmc init; ext2load mmc 0 ${RAM_LD_ADDR} /boot/${KRN_RAM}; bootm\0"

#endif	/* __CONFIG_H */
