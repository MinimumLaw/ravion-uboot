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
#define CONFIG_MX6
#define CONFIG_SYS_THUMB_BUILD
#define CONFIG_USE_ARCH_MEMCPY
#define CONFIG_USE_ARCH_MEMSET

#define CONFIG_COLIBRI_IMX6
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO_LATE

#define CONFIG_SYS_GENERIC_BOARD

#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_SERIAL_TAG
#define CONFIG_SERIAL_TAG_BOARD

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(32 * 1024 * 1024)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_MISC_INIT_R

#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE		UART1_BASE

/* Make the HW version stuff available in U-Boot env */
#define CONFIG_VERSION_VARIABLE		/* ver environment variable */
#define CONFIG_ENV_VARS_UBOOT_CONFIG
#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG

/* I2C Configs */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_SPEED		100000

/* OCOTP Configs */
#define CONFIG_CMD_FUSE
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif

/* MMC Configs */
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR	0
#define CONFIG_SYS_FSL_USDHC_NUM	2

#define CONFIG_SUPPORT_EMMC_BOOT	/* eMMC specific */
#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_FAT_WRITE
#define CONFIG_DOS_PARTITION

/* Network */
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_FEC_MXC
#define CONFIG_MII
#define IMX_FEC_BASE			ENET_BASE_ADDR
#define CONFIG_FEC_XCV_TYPE		RMII
#define CONFIG_ETHPRIME			"FEC"
#define CONFIG_FEC_MXC_PHYADDR		1
#define CONFIG_PHYLIB
#define CONFIG_PHY_MICREL
#define CONFIG_TFTP_TSIZE
#define CONFIG_IP_DEFRAG
#define CONFIG_TFTP_BLOCKSIZE		16384

/* USB Configs */
/* Host */
#define CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX6
#define CONFIG_USB_STORAGE
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_MAX_CONTROLLER_COUNT		2
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET	/* For OTG port */
#define CONFIG_MXC_USB_PORTSC		(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS		0
#define CONFIG_USB_KEYBOARD
#ifdef CONFIG_USB_KEYBOARD
#define CONFIG_SYS_USB_EVENT_POLL
#endif /* CONFIG_USB_KEYBOARD */
/* Client */
#define CONFIG_USB_GADGET
#define CONFIG_USB_GADGET_VBUS_DRAW	2
#define CONFIG_CI_UDC
#define CONFIG_USBD_HS
#define CONFIG_USB_GADGET_DUALSPEED

#define CONFIG_CMD_USB_MASS_STORAGE
#define CONFIG_USB_GADGET_MASS_STORAGE
#define CONFIG_USBDOWNLOAD_GADGET
#define CONFIG_TRDX_VID			0x1B67
#define CONFIG_TRDX_PID_COLIBRI_IMX6	0x0027
#define CONFIG_G_DNL_MANUFACTURER	"Toradex"
#define CONFIG_G_DNL_VENDOR_NUM		CONFIG_TRDX_VID
#define CONFIG_G_DNL_PRODUCT_NUM	CONFIG_TRDX_PID_COLIBRI_IMX6
/* USB DFU */
#define CONFIG_CMD_DFU
#define CONFIG_DFU_FUNCTION
#define CONFIG_DFU_MMC

/* Miscellaneous commands */
#define CONFIG_CMD_BMODE
#define CONFIG_CMD_SETEXPR
#define CONFIG_MXC_GPIO
#define CONFIG_CMD_GPIO
#define CONFIG_CMD_ELF

/* Framebuffer and LCD */
#define CONFIG_VIDEO
#define CONFIG_VIDEO_IPUV3
#define CONFIG_CFB_CONSOLE
#define CONFIG_VGA_AS_SINGLE_DEVICE
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
#define CONFIG_VIDEO_BMP_RLE8
#define CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN
#define CONFIG_BMP_16BPP
#define CONFIG_VIDEO_LOGO
#define CONFIG_VIDEO_BMP_LOGO
#define CONFIG_IPUV3_CLK		260000000
#define CONFIG_CMD_HDMIDETECT
#define CONFIG_CONSOLE_MUX
#define CONFIG_IMX_HDMI
#define CONFIG_IMX_VIDEO_SKIP
#define CONFIG_CMD_BMP

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200

/* Command definition */
#include <config_cmd_default.h>
#undef CONFIG_CMD_LOADB
#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_FLASH

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY		1
#define CONFIG_ZERO_BOOTDELAY_CHECK
#undef CONFIG_IPADDR
#define CONFIG_IPADDR			192.168.5.101
#define CONFIG_NETMASK			255.255.255.0
#undef CONFIG_SERVERIP
#define CONFIG_SERVERIP			192.168.5.222

