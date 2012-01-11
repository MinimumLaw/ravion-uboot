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
#define	CONFIG_CPU_PXA310	1	/* Marvell Monahans P CPU */
#define	CONFIG_COLIBRI_PXA310	1	/* Colibri PXA310 board */

#undef	BOARD_LATE_INIT
#undef	CONFIG_SKIP_RELOCATE_UBOOT
#undef	CONFIG_USE_IRQ
#undef	CONFIG_SKIP_LOWLEVEL_INIT

#define	CONFIG_DISPLAY_CPUINFO
#undef	CONFIG_DISPLAY_BOARDINFO

/*
 * Environment settings
 */
#define	CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 128*1024)
#define	CONFIG_SYS_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */
#define	CONFIG_SYS_64BIT_VSPRINTF		/* needed for nand_util.c */
#define	CONFIG_ENV_SIZE			0x8000

#define	CONFIG_ENV_OVERWRITE		/* override default environment */

#define	CONFIG_BOOTCOMMAND		"mmc init ; fatload mmc 0 0xa0000000 uImage ; bootm 0xa0000000"
#define	CONFIG_BOOTARGS			"console=tty0 console=ttyS0,115200 root=/dev/sda1 rootdelay=15"
#define	CONFIG_TIMESTAMP
#define	CONFIG_BOOTDELAY		1	/* autoboot delay */
#define	CONFIG_CMDLINE_TAG		/* Commandline ATAG for Linux kernel */
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

#define	CONFIG_DRIVER_AX88796L
#define	CONFIG_DRIVER_NE2000_BASE       0x10000000
#define	CONFIG_NE2000_NOPROM

#define	CONFIG_BOOTP_BOOTFILESIZE
#define	CONFIG_BOOTP_BOOTPATH
#define	CONFIG_BOOTP_GATEWAY
#define	CONFIG_BOOTP_HOSTNAME

#define	CONFIG_NETMASK			255.255.255.0
#define	CONFIG_IPADDR			192.168.1.52
#define	CONFIG_SERVERIP			192.168.1.51
#define	CONFIG_ETHADDR			00:11:22:33:44:55
#endif

/*
 * MMC Card Configuration
 */
#ifdef	CONFIG_CMD_MMC
#define	CONFIG_MMC
#define	CONFIG_PXA_MMC
#define	CONFIG_SYS_MMC_BASE		0xF0000000
#define	CONFIG_CMD_FAT
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
#define	CONFIG_SYS_HZ			3250000		/* Timer @ 3250000 Hz */

#define	CONFIG_SYS_MONAHANS_RUN_MODE_OSC_RATIO		0x8	/* valid values: 8, 16, 24, 31 */
#define	CONFIG_SYS_MONAHANS_TURBO_RUN_MODE_RATIO	0x1	/* valid values: 1, 2 */

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
 *
 * NOTE: PXA310 Colibri has twice as much DRAM, this is handled in the board
 * definition file.
 */
#define	CONFIG_NR_DRAM_BANKS		1	   /* We have 1 bank of DRAM */
#define	PHYS_SDRAM_1			0xa0000000 /* SDRAM Bank #1 */
#define	PHYS_SDRAM_1_SIZE		0x04000000 /* 64 MB */

#define	CONFIG_SYS_DRAM_BASE		0xa0000000 /* CS0 */
#define	CONFIG_SYS_DRAM_SIZE		0x04000000 /* 64 MB DRAM */

#define	CONFIG_SYS_MEMTEST_START	0xa0000000	/* mtest works on whole DRAM */
#define	CONFIG_SYS_MEMTEST_END		0xa4000000

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
#define CONFIG_ENV_OFFSET		0x60000
#define	CONFIG_ENV_SECT_SIZE		0x20000
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
#define	CONFIG_SYS_USB_OHCI_SLOT_NAME	"colibri310"
#define	CONFIG_USB_STORAGE
#endif

#endif	/* __CONFIG_H */
