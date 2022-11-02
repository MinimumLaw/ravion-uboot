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

/*****************************************************************************
 * Fixup memory layout
 * DISTRO_XXX requred for :
 *  = fdt_addr		- absent, use $(fdtcontroladdr)
 *  = fdt_addr_r	- defined
 *  = ramdisk_addr_r	- defined
 *  = kernel_addr_r	- defined
 *  = pxefile_addr_r	- defined
 *  = scriptaddr	- definded
 */
#define MEM_LAYOUT_ENV_SETTINGS \
"loadaddr_low_r=0x10400000\0" \
 "kernel_addr_r=0x11400000\0" \
"ramdisk_addr_r=0x1b400000\0" \
   "initrd_high=0x1f3fffff\0" \
    "fdt_addr_r=0x1f400000\0" \
      "fdt_high=0x1f7fffff\0" \
"pxefile_addr_r=0x1f800000\0" \
    "scriptaddr=0x1f800000\0" \

/* don't like mx6_common.h default */
#undef  CONFIG_SYS_LOAD_ADDR
#define CONFIG_SYS_LOAD_ADDR		0x10400000 /* $load_addr_low_r */

#ifdef CONFIG_SPL		/* SPL BUILD */
/* #warning SPL Build */
#include "imx6_spl.h"
/* SPL Basic params */
#define CONFIG_MXC_UART_BASE		UART1_BASE  /* debug console */
/* SPL MMC Configuration */
#define CONFIG_SYS_FSL_ESDHC_ADDR	USDHC3_BASE_ADDR
#endif

#define BOOT_TARGET_DEVICES(func) \
        func(USB, usb, 0) \
        func(MMC, mmc, 1) \
        func(MMC, mmc, 0) \
        func(SATA, sata, 0)
#include <config_distro_bootcmd.h>

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

#undef CONFIG_IPADDR
#undef CONFIG_NETMASK
#undef CONFIG_SERVERIP
#define CONFIG_IPADDR			192.168.5.200
#define CONFIG_NETMASK			255.255.255.0
#define CONFIG_SERVERIP			192.168.5.254

/*
 * Network/TFTP - only bootscript allowed.
 */
#define TFTP_BOOTCMD \
	"bootcmd_tftp0=" \
	"run tftpboot\0" \
	"tftpboot=" \
	    "for prefix in ${boot_prefixes}; do " \
		"for script in ${boot_scripts}; do " \
		    "if tftp ${scriptaddr} ${serverip}:${prefix}${script}; then " \
			"echo Found ${prefix}${script} on tftpserver; " \
			"source ${scriptaddr}; " \
		    "fi; " \
		"done;" \
	    "done\0"

#ifdef CONFIG_MX6Q
#define VARIANT	"variant=qp\0"
#else
#define VARIANT	"variant=dl\0"
#endif

/*****************************************************************************
 * Required for back compatible
 ****************************************************************************/
#define DISTRO_FIXUP \
	"board=kitsbimx6\0" \
	"ravion_fixup=" \
	    "env info -d && saveenv; " \
	    "setenv boot_syslinux_conf syslinux.conf; " \
	    "setenv boot_scripts bscript.img; " \
	    "setenv boot_targets ${boot_targets} tftp0; " \
	    "setenv nfs_path /cimc/root/colibri-imx6; " \
	    "setenv fdtfile i${soc}${variant}-${vendor}-${board}.dtb\0" \
	"bootcmd=run ravion_fixup; run distro_bootcmd\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"__INF0__=Ravion-V2 I.MX6 CPU Module BSP package\0" \
	"__INF1__=Created: Alex A. Mihaylov AKA MinimumLaw, MinimumLaw@Rambler.Ru\0" \
	"__INF2__=Request: Radioavionica Corp, Saint-Petersburg, Russia, 2022\0" \
	"__INF3__=License: GPL v2 and above, https://github.com/MinimumLaw\0" \
	MEM_LAYOUT_ENV_SETTINGS \
	VARIANT \
	BOOTENV \
	TFTP_BOOTCMD \
	DISTRO_FIXUP

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
