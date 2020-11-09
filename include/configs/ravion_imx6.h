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

#ifdef CONFIG_SPL
#include "imx6_spl.h"

/* Falcon Mode */
#define CONFIG_SPL_FS_LOAD_ARGS_NAME	"args"
#define CONFIG_SPL_FS_LOAD_KERNEL_NAME	"uImage"
#define CONFIG_SYS_SPL_ARGS_ADDR	0x18000000

#define CONFIG_MXC_UART_BASE		UART1_BASE

/* MMC Configuration */
#define CONFIG_SYS_FSL_ESDHC_ADDR	USDHC3_BASE_ADDR

/* Falcon Mode - MMC support: args@1MB kernel@2MB */
#define CONFIG_CMD_SPL_WRITE_SIZE	(128 * SZ_1K)
#define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR	0x0800 /* 1MB */
#define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTORS	(CONFIG_CMD_SPL_WRITE_SIZE / 512)
#define CONFIG_SYS_MMCSD_RAW_MODE_KERNEL_SECTOR	0x1000 /* 2MB */

#define CONFIG_SYS_USB_FAT_BOOT_PARTITION	1

#endif

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_SERIAL_TAG

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
#define CONFIG_IPADDR			192.168.5.101
#define CONFIG_NETMASK			255.255.255.0
#undef CONFIG_SERVERIP
#define CONFIG_SERVERIP			192.168.5.254

#define CONFIG_LOADADDR			0x12000000

#define MEM_LAYOUT_ENV_SETTINGS \
	"kernel_addr_r=0x11000000\0" \
	"fdt_addr_r=0x12000000\0" \
	"ramdisk_addr_r=0x12100000\0" \
	"script_addr_r=0x12300000\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0"

#define BLKDEV_BOOTCMD \
	"blkdevboot=sysboot ${blkname} ${blkdev} any " \
	"${script_addr_r} syslinux.conf || " \
	"load ${blkname} ${blkdev} ${script_addr_r} ${boot_script_file} && " \
	"source ${script_addr_r}\0" \

#define USB_BOOTCMD \
	"usbboot=setenv blkname usb && " \
	"setenv blkdev 0:1 && " \
	"run blkdevboot\0" \

#define SD_BOOTCMD \
	"sdboot=setenv blkname mmc && " \
	"setenv blkdev 1:1 && " \
	"run blkdevboot\0" \

#define NFS_BOOTCMD \
	"nfsboot=nfs ${script_addr_r} " \
	"${serverip}:${server_path}${boot_script_file} && " \
	"source ${script_addr_r}\0" \

#define TFTP_BOOTCMD \
	"tftpboot=tftp ${script_addr_r} ${serverip}:${boot_script_file} && " \
	"source ${script_addr_r}\0" \

#define EMMC_BOOTCMD \
	"emmcboot=setenv blkname mmc && " \
	"setenv blkdev 0:1 && " \
	"run blkdevboot\0" \

#define SATA_BOOTCMD \
	"sataboot=setenv boot_script_file bscript.img; "\
	"setenv blkname sata; " \
	"setenv blkdev 0:1; " \
	"sata init; " \
	"run blkdevboot\0" \

#define SPL_UPDATE \
	"bootcmd_mfg=run usbboot\0" \
	"spl-update=if env exists ethaddr; then; else "\
	"setenv ethaddr 00:14:2d:00:00:00; fi; " \
	"tftp ${kernel_addr_r} ${serverip}:${spl-file} && " \
	"setexpr blkcnt ${filesize} + 0x1ff && " \
	"setexpr blkcnt ${blkcnt} / 0x200 && " \
	"mmc dev 0 1 && " \
	"mmc write ${kernel_addr_r} 2 ${blkcnt} && " \
	"mmc bootbus 0 2 0 1 && " \
	"mmc partconf 0 1 1 0 && " \
	"mmc rst-function 0 1\0"

#define BOOTMENU_BOOTCMD \
	"bootmenu_0=Normal boot=run bootcmd\0" \
	"bootmenu_1=TFTP boot=run tftpboot\0" \
	"bootmenu_2=NFS boot=run nfsboot\0"

#ifdef CONFIG_MX6Q
#define VARIANT	"variant=qp\0"
#else
#define VARIANT	"variant=dl\0"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	VARIANT \
	"board=common\0" \
	"bootcmd=" \
	    "run usbboot; " \
	    "run sdboot; " \
	    "run emmcboot; " \
	    "run sataboot; " \
	    "ums 0 mmc 0\0" \
	"server_path=/cimc/root/colibri-imx6\0" \
	"boot_script_file=/boot/bscript.img\0" \
	"debug=setenv bootargs \"${bootargs} ${defargs} " \
	    "console=tty0 console=${console},${baudrate}n8 \"; " \
	    "printenv bootargs\0" \
	MEM_LAYOUT_ENV_SETTINGS \
	BLKDEV_BOOTCMD \
	USB_BOOTCMD \
	SD_BOOTCMD \
	NFS_BOOTCMD \
	TFTP_BOOTCMD \
	EMMC_BOOTCMD \
	SATA_BOOTCMD \
	BOOTMENU_BOOTCMD \
	"spl-file=SPL\0" \
	SPL_UPDATE \
	"splashpos=m,m\0" \

/* Miscellaneous configurable options */
#undef CONFIG_SYS_CBSIZE
#define CONFIG_SYS_CBSIZE		1024
#undef CONFIG_SYS_MAXARGS
#define CONFIG_SYS_MAXARGS		48

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR

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
