/*
 * Copyright (c) 2012-2016 Toradex, Inc.
 *
 * Configuration settings for the Toradex Apalis TK1 modules.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>

/* enable PMIC */
#define CONFIG_AS3722_POWER

#include "tegra124-common.h"

/* High-level configuration options */
#define V_PROMPT			"Apalis TK1 # "
#define CONFIG_CUSTOM_BOARDINFO		/* not from device-tree model node */
#undef CONFIG_DISPLAY_BOARDINFO
#define CONFIG_DISPLAY_BOARDINFO_LATE

/* Board-specific serial config */
#define CONFIG_SERIAL_MULTI
#define CONFIG_TEGRA_ENABLE_UARTA
#define CONFIG_SYS_NS16550_COM1		NV_PA_APB_UARTA_BASE

/* Make the HW version stuff available in U-Boot env */
#define CONFIG_VERSION_VARIABLE		/* ver environment variable */
#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
#define CONFIG_CMD_ASKENV

/* I2C */
#define CONFIG_SYS_I2C_TEGRA
#define CONFIG_CMD_I2C

/* SD/MMC support */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_TEGRA_MMC
#define CONFIG_CMD_MMC
#define CONFIG_SUPPORT_EMMC_BOOT	/* eMMC specific */

/* Environment in eMMC, before config block at the end of 1st "boot sector" */
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_ENV_OFFSET		(-CONFIG_ENV_SIZE + \
					 CONFIG_TRDX_CFG_BLOCK_OFFSET)
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_ENV_PART		1

/* USB client support */
#define CONFIG_G_DNL_MANUFACTURER	"Toradex"
#define CONFIG_G_DNL_VENDOR_NUM		0x1b67
#define CONFIG_G_DNL_PRODUCT_NUM	0xffff /* Fallback, set using fixup */

/* USB host support */
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_TEGRA
#define CONFIG_USB_MAX_CONTROLLER_COUNT	3
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_USB

/* PCI host support */
#define CONFIG_PCI
#undef CONFIG_PCI_SCAN_SHOW
#define CONFIG_PCI_TEGRA
#define CONFIG_PCI_PNP
#define CONFIG_CMD_PCI
#define CONFIG_CMD_PCI_ENUM

/* PCI networking support */
#define CONFIG_E1000
#define CONFIG_E1000_NO_NVM

/* General networking support */
#define CONFIG_CMD_NET
#define CONFIG_CMD_DHCP
#define CONFIG_IP_DEFRAG
#define CONFIG_TFTP_BLOCKSIZE		16384
#define CONFIG_TFTP_TSIZE

/* Miscellaneous commands */
#define CONFIG_CMD_SETEXPR
#define CONFIG_FAT_WRITE

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	1
#define CONFIG_ZERO_BOOTDELAY_CHECK
#undef CONFIG_IPADDR
#define CONFIG_IPADDR		192.168.10.2
#define CONFIG_NETMASK		255.255.255.0
#undef CONFIG_SERVERIP
#define CONFIG_SERVERIP		192.168.10.1

#define CONFIG_BOOTCOMMAND \
	"run emmcboot; echo; echo emmcboot failed; " \
	"run sdboot; echo; echo sdboot failed; " \
	"run usbboot; echo; echo usbboot failed; " \
	"run nfsboot; echo; echo nfsboot failed"

#define DFU_ALT_EMMC_INFO	"apalis-tk1.img raw 0x0 0x500 mmcpart 1; " \
				"boot part 0 1 mmcpart 0; " \
				"rootfs part 0 2 mmcpart 0; " \
				"uImage fat 0 1 mmcpart 0; " \
				"tegra30-apalis-eval.dtb fat 0 1 mmcpart 0"

#define EMMC_BOOTCMD \
	"emmcargs=ip=off root=/dev/mmcblk0p2 rw rootfstype=ext3 rootwait\0" \
	"emmcboot=run setup; setenv bootargs ${defargs} ${emmcargs} " \
		"${setupargs} ${vidargs}; echo Booting from internal eMMC " \
		"chip...; run emmcdtbload; load mmc 0:1 ${kernel_addr_r} " \
		"${boot_file} && bootm ${kernel_addr_r} - ${dtbparam}\0" \
	"emmcdtbload=setenv dtbparam; load mmc 0:1 ${fdt_addr_r} " \
		"${soc}-apalis-${fdt_board}.dtb && " \
		"setenv dtbparam ${fdt_addr_r}\0"

