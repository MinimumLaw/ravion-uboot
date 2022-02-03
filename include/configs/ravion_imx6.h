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
 "kernel_addr_r=0x11400000\0" \
"ramdisk_addr_r=0x1b400000\0" \
   "initrd_high=0x1f3fffff\0" \
    "fdt_addr_r=0x1f400000\0" \
      "fdt_high=0x1f7fffff\0" \
 "script_addr_r=0x1f800000\0" \

#ifdef CONFIG_SPL		/* SPL BUILD */
#include "imx6_spl.h"

/* Falcon Mode */
#define CONFIG_SPL_FS_LOAD_ARGS_NAME	"board.dtb"
#define CONFIG_SPL_FS_LOAD_KERNEL_NAME	"board.kernel"
#define CONFIG_SYS_LOAD_ADDR		0x11400000 /* kernel_addr_r */
#define CONFIG_SYS_SPL_ARGS_ADDR	0x1f400000 /* fdt_addr_r */

#define CONFIG_MXC_UART_BASE		UART1_BASE

/* MMC Configuration */
#define CONFIG_SYS_FSL_ESDHC_ADDR	USDHC3_BASE_ADDR

/* Falcon Mode - MMC support: args@1MB kernel@2MB */
#define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTORS	(CONFIG_CMD_SPL_WRITE_SIZE / 512)
#define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR	0x0800 /* 1MB */
#define CONFIG_SYS_MMCSD_RAW_MODE_KERNEL_SECTOR	0x1000 /* 2MB */

#define CONFIG_SYS_USB_FAT_BOOT_PARTITION	1
#else				/* UBOOT BULD */
#define CONFIG_SYS_LOAD_ADDR		0x10400000 /* SDRAM_START + 4Mb */
#endif

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(32 * 1024 * 1024)

/* SATA */
#ifdef CONFIG_CMD_SATA
#define CONFIG_LBA48
#endif
/* I2C Configs */
#define CONFIG_SYS_I2C_SPEED		100000
/* USB Config (Host) */
#define CONFIG_MXC_USB_PORTSC          (PORT_PTS_UTMI | PORT_PTS_PTW)

#undef CONFIG_IPADDR
#undef CONFIG_NETMASK
#undef CONFIG_SERVERIP
#define CONFIG_IPADDR			192.168.5.101
#define CONFIG_NETMASK			255.255.255.0
#define CONFIG_SERVERIP			192.168.5.254

/*
 *     Default do FAST (falcon) mode from 1-st eMMC partition
 * with  kernel  "board.kernel",  DTB  "board.dtb"
 * and rootfs on SECOND SATA disk partition. Console enabled.
 */
#define FALCON_ENV_DEFAULT \
	"bootargs=console=ttymxc0,115200n8 root=/dev/sda2 ro\0" \
	"boot_os=0\0"

#define BLKDEV_BOOTCMD \
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
 * Boot syslinux.conf & script from SECOND partition (slow).
 * 1-st partition MUST contents working mode falcon files,
 * u-boot-dtb.img an uboot.env platform file
 */
#define EMMC_BOOTCMD \
	"emmcboot=" \
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
	"sata init; " \
	"run _scriptboot; run _blkdevboot\0"

#define SPL_UPDATE \
	"prepare_module=mfgr_fuse; " \
	"mmc dev 0 1 && " \
	"mmc write 0x00940000 2 200 && " \
	"mmc bootbus 0 2 0 1 && " \
	"mmc partconf 0 1 1 0 && " \
	"mmc rst-function 0 1 && " \
	"mmc dev 0 0 && "\
	"cfgblock create; " \
	"mmc dev 0 0 && " \
	"mmc write 0x10400000 0 0x20100 && " \
	"setenv bootmenu_4 && " \
	"setenv prepare_module && " \
	"saveenv\0"

#define RECOVERY_BOOT \
	"recovery_boot=" \
	"load mmc 0:1 ${kernel_addr_r} zImage && " \
	"load mmc 0:1 ${ramdisk_addr_r} initrd.img && " \
	"load mmc 0:1 ${fdt_addr_r} i${soc}${variant}-${vendor}-${board}.dtb && " \
	"bootz ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}\0"

#define CHECK_UBOOTENV \
	"check_ubootenv=" \
	"setenv bootmenu_4; " \
	"load mmc 0:1 ${script_addr_r} uboot.env || saveenv\0"

#define BOOTMENU_BOOTCMD \
	"bootmenu_0=Normal boot=run bootcmd\0" \
	"bootmenu_1=TFTP boot=run tftpboot\0" \
	"bootmenu_2=eMMC as USB Storage=ums 0 mmc 0\0" \
	"bootmenu_3=SATA as USB Storage=sata init; ums 0 sata 0\0" \
	"bootmenu_4=Personalise module interactive=run prepare_module\0"

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
	"bootcmd=" \
	    "run check_ubootenv; " \
	    "run usbboot; " \
	    "run sdboot; " \
	    "run emmcboot; " \
	    "run sataboot; " \
	    "run recovery_boot; " \
	    "ums 0 mmc 0\0" \
	"server_path=/cimc/root/colibri-imx6\0" \
	"boot_script_file=bscript.img\0" \
	"board_name=\0" \
	FALCON_ENV_DEFAULT \
	VARIANT \
	MEM_LAYOUT_ENV_SETTINGS \
	BLKDEV_BOOTCMD \
	USB_BOOTCMD \
	SD_BOOTCMD \
	TFTP_BOOTCMD \
	EMMC_BOOTCMD \
	SATA_BOOTCMD \
	BOOTMENU_BOOTCMD \
	CHECK_UBOOTENV \
	RECOVERY_BOOT \
	SPL_UPDATE \
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

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#endif	/* __CONFIG_H */
