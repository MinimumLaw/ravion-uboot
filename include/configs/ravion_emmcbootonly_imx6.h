/*
 * Copyright 2013-2015 Toradex, Inc.
 *
 * Configuration settings for the Toradex Colibri iMX6
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "mx6_common.h"

#include <asm/arch/imx-regs.h>
#include <asm/mach-imx/gpio.h>

/********************
 * SDRAM Board layout

(9)8000 0000 2Gb
(5)4000 0000 1Gb
(3)2000 0000 512Mb
(2)1000 0000 256Mb
   [   4Mb U-Boot reserved ]
   [   4Mb U-Boot script reserved ]
   [   4Mb U-Boot DTB reserved ]
   [  64Mb U-Boot ramdisk area ]
   [ 160Mb U-Boot kernel Area ]
   [  16Mb U-Boot load area ]
   [   4Mb U-Boot reserved ]
(1)0000 0000 0Mb

SDRAM START - 0x1000 0000
*/

/* don't like mx6_common.h default */
#undef  CONFIG_SYS_LOAD_ADDR

#define MEM_LAYOUT_ENV_SETTINGS \
"loadaddr_low_r=0x10400000\0" \
 "kernel_addr_r=0x11400000\0" \
"ramdisk_addr_r=0x1b400000\0" \
   "initrd_high=0x1f3fffff\0" \
    "fdt_addr_r=0x1f400000\0" \
      "fdt_high=0x1f7fffff\0" \
 "script_addr_r=0x1f800000\0" \

#ifdef CONFIG_SPL		/* SPL BUILD */
#include "imx6_spl.h"
/* SPL Basic params */
#define CONFIG_SYS_LOAD_ADDR		0x11400000  /* kernel_addr_r */
#define CONFIG_MXC_UART_BASE		UART1_BASE  /* debug console */
/* SPL MMC Configuration */
#define CONFIG_SYS_FSL_ESDHC_ADDR	USDHC3_BASE_ADDR
#else				/* UBOOT BULD */
/* U-Bool default load address (16Mb block) */
#define CONFIG_SYS_LOAD_ADDR		0x10400000 /* SDRAM_START + 4Mb */
#endif

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

#undef CONFIG_IPADDR
#undef CONFIG_NETMASK
#undef CONFIG_SERVERIP
#define CONFIG_IPADDR			192.168.5.200
#define CONFIG_NETMASK			255.255.255.0
#define CONFIG_SERVERIP			192.168.5.254

#define BLKDEV_BOOTCMD \
	"efiboot=bootefi bootmgr\0" \
	"_blkdevboot=" \
	"sysboot ${blkname} ${blkdev} any " \
	"${script_addr_r} syslinux.conf\0" \
	"_scriptboot=" \
	"load ${blkname} ${blkdev} ${script_addr_r} " \
	"${boot_script_file} && source ${script_addr_r}\0"

/*
 * USB storage.
 * Boot script & syslinux.conf from SECOND partition (slow).
 * 1-st partition may contents recovery mode falcon files,
 * u-boot-dtb.img
 */
#define USB_BOOTCMD \
	"usbboot=" \
	"setenv blkname usb && " \
	"setenv blkdev 0:1 && " \
	"run _scriptboot; run _blkdevboot\0"

/*
 * SD/MMC card removable storage
 */
#define SD_BOOTCMD \
	"sdboot=" \
	"setenv blkname mmc && " \
	"setenv blkdev 1:1 && " \
	"run _scriptboot; run _blkdevboot\0"

/*
 * Network/TFTP - only bootscript allowed.
 */
#define TFTP_BOOTCMD \
	"tftpboot=" \
	"setenv boot_script_file /boot/bscript.img; "\
	"tftp ${script_addr_r} ${serverip}:${boot_script_file} && " \
	"source ${script_addr_r} || env default boot_script_file\0"

/*
 * eMMC storage.
 * Boot syslinux.conf & script from first and second (compatibe) partition(s).
 */
#define EMMC_BOOTCMD \
	"emmcboot=" \
	"setenv blkname mmc && " \
	"setenv blkdev 0:1 && " \
	"run _scriptboot; run _blkdevboot\0" \
	"emmcboot2=" \
	"setenv blkname mmc && " \
	"setenv blkdev 0:2 && " \
	"run _scriptboot; run _blkdevboot\0"

/*
 * SATA storage.
 * Boot syslinux.conf & script from FIRST partition (slow).
 */
#define SATA_BOOTCMD \
	"sataboot="\
	"setenv blkname sata; " \
	"setenv blkdev 0:1; " \
	"run _scriptboot; run _blkdevboot\0"

#ifdef CONFIG_MX6Q
#define VARIANT	"variant=qp\0"
#else
#define VARIANT	"variant=dl\0"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	"__INF0__=Ravion-V2 I.MX6 CPU Module BSP package\0" \
	"__INF1__=Created: Alex A. Mihaylov AKA MinimumLaw, MinimumLaw@Rambler.Ru\0" \
	"__INF2__=Request: Radioavionica Corp, Saint-Petersburg, Russia, 2022\0" \
	"__INF3__=License: GPL v2 and above, https://github.com/MinimumLaw\0" \
	"board=kitsbimx6\0" \
	"bootcmd=echo BOOT DONE\0" \
	"distro_bootcmd=" \
	    "run efiboot; " \
	    "run usbboot; " \
	    "run sdboot; " \
	    "run emmcboot; " \
	    "run emmcboot2; " \
	    "run sataboot; " \
	    "ums 0 mmc 0\0" \
	"server_path=/cimc/root/colibri-imx6\0" \
	"boot_script_file=bscript.img\0" \
	"board_name=\0" \
	VARIANT \
	MEM_LAYOUT_ENV_SETTINGS \
	BLKDEV_BOOTCMD \
	USB_BOOTCMD \
	SD_BOOTCMD \
	TFTP_BOOTCMD \
	EMMC_BOOTCMD \
	SATA_BOOTCMD \
	"splashpos=m,m\0"

/* Miscellaneous configurable options */
#undef CONFIG_SYS_CBSIZE
#define CONFIG_SYS_CBSIZE		1024
#undef CONFIG_SYS_MAXARGS
#define CONFIG_SYS_MAXARGS		48

/* Physical Memory Map */
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#endif	/* __CONFIG_H */