#define NFS_BOOTCMD \
	"nfsargs=ip=:::::eth0:on root=/dev/nfs rw netdevwait\0" \
	"nfsboot=run setup; setenv bootargs ${defargs} ${nfsargs} " \
		"${setupargs} ${vidargs}; echo Booting via DHCP/TFTP/NFS...; " \
		"run nfsdtbload; dhcp ${kernel_addr_r} " \
		"&& bootm ${kernel_addr_r} - ${dtbparam}\0" \
	"nfsdtbload=setenv dtbparam; tftp ${fdt_addr_r} " \
		"${soc}-apalis-${fdt_board}.dtb " \
		"&& setenv dtbparam ${fdt_addr_r}\0"

#define SD_BOOTCMD \
	"sdargs=ip=off root=/dev/mmcblk1p2 rw rootfstype=ext3 rootwait\0" \
	"sdboot=run setup; setenv bootargs ${defargs} ${sdargs} ${setupargs} " \
		"${vidargs}; echo Booting from SD card in 8bit slot...; " \
		"run sddtbload; load mmc 1:1 ${kernel_addr_r} " \
		"${boot_file} && bootm ${kernel_addr_r} - ${dtbparam}\0" \
	"sddtbload=setenv dtbparam; load mmc 1:1 ${fdt_addr_r} " \
		"${soc}-apalis-${fdt_board}.dtb " \
		"&& setenv dtbparam ${fdt_addr_r}\0"

#define USB_BOOTCMD \
	"usbargs=ip=off root=/dev/sda2 rw rootfstype=ext3 rootwait\0" \
	"usbboot=run setup; setenv bootargs ${defargs} ${setupargs} " \
		"${usbargs} ${vidargs}; echo Booting from USB stick...; " \
		"usb start && run usbdtbload; load usb 0:1 ${kernel_addr_r} " \
		"${boot_file} && bootm ${kernel_addr_r} - ${dtbparam}\0" \
	"usbdtbload=setenv dtbparam; load usb 0:1 ${fdt_addr_r} " \
		"${soc}-apalis-${fdt_board}.dtb " \
		"&& setenv dtbparam ${fdt_addr_r}\0"

#define BOARD_EXTRA_ENV_SETTINGS \
	"boot_file=uImage\0" \
	"console=ttyS0\0" \
	"defargs=lp0_vec=2064@0xf46ff000 core_edp_mv=1150 core_edp_ma=4000 " \
		"usb_port_owner_info=2 lane_owner_info=6 emc_max_dvfs=0\0" \
	"dfu_alt_info=" DFU_ALT_EMMC_INFO "\0" \
	EMMC_BOOTCMD \
	"fdt_board=eval\0" \
	NFS_BOOTCMD \
	SD_BOOTCMD \
	"setethupdate=if env exists ethaddr; then; else setenv ethaddr " \
		"00:14:2d:00:00:00; fi; tftpboot ${loadaddr} " \
		"flash_eth.img && source ${loadaddr}\0" \
	"setsdupdate=setenv interface mmc; setenv drive 1; mmc rescan; " \
		"load ${interface} ${drive}:1 ${loadaddr} flash_blk.img " \
		"|| setenv drive 2; mmc rescan; load ${interface} ${drive}:1 " \
		"${loadaddr} flash_blk.img && " \
		"source ${loadaddr}\0" \
	"setup=setenv setupargs igb_mac=${ethaddr} " \
		"consoleblank=0 no_console_suspend=1 console=tty1 " \
		"console=${console},${baudrate}n8 debug_uartport=lsport,0 " \
		"${memargs}\0" \
	"setupdate=run setsdupdate || run setusbupdate || run setethupdate\0" \
	"setusbupdate=usb start && setenv interface usb; setenv drive 0; " \
		"load ${interface} ${drive}:1 ${loadaddr} flash_blk.img && " \
		"source ${loadaddr}\0" \
	USB_BOOTCMD \
	"vidargs=video=tegrafb0:640x480-16@60 fbcon=map:1\0"

/* Increase console I/O buffer size */
#undef CONFIG_SYS_CBSIZE
#define CONFIG_SYS_CBSIZE		1024

/* Increase arguments buffer size */
#undef CONFIG_SYS_BARGSIZE
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE

/* Increase print buffer size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/* Increase maximum number of arguments */
#undef CONFIG_SYS_MAXARGS
#define CONFIG_SYS_MAXARGS		32

#define CONFIG_CMD_TIME
#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_ALT_MEMTEST

#define CONFIG_OF_SYSTEM_SETUP

#define CONFIG_SUPPORT_RAW_INITRD
#define CONFIG_SYS_BOOT_RAMDISK_HIGH

#include "tegra-common-usb-gadget.h"
#undef CONFIG_SYS_DFU_DATA_BUF_SIZE
#include "tegra-common-post.h"

#endif /* __CONFIG_H */