#define CONFIG_LOADADDR			0x12000000
#define CONFIG_SYS_TEXT_BASE		0x17800000

#ifdef CONFIG_CMD_MMC
#define CONFIG_DRIVE_MMC "mmc "
#else
#define CONFIG_DRIVE_MMC
#endif

#define CONFIG_DRIVE_TYPES CONFIG_DRIVE_MMC

#define MEM_LAYOUT_ENV_SETTINGS \
	"kernel_addr_r=0x11000000\0" \
	"fdt_addr_r=0x12000000\0" \
	"ramdisk_addr_r=0x12100000\0" \
	"script_addr_r=0x12300000\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0"

#define BLKDEV_BOOTCMD \
	"blkdevboot=load ${blkname} ${blkdev} ${script_addr_r} ${boot_script_file} && " \
	"source ${script_addr_r}\0" \

#define USB_BOOTCMD \
	"usbboot=usb start && " \
	"setenv blkname usb && " \
	"setenv blkdev 0:1 && " \
	"run blkdevboot\0" \

#define SD_BOOTCMD \
	"sdboot=setenv blkname mmc && " \
	"setenv blkdev 1:1 && " \
	"run blkdevboot\0" \

#define NFS_BOOTCMD \
	"nfsboot=nfs ${script_addr_r} " \
	"${serverip}:${nfs_root_path}${boot_script_file} && " \
	"source ${script_addr_r}\0" \

#define TFTP_BOOTCMD \
	"tftpboot=tftp ${script_addr_r} ${serverip}:${boot_script_file} && " \
	"source ${script_addr_r}\0" \

#define EMMC_BOOTCMD \
	"emmcboot=setenv blkname mmc && " \
	"setenv blkdev 0:1 && " \
	"run blkdevboot\0" \

#define UBOOT_UPDATE \
	"ubootupdate=if env exists ethaddr; then; else "\
	"setenv ethaddr 00:14:2d:00:00:00; fi; " \
	"tftp ${kernel_addr_r} ${serverip}:u-boot.imx && " \
	"setexpr blkcnt ${filesize} + 0x1ff && " \
	"setexpr blkcnt ${blkcnt} / 0x200 && " \
	"mmc dev 0 1 && " \
	"mmc write ${kernel_addr_r} 2 ${blkcnt} && " \
	"mmc bootbus 0 2 0 1 && " \
	"mmc partconf 0 1 1 0 && " \
	"mmc rst-function 0 1\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootcmd=run usbboot; run sdboot; " \
	    "run emmcboot; " \
	    "ums 0 mmc 0\0" \
	"nfs_root_path=/cimc/root/armv7a-neon/exports\0" \
	"boot_script_file=/boot/bscript.img\0" \
	"console=ttymxc0\0" \
	"defargs=enable_wait_mode=off galcore.contiguousSize=50331648\0" \
	MEM_LAYOUT_ENV_SETTINGS \
	BLKDEV_BOOTCMD \
	USB_BOOTCMD \
	SD_BOOTCMD \
	NFS_BOOTCMD \
	TFTP_BOOTCMD \
	EMMC_BOOTCMD \
	UBOOT_UPDATE \
	"splashpos=m,m\0" \
	"vidargs=video=mxcfb0:dev=lcd,640x480M@60,if=RGB666 " \
		"video=mxcfb1:off fbmem=8M\0 "

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT		"Ravion iMX6 # "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		1024
#define CONFIG_SYS_MAXARGS		48
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#define CONFIG_SYS_ALT_MEMTEST
#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_MEMTEST_START	0x10000000
#define CONFIG_SYS_MEMTEST_END		0x10010000
#define CONFIG_SYS_MEMTEST_SCRATCH	0x10800000

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR

#define CONFIG_CMDLINE_EDITING

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH

#define CONFIG_ENV_SIZE			(8 * 1024)

#define CONFIG_ENV_IS_IN_MMC

#if defined(CONFIG_ENV_IS_IN_MMC)
/* Environment in eMMC, before config block at the end of 1st "boot sector" */
#define CONFIG_ENV_OFFSET		(-CONFIG_ENV_SIZE + \
					 CONFIG_TRDX_CFG_BLOCK_OFFSET)
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_ENV_PART		1
#endif

#define CONFIG_OF_LIBFDT
#define CONFIG_OF_SYSTEM_SETUP

#ifndef CONFIG_SYS_DCACHE_OFF
#define CONFIG_CMD_CACHE
#endif

#define CONFIG_CMD_TIME

#define CONFIG_CMD_BOOTZ
#define CONFIG_SUPPORT_RAW_INITRD
#define CONFIG_CMD_FS_GENERIC

#define CONFIG_CRC32_VERIFY

#endif	/* __CONFIG_H */
